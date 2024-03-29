#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  size_t size = 0;
  for(size = 0; len > 0; len = len - 1){
    putch(((char *)buf)[size]);
    size = size + 1;
  }
  return size;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  // TODO: change to snprintf when kilb is implemented!
  //Log("In function");
  size_t sizeCount = 0;
  AM_INPUT_KEYBRD_T keyboardData = io_read(AM_INPUT_KEYBRD);
  if(keyboardData.keycode == 0){
    //Log("No Keyboard Event");
    return 0;
  }
  switch(keyboardData.keydown){
    case 1:{
      // KeyDown
      //Log("keydown: %d -> \"%s\"", keyboardData.keycode, keyname[keyboardData.keycode]);
      //sprintf((char *)buf, "kd %s", keyname[keyboardData.keycode]);
      sprintf((char *)buf, "kd %d", keyboardData.keycode);
      return strlen(buf);
    }
    case 0:{
      // KeyUp
      //Log("keyup: %d -> \"%s\"", keyboardData.keycode, keyname[keyboardData.keycode]);
      //sprintf((char *)buf, "ku %s", keyname[keyboardData.keycode]);
      sprintf((char *)buf, "ku %d", keyboardData.keycode);
      return strlen(buf);
    }
    default: assert(0); return -1;
  }
  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  // TODO: read from am
  AM_GPU_CONFIG_T GPUConfig = io_read(AM_GPU_CONFIG);
  sprintf(buf, "WIDTH:%d\nHEIGHT:%d\n",GPUConfig.width, GPUConfig.height);
  Log("%s\n", buf);
  return strlen(buf);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T GPUConfig = io_read(AM_GPU_CONFIG);

  int writeRow = -1;
  int writeColumn = -1;
  writeRow    = (offset / 4) / GPUConfig.width;
  writeColumn = (offset / 4) % GPUConfig.width;
  //writeColumn = 10;
  //Log("offset = %d, writeRow = %d, writeColumn = %d, len = %d", offset, writeRow, writeColumn, len);

  AM_GPU_FBDRAW_T frameBuffer;
  frameBuffer.x = writeColumn;
  frameBuffer.y = writeRow;

  frameBuffer.pixels = buf;
  frameBuffer.w = len / 4;
  frameBuffer.h = 1;
  frameBuffer.sync = true;

  //Log("x = %d, y = %d, w = %d, h = %d", frameBuffer.x, frameBuffer.y, frameBuffer.w, frameBuffer.h);

  io_write(AM_GPU_FBDRAW, frameBuffer.x, frameBuffer.y, frameBuffer.pixels, frameBuffer.w, frameBuffer.h, frameBuffer.sync);
  return 0;


  /*AM_GPU_CONFIG_T HighPerformanceGPUConfig = io_read(AM_GPU_CONFIG);

  AM_GPU_FBDRAW_T HighPerformanceFrameBuffer;
  HighPerformanceFrameBuffer.pixels = (void *)buf;
  HighPerformanceFrameBuffer.sync = true;

  HighPerformanceFrameBuffer.x = offset % HighPerformanceGPUConfig.width;
  HighPerformanceFrameBuffer.y = offset / HighPerformanceGPUConfig.width;
  HighPerformanceFrameBuffer.w = len >> 32;
  HighPerformanceFrameBuffer.h = len & 0x00000000FFFFFFFF;

  io_write(AM_GPU_FBDRAW, HighPerformanceFrameBuffer.x, HighPerformanceFrameBuffer.y,\
   HighPerformanceFrameBuffer.pixels, HighPerformanceFrameBuffer.w, HighPerformanceFrameBuffer.h, HighPerformanceFrameBuffer.sync);

  return 0;*/
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
