#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

// let's treat hight equals to height
// so the code appears more beautiful

uint32_t am_gpu_width = 0;
uint32_t am_gpu_hight = 0;

#define  am_gpu_width_mask 0xFFFF0000
#define  am_gpu_hight_mask 0x0000FFFF

void __am_gpu_init() {
  uint32_t am_gpu_config = inl(VGACTL_ADDR);
  am_gpu_width = (am_gpu_config & am_gpu_width_mask) >> 16;
  am_gpu_hight = am_gpu_config & am_gpu_hight_mask;
  int i;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for(i = 0; i < am_gpu_width * am_gpu_hight; i = i + 1){
    fb[i] = i;
  }
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  //printf("am_gpu_width = %d, am_gpu_hight = %d\n", am_gpu_width, am_gpu_hight);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true,
    .has_accel = false,
    .width  = am_gpu_width,
    .height = am_gpu_hight,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
