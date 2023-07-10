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
      sprintf((char *)buf, "kd %s", keyname[keyboardData.keycode]);
      return strlen(buf);
    }
    case 0:{
      // KeyUp
      //Log("keyup: %d -> \"%s\"", keyboardData.keycode, keyname[keyboardData.keycode]);
      sprintf((char *)buf, "ku %s", keyname[keyboardData.keycode]);
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
  return strlen(buf);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
