#include <fs.h>

//#define FS_LOG

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENT, FD_DISPINFO, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write, 0},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write, 0},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write, 0},
  [FD_EVENT]  = {"/dev/events", 0, 0, events_read, invalid_write, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write, 0},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write, 0},
#include "files.h"
};

int fs_open(const char *pathname, int flags, int mode){
  int fileTabLen = LENGTH(file_table);
  #ifdef FS_LOG
  Log("Size of file-table is %d", fileTabLen);
  #endif
  for(int i = 0; i < fileTabLen; i = i + 1){
    if(strcmp(pathname, file_table[i].name) == 0){
      #ifdef FS_LOG
      Log("Find file \"%s\" with index %d", file_table[i].name, i);
      #endif
      //file_table->open_offset = 0;
      return i;
    }
  }
  // should not reach here
  panic("No such file in file-table");
}

int fs_close(int fd){
  #ifdef FS_LOG
  Log("Close file \"%s\" with fd = %d", file_table[fd].name, fd);
  #endif
  file_table[fd].open_offset = 0;
  return 0;
}

size_t fs_read(int fd, void *buf, size_t len){
  assert(fd >= 0);

  Finfo *file = &file_table[fd];
  assert(file);

  if(file->read != NULL){
    return file->read(buf, 0, len);
  }

  // check read will not exceed the boundry of file
  if(file_table[fd].open_offset + len > file_table[fd].size){
    size_t old_len = len;
    len = file_table[fd].size - file_table[fd].open_offset;
    #ifdef FS_LOG
    Log("File No.%d -> %s: read exceed size, reduce read length from %d to %d", fd, file_table[fd].name, old_len, len);
    #endif
  }

  // calculate the offset in ramdisk
  size_t ramdiskOffset = file_table[fd].disk_offset + file_table[fd].open_offset;

  // call ramdisk_read to read, the length of read is ret
  size_t ret = ramdisk_read(buf, ramdiskOffset, len);

  // add open_offset to how much we read
  file_table[fd].open_offset = file_table[fd].open_offset + ret;

  // return the read length
  return ret;
}

size_t fs_lseek(int fd, size_t offset, int whence){
  // SEEK_SET: The file offset is set to offset bytes
  // SEEK_CUR: The file offset is set to its current location plus offset bytes
  // SEEK_END: The file offset is set to the size of the file plus offset bytes
  switch(whence){
    case SEEK_SET:{
      size_t oldOffset = file_table[fd].open_offset;
      file_table[fd].open_offset = offset;
      #ifdef FS_LOG
      Log("File No.%d -> \"%s\": change open_offset %x -> %x (SEEK_SET)", fd, file_table[fd].name, oldOffset, file_table[fd].open_offset);
      #endif
      return file_table[fd].open_offset;
      break;
    }
    case SEEK_CUR:{
      size_t oldOffset = file_table[fd].open_offset;
      file_table[fd].open_offset = oldOffset + offset;
      #ifdef FS_LOG
      Log("File No.%d -> \"%s\": change open_offset %x -> %x (SEEK_CUR)", fd, file_table[fd].name, oldOffset, file_table[fd].open_offset);
      #endif
      return file_table[fd].open_offset;
      break;
    }
    case SEEK_END:{
      size_t oldOffset = file_table[fd].open_offset;
      file_table[fd].open_offset = file_table[fd].size + offset;
      #ifdef FS_LOG
      Log("File No.%d -> \"%s\": change open_offset %x -> %x (SEEK_END)", fd, file_table[fd].name, oldOffset, file_table[fd].open_offset);
      #endif
      return file_table[fd].open_offset;
      break;
    }
    default: panic("Wrong lseek() operation");
  }
  return 0;
}

size_t fs_write(int fd, const void *buf, size_t len){
  assert(fd >= 0);

  Finfo *file = &file_table[fd];

  if(file->write != NULL){
    return file->write(buf, file->open_offset, len);
  }

  // check write will not exceed the boundry of file
  if(file_table[fd].open_offset >= file_table[fd].size){
    #ifdef FS_LOG
    panic("Write file No.%d -> \"%s\" with offset %x exceed size %x", fd, file_table[fd].name, (file_table[fd].open_offset + len), file_table[fd].size);
    #endif
    return -1;
  }

  if(file_table[fd].open_offset + len > file_table[fd].size){
    size_t old_len = len;
    len = file_table[fd].size - file_table[fd].open_offset;
    #ifdef FS_LOG
    Log("File No.%d -> %s: read exceed size, reduce read length from %d to %d", fd, file_table[fd].name, old_len, len);
    #endif
  }

  // calculate the offset in ramdisk
  size_t ramdiskOffset = file_table[fd].disk_offset + file_table[fd].open_offset;

  // call ramdisk_write to write, the length of write is ret
  size_t ret = ramdisk_write(buf, ramdiskOffset, len);

  // add open_offset to how much we write
  file_table[fd].open_offset = file_table[fd].open_offset + ret;

  // return the write length
  return ret;
}

void init_fs() {
  AM_GPU_CONFIG_T GPUConfig = io_read(AM_GPU_CONFIG);
  int sizeOfFB = GPUConfig.width * GPUConfig.height * 4; // each pixel is a 32 bit integer!
  Log("GPU: width = %d, height = %d -> FrameBuffer size = %d", GPUConfig.width, GPUConfig.height, sizeOfFB);
  file_table[FD_FB].size = sizeOfFB;
  //assert(file_table[FD_FB].size > 0);
}
