#include <am.h>
#include "riscv/riscv.h"

#define AM_RV_NPC_KBD_ADDR 0xa0000060

#define KEYDOWN_MASK 0x8000
#define KEYCODE_MASK 0x7FFF

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t keyborad_data = inl(AM_RV_NPC_KBD_ADDR);
  kbd->keydown = keyborad_data & KEYDOWN_MASK;
  kbd->keycode = keyborad_data & KEYCODE_MASK;
}
