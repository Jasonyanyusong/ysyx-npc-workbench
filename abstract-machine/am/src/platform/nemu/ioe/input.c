#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000
#define KEYCODE_MASK 0x7FFF

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t keyborad_data = inl(KBD_ADDR);
  kbd->keydown = keyborad_data & KEYDOWN_MASK;
  kbd->keycode = keyborad_data & KEYCODE_MASK;
}
