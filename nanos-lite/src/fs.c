#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

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
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write, 0},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write, 0},
#include "files.h"
};

int fs_open(const char *pathname, int flags, int mode){
  int fileTabLen = LENGTH(file_table);
  Log("Size of file-table is %d", fileTabLen);
  for(int i = 0; i < fileTabLen; i = i + 1){
    if(strcmp(pathname, file_table[i].name) == 0){
      Log("Find file \"%s\" with index %d", file_table[i].name, i);
      return i;
    }
  }
  // should not reach here
  panic("No such file in file-table");
}

int fs_close(int fd){
  Log("Close file \"%s\" with fd = %d", file_table[fd].name, fd);
  return 0;
}

void init_fs() {
  // TODO: initialize the size of /dev/fb
}
