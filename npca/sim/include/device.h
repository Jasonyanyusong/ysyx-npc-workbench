/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing No.101 High School
* Copyright (c) 2023 Yusong Yan, University of Washington - Seattle
*
* NSIM is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include<common.h>

//---------- Differencial Testing ----------

enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };

void diff_difftest_init();
void diff_difftest_one_exec();
bool diff_difftest_check_reg();

bool diff_skip_ref_exec = false;

void (*ref_difftest_memcpy)(uint64_t addr, void *buf, size_t n, bool direction) = NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;
void (*ref_difftest_raise_intr)(uint64_t NO) = NULL;

//---------- Host timer ----------

uint64_t host_timer_boot_time = 0;
uint64_t host_timer_get_time_internal();
uint64_t host_timer_get_time();

//---------- Device-Timer (RTC) ----------

#define DEVICE_RTC_ADDR_LO 0xa0000048
#define DEVICE_RTC_ADDR_HI 0xa000004c
uint32_t device_timer_write_time_to_sim(bool low_high); // When is false, return low 4 bytes, else high 4 bytes

//---------- Devices ----------

void device_init_map();
void device_init_serial();
void device_init_timer();
void device_vga_update_screen();
void device_update();
void device_init_devices();

uint64_t device_last = 0;

void device_sdl_clear_event_queue();

//********** Serial Definitions **********

void device_serial_io_handler(uint32_t offset, int len, bool is_write);

#define DEVICE_SERIAL_BASE     0xa00003f8
#define DEVICE_SERIO_CH_OFFSET 0
static uint8_t *device_serial_base = NULL;


//********** RTC Timer Definitions **********

void device_rtc_io_handler(uint32_t offset, int len, bool is_write);
void device_rtc_time_interrupt(); // Not implemented yet

#define DEVICE_RTC_BASE        0xa0000048
static uint32_t *device_rtc_port_base = NULL;

//typedef uint16_t ioaddr_t;

//********** Keyboard Definitions **********

#define DEVICE_KEYBOARD_I8042_BASE 0xa0000060

#define DEVICE_KEYBOARD_KEYDOWN_MASK 0x8000
#define DEVICE_KEYBOARD_KEYCODE_MASK 0x7fff

#define _KEYS(f) \
    f(ESCAPE) f(F1) f(F2) f(F3) f(F4) f(F5) f(F6) f(F7) f(F8) f(F9) f(F10) f(F11) f(F12) \
f(GRAVE) f(1) f(2) f(3) f(4) f(5) f(6) f(7) f(8) f(9) f(0) f(MINUS) f(EQUALS) f(BACKSPACE) \
f(TAB) f(Q) f(W) f(E) f(R) f(T) f(Y) f(U) f(I) f(O) f(P) f(LEFTBRACKET) f(RIGHTBRACKET) f(BACKSLASH) \
f(CAPSLOCK) f(A) f(S) f(D) f(F) f(G) f(H) f(J) f(K) f(L) f(SEMICOLON) f(APOSTROPHE) f(RETURN) \
f(LSHIFT) f(Z) f(X) f(C) f(V) f(B) f(N) f(M) f(COMMA) f(PERIOD) f(SLASH) f(RSHIFT) \
f(LCTRL) f(APPLICATION) f(LALT) f(SPACE) f(RALT) f(RCTRL) \
f(UP) f(DOWN) f(LEFT) f(RIGHT) f(INSERT) f(DELETE) f(HOME) f(END) f(PAGEUP) f(PAGEDOWN)

#define _KEY_NAME(k) _KEY_##k,

// It's conflicted on macos with sys/_types/_key_t.h
#ifdef __APPLE__
  #undef _KEY_T 
#endif

enum{
    _KEY_NONE = 0,
    _KEY_ESCAPE = 1,
    _KEY_F1 = 2,
    _KEY_F2 = 3,
    _KEY_F3 = 4, 
    _KEY_F4 = 5, 
    _KEY_F5 = 6, 
    _KEY_F6 = 7, 
    _KEY_F7 = 8, 
    _KEY_F8 = 9, 
    _KEY_F9 = 10, 
    _KEY_F10 = 11, 
    _KEY_F11 = 12, 
    _KEY_F12 = 13, 
    _KEY_GRAVE = 14, 
    _KEY_1 = 15,
    _KEY_2 = 16,
    _KEY_3 = 17,
    _KEY_4 = 18, 
    _KEY_5 = 19, 
    _KEY_6 = 20, 
    _KEY_7 = 21, 
    _KEY_8 = 22, 
    _KEY_9 = 23, 
    _KEY_0 = 24, 
    _KEY_MINUS = 25, 
    _KEY_EQUALS = 26, 
    _KEY_BACKSPACE = 27, 
    _KEY_TAB = 28, 
    _KEY_Q = 29, 
    _KEY_W = 30, 
    _KEY_E = 31, 
    _KEY_R = 32, 
    _KEY_T = 33, 
    _KEY_Y = 34, 
    _KEY_U = 35, 
    _KEY_I = 36, 
    _KEY_O = 37, 
    _KEY_P = 38, 
    _KEY_LEFTBRACKET = 39, 
    _KEY_RIGHTBRACKET = 40, 
    _KEY_BACKSLASH = 41, 
    _KEY_CAPSLOCK = 42, 
    _KEY_A = 43, 
    _KEY_S = 44, 
    _KEY_D = 45, 
    _KEY_F = 46, 
    _KEY_G = 47, 
    _KEY_H = 48, 
    _KEY_J = 49, 
    _KEY_K = 50, 
    _KEY_L = 51, 
    _KEY_SEMICOLON = 52, 
    _KEY_APOSTROPHE = 53, 
    _KEY_RETURN = 54, 
    _KEY_LSHIFT = 55, 
    _KEY_Z = 56,
    _KEY_X = 57,
    _KEY_C = 58,
    _KEY_V = 59,
    _KEY_B = 60,
    _KEY_N = 61,
    _KEY_M = 62, 
    _KEY_COMMA = 63, 
    _KEY_PERIOD = 64, 
    _KEY_SLASH = 65, 
    _KEY_RSHIFT = 66, 
    _KEY_LCTRL = 67, 
    _KEY_APPLICATION = 68, 
    _KEY_LALT = 69, 
    _KEY_SPACE = 70, 
    _KEY_RALT = 71, 
    _KEY_RCTRL = 72, 
    _KEY_UP = 73, 
    _KEY_DOWN = 74, 
    _KEY_LEFT = 75, 
    _KEY_RIGHT = 76, 
    _KEY_INSERT = 77, 
    _KEY_DELETE = 78, 
    _KEY_HOME = 79, 
    _KEY_END = 80, 
    _KEY_PAGEUP = 81, 
    _KEY_PAGEDOWN = 82,
    //MAP(_KEYS, _KEY_NAME)
};

