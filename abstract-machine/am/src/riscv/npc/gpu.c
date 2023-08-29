#include <am.h>
#include "riscv/riscv.h"

#define VGACTL_ADDR 0xa0000100
#define FB_ADDR     0xa1000000

#define SYNC_ADDR (VGACTL_ADDR + 4)

uint32_t am_gpu_width = 0;
uint32_t am_gpu_hight = 0;

#define  am_gpu_width_mask 0xFFFF0000
#define  am_gpu_hight_mask 0x0000FFFF

void __am_gpu_init() {
  uint32_t am_gpu_config = inl(VGACTL_ADDR);
  am_gpu_width = (am_gpu_config & am_gpu_width_mask) >> 16;
  am_gpu_hight = am_gpu_config & am_gpu_hight_mask;
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true,
    .has_accel = false,
    .width  = am_gpu_width,
    .height = am_gpu_hight,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t* fb = (uint32_t*)(uintptr_t)FB_ADDR;
  uint32_t* pixels = (uint32_t*)(ctl->pixels);
  int x = ctl -> x, y = ctl -> y;
  int w = ctl -> w, h = ctl -> h;
  for(int j = 0; j < h; j = j + 1){
    for(int i = 0; i < w; i = i + 1){
      fb[(y + j) * am_gpu_width + (x + i)] = *(pixels + j * w + i);
    }
  }
  if (ctl->sync) {
    outl(VGACTL_ADDR + 4, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