#define SDL_KEYMAP(k) device_keyboard_keymap[concat(SDL_SCANCODE_, k)] = concat(_KEY_, k);
static uint32_t device_keyboard_keymap[256] = {};

void device_keyboard_init_keymap(); // is necessary or send key will fail

#define DEVICE_KEYBOARD_KEY_QUEUE_LEN 1024
static int device_keyboard_key_queue[DEVICE_KEYBOARD_KEY_QUEUE_LEN] = {};
static int device_keyboard_key_f = 0, device_keyboard_key_r = 0;

void device_keyboard_key_enqueue(uint32_t am_scancode);
uint32_t device_keyboard_key_dequeue();
void device_keyboard_send_key(uint8_t scancode, bool is_keydown);

uint32_t *device_keyboard_i8042_data_port_base = NULL;
void device_keyboard_i8042_data_io_handler(uint32_t offset, int len, bool is_write);
void device_keyboard_init_i8042();

//********** VGA Definitions **********

#define DEVICE_VGA_SHOW_SCREEN true
#define DEVICE_VGA_SCREEN_W 400
#define DEVICE_VGA_SCREEN_H 300

#define DEVICE_VGA_CTRL_BASE 0xa0000100
#define DEVICE_VGA_FB_ADDR   0xa1000000

uint32_t device_vga_screen_width();
uint32_t device_vga_screen_height();
uint32_t device_vga_screen_size();

SDL_Renderer *device_vga_renderer = NULL;
SDL_Texture  *device_vga_texture  = NULL;

void device_vga_init_screen();
void device_vga_update_screen();
void device_vga_vga_update_screen();
void device_vga_init_vga();

void *device_vga_vmem = NULL;
uint32_t *device_vga_ctl_port_base = NULL;

//---------- Device: Map & MMIO----------

#define DEVICE_MAP_IO_SPACE_MAX (2 * 1024 * 1024)
#define DEVICE_NR_MAP 16

#define PAGE_SHIFT        12
#define PAGE_SIZE         (1ul << PAGE_SHIFT)
#define PAGE_MASK         (PAGE_SIZE - 1)

static uint8_t *device_map_io_space = NULL;
static uint8_t *device_map_p_space  = NULL;

typedef void(*io_callback_t)(uint32_t, int, bool);

typedef struct {
    const char *name;
    uint64_t low;
    uint64_t high;
    void *space;
    io_callback_t callback;
} IOMap;

static IOMap device_maps[DEVICE_NR_MAP] = {};
static int device_nr_map = 0;

static bool device_map_inside(IOMap *map, uint64_t addr);
static int  device_find_mapID_by_addr(IOMap *maps, int size, uint64_t addr);

static IOMap* device_mmio_fetch_mmio_map(uint64_t addr);

void device_report_mmio_overlap(const char* name1, uint64_t left1, uint64_t right1, const char* name2, uint64_t left2, uint64_t right2);
void device_add_mmio_map(const char *name, uint64_t addr, void *space, uint32_t len, io_callback_t callback);

void device_mmio_check_bound(IOMap *map, uint64_t addr);

uint64_t device_map_read(uint64_t addr, int len, IOMap *map);
void device_map_write(uint64_t addr, int len, uint64_t data, IOMap *map);

void device_mmio_invoke_callback(io_callback_t c, uint64_t offset, int len, bool is_write);

uint64_t device_mmio_read(uint64_t addr, int len);
void device_mmio_write(uint64_t addr, int len, uint64_t data);

uint8_t* device_map_new_space(int size);
//static void device_check_bound(IOMap *map, uint64_t addr);
//static void device_invoke_callback(io_callback_t c, uint64_t offset, int len, bool is_write);

//---------- Device-Serial ----------

#define DEVICE_SERIAL_ADDR 0xa00003f8
void device_serial_putchar(uint64_t device_serial_mem_write_data);