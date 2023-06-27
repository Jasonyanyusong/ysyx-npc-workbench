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

/*Usage:
    Linux: "verilator -Wno-lint -Wno-style --cc --exe --build --trace nsim.cpp npc.v -LDFLAGS -lreadline -LDFLAGS -lSDL2"
*/

//========== Macro Configurations ==========
#define mem_start_addr  0x080000000
#define mem_end_addr    0x08fffffff
#define mem_size        mem_end_addr - mem_start_addr + 1

#define print_debug_informations true
#define generate_dump_wave_file  false

#define difftest_enable true

#define device_have_serial   true
#define device_have_rtc      true
#define device_have_keyboard true
#define device_have_vga      true
#define device_have_audio    false
#define device_have_disk     false
#define device_have_sdcard   false

//#define trace_enable_itrace 
//#define trace_enable_mtrace 
//#define trace_enable_rtrace 
//#define trace_enable_dtrace 

//========== Macros ==========

#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)

#define BITMASK(bits) ((1ull << (bits)) - 1)
#define BITS(x, hi, lo) (((x) >> (lo)) & BITMASK((hi) - (lo) + 1)) // similar to x[hi:lo] in verilog
#define SEXT(x, len) ({ struct { int64_t n : len; } __x = { .n = x }; (uint64_t)__x.n; })

#define MAP(c, f) c(f)

//========== Include Headers ==========

#include "verilated.h"
#include "verilated_vcd_c.h"
#include "../obj_dir/Vnpc.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <getopt.h>
#include <dlfcn.h>
#include <sys/time.h> // Host timer
#include <SDL2/SDL.h> // Used for IO devices

//========== List functions and variables that will be used later ==========

//---------- Memory manipulations ----------
static inline uint64_t mem_host_read(uint8_t *mem_addr, int mem_length);
static void mem_host_write(void *mem_addr, int mem_length, uint64_t mem_data);
bool mem_addr_in_bound(uint32_t mem_addr);

uint64_t mem_vaddr_ifetch(uint64_t mem_addr, int mem_length);
uint64_t mem_vaddr_read(uint64_t mem_addr, int mem_length);
void mem_vaddr_write(uint64_t mem_addr, int mem_length, uint64_t mem_data);

uint8_t* mem_guest_to_host(uint64_t paddr);
uint64_t mem_host_to_guest(uint8_t *haddr);
uint8_t* mem_guest_to_host(uint64_t mem_physical_addr);
uint64_t mem_host_to_guest(uint8_t *mem_host_addr);
uint64_t mem_pmem_read(uint64_t mem_addr, int mem_length);
void mem_pmem_write(uint64_t mem_addr, int mem_length, uint64_t mem_data);
uint64_t mem_paddr_read(uint64_t mem_addr, int mem_length);
void mem_paddr_write(uint64_t mem_addr, int mem_length, uint64_t mem_data);

void mem_init_mem();

//---------- Statistics ----------

uint64_t statistics_nr_exec = 0;
uint64_t statistics_time_exec = 0;

void statistics_show();

//---------- Simple Debugger (SDB) Monitor ----------

char* monitor_log_file = NULL;
char* monitor_diff_so_file = NULL;
char* monitor_img_file = NULL;
char* monitor_elf_file = NULL;
char* monitor_das_file = NULL;
int   monitor_difftest_port = 1234;
long  monitor_img_size = -1;

bool  monitor_script_mode = false;

long monitor_load_img();
int monitor_parse_args(int argc, char*argv[]);
void monitor_init_monitor(int argc, char*argv[]);

//---------- Simple Debugger (SDB) User Interface ----------

//int sdb_is_batch_mode = false;

char* sdb_rl_gets();

int sdb_cmd_c(char* args); // continue execution
int sdb_cmd_s(char* args); // single-step execution
int sdb_cmd_i(char* args); // informations (register and watchpoint)
int sdb_cmd_x(char* args); // scan and print memory
int sdb_cmd_p(char* args); // expression evaluation
int sdb_cmd_w(char* args); // add watchpoint
int sdb_cmd_d(char* args); // delete watchpoint
int sdb_cmd_q(char* args); // quit NSIM
int sdb_cmd_h(char* args); // help

static struct {
    const char *name;
    const char *description;
    int (*handler) (char *);
} sdb_cmd_table [] = {
    { "h", "help",                                     sdb_cmd_h},
    { "c", "continue execution",                       sdb_cmd_c},
    { "q", "quit NSIM",                                sdb_cmd_q},
    { "s", "single-step execution",                    sdb_cmd_s},
    { "i", "informations (register and watchpoint)",   sdb_cmd_i},
    { "x", "scan and print memory",                    sdb_cmd_x},
    { "p", "expression evaluation",                    sdb_cmd_p},
    { "w", "add watchpoint",                           sdb_cmd_w},
    { "d", "delete watchpoint",                        sdb_cmd_d},
};

#define SDB_NR_CMD 9

//void sdb_set_batch_mode();
void sdb_main_loop();
void sdb_init_sdb();

//---------- NSIM states ----------

enum {NSIM_CONTINUE = 11, NSIM_STOP = 12, NSIM_ABORT = 13, NSIM_END = 14, NSIM_HALT = 15, NSIM_QUIT = 16};

/*
Informations about NSIM sate:
NSIM_CONTINUE indicates that NSIM will continue normally
NSIM_STOP indicates that watchpoint(s)'s value changed, so the simulation will be paused, which can be manually continued
NSIM_ABORT indicates that NPC failed DiffTest, so we stop the simualation, which can not be manually continued
NSIM_END indicates that NPC's program execution have finished, so we stop the simulation, which can be restarted by quit NSIM and run again
NSIM_HALT indicates that NPC have raised an error, so we stop the simulation, which can not be manually continued
NSIM_QUIT indicates that user quit SIM using SDB Q command
*/

typedef struct{
    int state;
    uint64_t halt_pc;
    uint32_t halt_ret;
} NSIMState;

NSIMState nsim_state;

bool state_check_can_continue();
void state_set_state(int state_get_state);
void state_show_state();

//---------- Register manipulations ----------

typedef struct{
    int index;
    uint64_t value;
    //char alias[4];
} NSIMGetGPR;

NSIMGetGPR nsim_gpr[32];

uint64_t reg_pc, reg_snpc, reg_dnpc;

void reg_get_reg_from_sim();
void reg_get_pcreg_from_sim();
void reg_display(bool sdb_print_regs);

//---------- RTL simulation ----------

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;
static Vnpc* top;

typedef struct{
    uint64_t gpr[32];
    uint64_t pc = mem_start_addr;
} riscv64_CPU_State;

riscv64_CPU_State cpu;

void sim_sim_init();
void sim_sim_exit();
void sim_one_exec();
void sim_step_and_dump_wave();

//---------- Image loading ----------

static const uint32_t img [] = {
  0x00000297,  // auipc t0,0
  0x0002b823,  // sd  zero,16(t0)
  0x0102b503,  // ld  a0,16(t0)
  0x00100073,  // ebreak (used as nemu_trap)
  0xdeadbeef,  // some data
};

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

//---------- Trace ----------

uint64_t trace_pc = 0;

void trace_init_trace();

void trace_itrace_init();
void trace_itrace_write(uint32_t itrace_inst);

void trace_mtrace_init();
void trace_mtrace_write(uint64_t mtrace_addr, bool mtrace_is_write, uint64_t mtrace_data, int mtrace_len);

typedef struct{
    uint64_t gpr[32];
    uint64_t pc;
} trace_rtrace_context;
void trace_rtrace_init();
void trace_rtrace_write(trace_rtrace_context rtrace_context);

void trace_dtrace_init();
void trace_dtrace_write(char* dtrace_device_name, uint64_t dtrace_addr, int dtrace_len, bool dtrace_is_write, uint64_t dtrace_data);

//========== Trace ==========

void trace_init_trace(){
    printf("[trace] initializing tracer(s)\n");
    trace_itrace_init();
    trace_mtrace_init();
    trace_rtrace_init();
    trace_dtrace_init();
    return;
}

void trace_itrace_init(){
    printf("[trace-itrace] initializing\n");
    if(remove("itrace.txt") == 0){
        printf("[trace-itrace] previous itrace record deleted\n");
    }
    return;
}

void trace_itrace_write(uint32_t itrace_inst){
    FILE *trace_itrace_file = fopen("itrace.txt", "a+");
    if(trace_itrace_file == NULL){
        // should not reach here!
        printf("[trace-itrace] error: itrace file can not be opened\n");
        assert(0);
        return;
    }
    char itrace_to_be_written[128];
    sprintf(itrace_to_be_written, "[itrace] pc: 0x%lx, inst: 0x%x\n", trace_pc, itrace_inst);
    fputs(itrace_to_be_written, trace_itrace_file);
    fclose(trace_itrace_file);
    return;
}

void trace_mtrace_init(){
    printf("[trace-mtrace] initializing\n");
    if(remove("mtrace.txt") == 0){
        printf("[trace-mtrace] previous mtrace record deleted\n");
    }
    return;
}

void trace_mtrace_write(uint64_t mtrace_addr, bool mtrace_is_write, uint64_t mtrace_data, int mtrace_len){
    FILE *trace_mtrace_file = fopen("mtrace.txt", "a+");
    if(trace_mtrace_file == NULL){
        // should not reach here!
        printf("[trace-mtrace] error: mtrace file can not be opened\n");
        assert(0);
        return;
    }
    char mtrace_to_be_written[128];
    if(mtrace_is_write == true){
        sprintf(mtrace_to_be_written, "[mtrace] pc: 0x%lx, mem_W, addr = 0x%lx, len = %d, data = 0x%lx\n", trace_pc, mtrace_addr, mtrace_len, mtrace_data);
    }else{
        sprintf(mtrace_to_be_written, "[mtrace] pc: 0x%lx, mem_R, addr = 0x%lx, len = %d, data = 0x%lx\n", trace_pc, mtrace_addr, mtrace_len, mtrace_data);
    }
    fputs(mtrace_to_be_written, trace_mtrace_file);
    fclose(trace_mtrace_file);
    return;
}

void trace_rtrace_init(){
    printf("[trace-rtrace] initializing\n");
    if(remove("rtrace.txt") == 0){
        printf("[trace-rtrace] previous rtrace record deleted\n");
    }
    return;
}

void trace_rtrace_write(trace_rtrace_context rtrace_context){
    FILE *trace_rtrace_file = fopen("rtrace.txt", "a+");
    if(trace_rtrace_file == NULL){
        // should not reach here!
        printf("[trace-rtrace] error: rtrace file can not be opened\n");
        assert(0);
        return;
    }
    char rtrace_to_be_written[1024];
    sprintf(rtrace_to_be_written, "[trace-rtrace] pc: 0x%lx, ", trace_pc);
    for(int i = 0; i < 32; i = i + 1){
        char rtrace_single_gpr[64];
        sprintf(rtrace_single_gpr, "x%2d: 0x%16lx, ", i, rtrace_context.gpr[i]);
        strcat(rtrace_to_be_written, rtrace_single_gpr);
    }
    char rtrace_pc_reg[64];
    sprintf(rtrace_pc_reg,  "pc: 0x%16lx\n\0", trace_pc);
    strcat(rtrace_to_be_written, rtrace_pc_reg);
    fputs(rtrace_to_be_written, trace_rtrace_file);
    fclose(trace_rtrace_file);
    return;
}

void trace_dtrace_init(){
    printf("[trace-dtrace] initializing\n");
    if(remove("dtrace.txt") == 0){
        printf("[trace-dtrace] previous dtrace record deleted\n");
    }
    return;
}

void trace_dtrace_write(char* dtrace_device_name, uint64_t dtrace_addr, int dtrace_len, bool dtrace_is_write, uint64_t dtrace_data){
    FILE *trace_dtrace_file = fopen("dtrace.txt", "a+");
    if(trace_dtrace_file == NULL){
        // should not reach here!
        printf("[trace-dtrace] error: dtrace file can not be opened\n");
        assert(0);
        return;
    }
    char mtrace_to_be_written[128];
    if(dtrace_is_write == true){
        sprintf(mtrace_to_be_written, "[dtrace] pc: 0x%lx, mmio_W, name = \"%s\", addr = 0x%lx, len = %d, data = 0x%lx\n", trace_pc, dtrace_device_name, dtrace_addr, dtrace_len, dtrace_data);
    }else{
        sprintf(mtrace_to_be_written, "[dtrace] pc: 0x%lx, mmio_R, name = \"%s\", addr = 0x%lx, len = %d, data = 0x%lx\n", trace_pc, dtrace_device_name, dtrace_addr, dtrace_len, dtrace_data);
    }
    fputs(mtrace_to_be_written, trace_dtrace_file);
    fclose(trace_dtrace_file);
    return;
}

//========== Statistics ==========

void statistics_show(){
    printf("[statistics] total insturction executed by NSIM: %ld instructions\n", statistics_nr_exec);
    printf("[statistics] total time cost for execution instructions: %ld us\n", statistics_time_exec);
    if(statistics_time_exec == 0){
        printf("[statistics] time is so low, can not show statistics\n");
    }else{
        printf("[statistics] NSIM simulation frequency: %ld inst/second\n", (statistics_nr_exec * 1000000 / statistics_time_exec));
    }
    return;
}

//========== Device: Map & MMIO ==========

static bool device_map_inside(IOMap *map, uint64_t addr){
    //printf("At pc = 0x%8lx, addr: 0x%8x, Map \"%s\" [0x%8x,0x%8x]\n", cpu.pc, addr, map -> name, map -> low, map -> high);
    return (addr >= map->low && addr <= map->high);
}

int device_find_mapID_by_addr(IOMap *maps, int size, uint64_t addr){
    int i;
    //printf("[find_mapid_by_addr] paddr is 0x%8x\n", addr);
    for(i = 0; i < size; i = i + 1){
        if(device_map_inside(maps + i, addr)){
            diff_skip_ref_exec = true;
            //difftest_skip_ref(); //As we use NEMU to difftest, the behavior of decices are same with NSIM
            return i;
        }
    }
    // should not reach here!
    assert(0);
    return -1;
}

static IOMap* device_mmio_fetch_mmio_map(uint64_t addr){
    //printf("[fetch_mmio_map] paddr is 0x%lx\n", addr);
    //printf("device_nr_map = %d\n", device_nr_map);
    int mapid = device_find_mapID_by_addr(device_maps, device_nr_map, addr);
    //printf("[device-mmio-fetch-mmio-map]\n");
    //printf("[fetch_mmio_map] mapid = %d\n", mapid);
    return (mapid == -1 ? NULL : &device_maps[mapid]);
}

void device_report_mmio_overlap(const char* name1, uint64_t left1, uint64_t right1, const char* name2, uint64_t left2, uint64_t right2){
    printf("[device] error: MMIO region \"%s\"@[0x%lx,0x%lx] is overlapped with \"%s\"@[0x%lx,0x%lx]\n", name1, left1, right1, name2, left2, right2);
    // should not reach here!
    assert(0);
    return;
}

void device_add_mmio_map(const char *name, uint64_t addr, void *space, uint32_t len, io_callback_t callback){
    assert(device_nr_map < DEVICE_NR_MAP); // so that we have space to add another device
    uint64_t left = addr;
    uint64_t right = addr + len - 1;
    //printf("[device] name \"%s\", left 0x%lx, right 0x%lx check before add to mmio map\n", name, left, right);
    if(mem_addr_in_bound(left) || mem_addr_in_bound(right)){
        // should not reach here!
        device_report_mmio_overlap(name, left, right, "pmem", mem_start_addr, mem_end_addr);
        assert(0);
        return;
    }
    for(int i = 0; i < device_nr_map; i = i + 1){
        if(left <= device_maps[i].high && right >= device_maps[i].low){
            // should not reach here!
            device_report_mmio_overlap(name, left, right, device_maps[i].name, device_maps[i].low, device_maps[i].high);
            assert(0);
            return;
        }
    }
    //printf("[device] name \"%s\", left 0x%lx, right 0x%lx check passed, adding to mmio map\n", name, left, right);
    device_maps[device_nr_map] = (IOMap){ .name = name, .low = addr, .high = addr + len - 1, .space = space, .callback = callback};
    device_nr_map = device_nr_map + 1;
    printf("[device] name \"%s\", left 0x%lx, right 0x%lx, added to mmio map\n", name, left, right);
    return;
}

void device_mmio_check_bound(IOMap *map, uint64_t addr){
    if(map == NULL){
        // should not reach here!
        printf("[device] error: IOMap is NULL\n");
        assert(0);
        return;
    }
    else{
        if(addr > map -> high || addr < map -> low){
            // should not reach here!
            printf("[device] error: address 0x%lx is out of bound @\"%s\"[0x%lx,0x%lx]\n", addr, map -> name, map -> low, map -> high);
            assert(0);
            return;
        }
    }
}

void device_mmio_invoke_callback(io_callback_t c, uint64_t offset, int len, bool is_write){
    if (c != NULL) { c(offset, len, is_write); }
    return;
}

uint64_t device_map_read(uint64_t addr, int len, IOMap *map){
    assert(len >= 1 && len <= 8);
    device_mmio_check_bound(map, addr);
    uint64_t offset = addr - map -> low;
    device_mmio_invoke_callback(map -> callback, offset, len, false);
    uint64_t ret = mem_host_read((uint8_t *)map -> space + offset, len);
    //printf("[device-map-read] read map \"%s\", at addr = 0x%lx, with len = %d, ret = 0x%lx\n", map -> name, addr, len, ret);
    return ret;
}

void device_map_write(uint64_t addr, int len, uint64_t data, IOMap *map){
    assert(len >= 1 && len <= 8);
    //if(strcmp(map -> name, "vmem") != 0) {printf("[device-map-write] write map \"%s\", at addr = 0x%lx, with len = %d, data = 0x%lx\n", map -> name, addr, len, data);}
    device_mmio_check_bound(map, addr);
    uint64_t offset = addr - map -> low;
    //if(strcmp(map -> name, "vmem") != 0) {printf("[device-map-write] offset = 0x%lx\n", offset);}
    mem_host_write(map -> space + offset, len, data);
    device_mmio_invoke_callback(map -> callback, offset, len, true);
    return;
}

uint64_t device_mmio_read(uint64_t addr, int len){
    if(print_debug_informations) {printf("[mmio_read] paddr is 0x%lx\n", addr);}
    return device_map_read(addr, len, device_mmio_fetch_mmio_map(addr));
}

void device_mmio_write(uint64_t addr, int len, uint64_t data){
    if(print_debug_informations) {printf("[mmio_write] paddr is 0x%lx\n", addr);}
    //if(addr == 0xa0000100 || addr == 0xa0000104){printf("[device-mmio-write] catch vgactl write, addr = 0x%lx, len = %d, data = 0x%lx\n", addr, len, data);}
    device_map_write(addr, len, data, device_mmio_fetch_mmio_map(addr));
    //if(addr == 0xa0000100 || addr == 0xa0000104) {printf("device_vga_ctl_port_base[0] = 0x%x, device_vga_ctl_port_base[1] = 0x%x\n", device_vga_ctl_port_base[0], device_vga_ctl_port_base[1]);}
    return;
}

uint8_t* device_map_new_space(int size){
    uint8_t *p = device_map_p_space;
    size = (size + (PAGE_SIZE - 1)) & ~PAGE_MASK;
    device_map_p_space += size;
    assert(device_map_p_space - device_map_io_space < DEVICE_MAP_IO_SPACE_MAX);
    return p;
}

//========== Devices ==========

void device_init_map(){
    device_map_io_space = (uint8_t *)malloc(DEVICE_MAP_IO_SPACE_MAX);
    assert(device_map_io_space);
    device_map_p_space = device_map_io_space;
    return;
}

void device_init_devices(){
    device_init_map();
    if(device_have_serial)   {device_init_serial();}
    if(device_have_rtc)      {device_init_timer();}
    if(device_have_keyboard) {device_keyboard_init_i8042();}
    if(device_have_vga)      {device_vga_init_vga();}
    return;
}

void device_update(){
    uint64_t device_now = host_timer_get_time();
    if(device_now - device_last < (1000000 / 60)){
        return;
    }
    device_last = device_now;

    if(device_have_vga){
        device_vga_vga_update_screen();
    }
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:{
                state_set_state(NSIM_STOP);
                break;
            }
            case SDL_KEYDOWN:
            case SDL_KEYUP:{
                if(device_have_keyboard){
                    //printf("[device-keyboard] SDL event catch key with scancode %d, have event \"%s\"\n", event.key.keysym.scancode, event.key.type == SDL_KEYDOWN ? "KEY DOWN" : "KEY UP");
                    uint8_t k = event.key.keysym.scancode;
                    bool is_keydown = (event.key.type == SDL_KEYDOWN);
                    device_keyboard_send_key(k, is_keydown);
                    break;
                }
                else{
                    break;
                }
                break;
            }
            default: break;
        }
    }
    return;
}

void device_sdl_clear_event_queue(){
    SDL_Event event;
    while (SDL_PollEvent(&event));
}

//========== Device: Serial ==========

void device_serial_io_handler(uint32_t offset, int len, bool is_write){
    assert(len == 1);
    switch(offset){
        case DEVICE_SERIO_CH_OFFSET:
            if (is_write == true) {putc((char)device_serial_base[0], stderr);}
            else {printf("[decive-serio] error: serio do not support read\n"); assert(0);}
            break;
        default: {printf("[device-serio] error: do not support offset = %d\n", offset); assert(0); break;}
    }
}

void device_init_serial(){
    device_serial_base = device_map_new_space(8);
    device_add_mmio_map("serial", DEVICE_SERIAL_BASE, device_serial_base, 8, device_serial_io_handler);
    return;
}

//========== Device: RTC Timer ==========

void device_init_timer(){
    device_rtc_port_base = (uint32_t *)device_map_new_space(8);
    device_add_mmio_map("rtc-timer", DEVICE_RTC_BASE, device_rtc_port_base, 8, device_rtc_io_handler);
    //device_add_alarm_handle(device_rtc_time_interrupt);
    return;
}

void device_rtc_io_handler(uint32_t offset, int len, bool is_write){
    assert(offset == 0 || offset == 4);
    if(!is_write && offset == 4){
        uint64_t us = host_timer_get_time();
        device_rtc_port_base[0] = (uint32_t)us;
        device_rtc_port_base[1] = us >> 32;
    }
    return;
}

void device_rtc_time_interrupt(){
    // TODO
    assert(0);
    return;
} // Not implemented yet

//========== Device: Keyboard i8042 ==========

void device_keyboard_key_enqueue(uint32_t am_scancode){
    device_keyboard_key_queue[device_keyboard_key_r] = am_scancode;
    device_keyboard_key_r = (device_keyboard_key_r + 1) % DEVICE_KEYBOARD_KEY_QUEUE_LEN;
    if(device_keyboard_key_r != device_keyboard_key_f){
        // should not reach here, however, this does not seems to be wrong, so do not assert :)
        //printf("[device-keyboard] error: key queue overflow\n");
        //assert(0);
        return;
    }
    return;
} 

uint32_t device_keyboard_key_dequeue(){
    uint32_t key = _KEY_NONE;
    if(device_keyboard_key_f != device_keyboard_key_r){
        key = device_keyboard_key_queue[device_keyboard_key_f];
        device_keyboard_key_f = (device_keyboard_key_f + 1) % DEVICE_KEYBOARD_KEY_QUEUE_LEN;
    }
    return key;
}

void device_keyboard_send_key(uint8_t scancode, bool is_keydown){
    /*if(nsim_state.state == NSIM_CONTINUE){
        printf("[device-keyboard] nsim's state is NSIM_CONTINUE\n");
    }
    if(device_keyboard_keymap[scancode] != _KEY_NONE){
        printf("[device-keyboard] device_keyboard_keymap[scancode] != _KEY_NONE\n");
    }
    printf("[device-keyboard] device_keyboard_keymap[scancode] = %d\n", device_keyboard_keymap[scancode]);*/
    if(nsim_state.state == NSIM_CONTINUE && device_keyboard_keymap[scancode] != _KEY_NONE){
        uint32_t am_scancode = device_keyboard_keymap[scancode] | (is_keydown ? DEVICE_KEYBOARD_KEYDOWN_MASK : 0);
        //printf("[device-keyboard] send key with am_scancode = %d\n", am_scancode);
        device_keyboard_key_enqueue(am_scancode);
    }
    return;
}

void device_keyboard_i8042_data_io_handler(uint32_t offset, int len, bool is_write){
    if(is_write){
        // should not reach here
        printf("[device-keyboard] error: can not handle write to keyboard MMIO\n");
        assert(0);
        return;
    }
    if(offset != 0){
        // should not reach here
        printf("[device-keyboard] error: can not process offset %d, only can process offset = 0\n", offset);
        assert(0);
        return;
    }
    device_keyboard_i8042_data_port_base[0] = device_keyboard_key_dequeue();
}

void device_keyboard_init_i8042(){
    device_keyboard_i8042_data_port_base = (uint32_t *)device_map_new_space(4);
    device_keyboard_i8042_data_port_base[0] = _KEY_NONE;
    device_add_mmio_map("keyboard", DEVICE_KEYBOARD_I8042_BASE, device_keyboard_i8042_data_port_base, 4, device_keyboard_i8042_data_io_handler);
    device_keyboard_init_keymap();
}

void device_keyboard_init_keymap(){
    //MAP(_KEYS, SDL_KEYMAP)
    device_keyboard_keymap[SDL_SCANCODE_ESCAPE] = _KEY_ESCAPE;
    device_keyboard_keymap[SDL_SCANCODE_F1] = _KEY_F1;
    device_keyboard_keymap[SDL_SCANCODE_F2] = _KEY_F2;
    device_keyboard_keymap[SDL_SCANCODE_F3] = _KEY_F3;
    device_keyboard_keymap[SDL_SCANCODE_F4] = _KEY_F4;
    device_keyboard_keymap[SDL_SCANCODE_F5] = _KEY_F5;
    device_keyboard_keymap[SDL_SCANCODE_F6] = _KEY_F6;
    device_keyboard_keymap[SDL_SCANCODE_F7] = _KEY_F7;
    device_keyboard_keymap[SDL_SCANCODE_F8] = _KEY_F8;
    device_keyboard_keymap[SDL_SCANCODE_F9] = _KEY_F9;
    device_keyboard_keymap[SDL_SCANCODE_F10] = _KEY_F10;
    device_keyboard_keymap[SDL_SCANCODE_F11] = _KEY_F11;
    device_keyboard_keymap[SDL_SCANCODE_F12] = _KEY_F12;
    
    device_keyboard_keymap[SDL_SCANCODE_GRAVE] = _KEY_GRAVE;
    device_keyboard_keymap[SDL_SCANCODE_1] = _KEY_1;
    device_keyboard_keymap[SDL_SCANCODE_2] = _KEY_2;
    device_keyboard_keymap[SDL_SCANCODE_3] = _KEY_3;
    device_keyboard_keymap[SDL_SCANCODE_4] = _KEY_4;
    device_keyboard_keymap[SDL_SCANCODE_5] = _KEY_5;
    device_keyboard_keymap[SDL_SCANCODE_6] = _KEY_6;
    device_keyboard_keymap[SDL_SCANCODE_7] = _KEY_7;
    device_keyboard_keymap[SDL_SCANCODE_8] = _KEY_8;
    device_keyboard_keymap[SDL_SCANCODE_9] = _KEY_9;
    device_keyboard_keymap[SDL_SCANCODE_0] = _KEY_0;
    device_keyboard_keymap[SDL_SCANCODE_MINUS] = _KEY_MINUS;
    device_keyboard_keymap[SDL_SCANCODE_EQUALS] = _KEY_EQUALS;
    device_keyboard_keymap[SDL_SCANCODE_BACKSPACE] = _KEY_BACKSPACE;
    
    device_keyboard_keymap[SDL_SCANCODE_TAB] = _KEY_TAB;
    device_keyboard_keymap[SDL_SCANCODE_Q] = _KEY_Q;
    device_keyboard_keymap[SDL_SCANCODE_W] = _KEY_W;
    device_keyboard_keymap[SDL_SCANCODE_E] = _KEY_E;
    device_keyboard_keymap[SDL_SCANCODE_R] = _KEY_R;
    device_keyboard_keymap[SDL_SCANCODE_T] = _KEY_T;
    device_keyboard_keymap[SDL_SCANCODE_Y] = _KEY_Y;
    device_keyboard_keymap[SDL_SCANCODE_U] = _KEY_U;
    device_keyboard_keymap[SDL_SCANCODE_I] = _KEY_I;
    device_keyboard_keymap[SDL_SCANCODE_O] = _KEY_O;
    device_keyboard_keymap[SDL_SCANCODE_P] = _KEY_P;
    device_keyboard_keymap[SDL_SCANCODE_LEFTBRACKET] = _KEY_LEFTBRACKET;
    device_keyboard_keymap[SDL_SCANCODE_RIGHTBRACKET] = _KEY_RIGHTBRACKET;
    device_keyboard_keymap[SDL_SCANCODE_BACKSLASH] = _KEY_BACKSLASH;
    
    device_keyboard_keymap[SDL_SCANCODE_CAPSLOCK] = _KEY_CAPSLOCK;
    device_keyboard_keymap[SDL_SCANCODE_A] = _KEY_A;
    device_keyboard_keymap[SDL_SCANCODE_S] = _KEY_S;
    device_keyboard_keymap[SDL_SCANCODE_D] = _KEY_D;
    device_keyboard_keymap[SDL_SCANCODE_F] = _KEY_F;
    device_keyboard_keymap[SDL_SCANCODE_G] = _KEY_G;
    device_keyboard_keymap[SDL_SCANCODE_H] = _KEY_H;
    device_keyboard_keymap[SDL_SCANCODE_J] = _KEY_J;
    device_keyboard_keymap[SDL_SCANCODE_K] = _KEY_K;
    device_keyboard_keymap[SDL_SCANCODE_L] = _KEY_L;
    device_keyboard_keymap[SDL_SCANCODE_SEMICOLON] = _KEY_SEMICOLON;
    device_keyboard_keymap[SDL_SCANCODE_APOSTROPHE] = _KEY_APOSTROPHE;
    device_keyboard_keymap[SDL_SCANCODE_RETURN] = _KEY_RETURN;

    device_keyboard_keymap[SDL_SCANCODE_LSHIFT] = _KEY_LSHIFT;
    device_keyboard_keymap[SDL_SCANCODE_Z] = _KEY_Z;
    device_keyboard_keymap[SDL_SCANCODE_X] = _KEY_X;
    device_keyboard_keymap[SDL_SCANCODE_C] = _KEY_C;
    device_keyboard_keymap[SDL_SCANCODE_V] = _KEY_V;
    device_keyboard_keymap[SDL_SCANCODE_B] = _KEY_B;
    device_keyboard_keymap[SDL_SCANCODE_N] = _KEY_N;
    device_keyboard_keymap[SDL_SCANCODE_M] = _KEY_M;
    device_keyboard_keymap[SDL_SCANCODE_COMMA] = _KEY_COMMA;
    device_keyboard_keymap[SDL_SCANCODE_PERIOD] = _KEY_PERIOD;
    device_keyboard_keymap[SDL_SCANCODE_SLASH] = _KEY_SLASH;
    device_keyboard_keymap[SDL_SCANCODE_RSHIFT] = _KEY_RSHIFT;
    
    device_keyboard_keymap[SDL_SCANCODE_LCTRL] = _KEY_LCTRL;
    device_keyboard_keymap[SDL_SCANCODE_APPLICATION] = _KEY_APPLICATION;
    device_keyboard_keymap[SDL_SCANCODE_LALT] = _KEY_LALT;
    device_keyboard_keymap[SDL_SCANCODE_SPACE] = _KEY_SPACE;
    device_keyboard_keymap[SDL_SCANCODE_RALT] = _KEY_RALT;
    device_keyboard_keymap[SDL_SCANCODE_RCTRL] = _KEY_RCTRL;
    
    device_keyboard_keymap[SDL_SCANCODE_UP] = _KEY_UP;
    device_keyboard_keymap[SDL_SCANCODE_DOWN] = _KEY_DOWN;
    device_keyboard_keymap[SDL_SCANCODE_LEFT] = _KEY_LEFT;
    device_keyboard_keymap[SDL_SCANCODE_RIGHT] = _KEY_RIGHT;
    device_keyboard_keymap[SDL_SCANCODE_INSERT] = _KEY_INSERT;
    device_keyboard_keymap[SDL_SCANCODE_DELETE] = _KEY_DELETE;
    device_keyboard_keymap[SDL_SCANCODE_HOME] = _KEY_HOME;
    device_keyboard_keymap[SDL_SCANCODE_END] = _KEY_END;
    device_keyboard_keymap[SDL_SCANCODE_PAGEUP] = _KEY_PAGEUP;
    device_keyboard_keymap[SDL_SCANCODE_PAGEDOWN] = _KEY_PAGEDOWN;
    return;
}

//========== Device: VGA ==========

uint32_t device_vga_screen_width(){
    return DEVICE_VGA_SCREEN_W;
}

uint32_t device_vga_screen_height(){
    return DEVICE_VGA_SCREEN_H;
}

uint32_t device_vga_screen_size(){
    return device_vga_screen_width() * device_vga_screen_height() * sizeof(uint32_t);
}

void device_vga_init_screen(){
    SDL_Window *device_vga_window = NULL;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(DEVICE_VGA_SCREEN_W * 2, DEVICE_VGA_SCREEN_H * 2, 0, &device_vga_window, &device_vga_renderer);
    SDL_SetWindowTitle(device_vga_window, "YSYX NPC (Baisc Level) Simulator By Jasonyanyusong");
    device_vga_texture = SDL_CreateTexture(device_vga_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, DEVICE_VGA_SCREEN_W, DEVICE_VGA_SCREEN_H);
    return;
}

void device_vga_update_screen(){
    //if(true) {printf("[device-vga] calling SDL2 functions to update screen\n");}
    SDL_UpdateTexture(device_vga_texture, NULL, device_vga_vmem, DEVICE_VGA_SCREEN_W * sizeof(uint32_t));
    SDL_RenderClear(device_vga_renderer);
    SDL_RenderCopy(device_vga_renderer, device_vga_texture, NULL, NULL);
    SDL_RenderPresent(device_vga_renderer);
}

void device_vga_vga_update_screen(){
    //if(print_debug_informations) {printf("[device-vga] vga sync register hold val = 0x%x\n", device_vga_ctl_port_base[1]);}
    if(device_vga_ctl_port_base[1] != 0){
        //if(true) {printf("[device-vga] ready to update screen\n");}
        if(DEVICE_VGA_SHOW_SCREEN) {device_vga_update_screen();}
        device_vga_ctl_port_base[1] = 0;
        return;
    }
    return;
}

void device_vga_init_vga(){
    device_vga_ctl_port_base = (uint32_t *)device_map_new_space(8);
    device_vga_ctl_port_base[0] = (device_vga_screen_width() << 16) | device_vga_screen_height();
    device_add_mmio_map("vgactl", DEVICE_VGA_CTRL_BASE, device_vga_ctl_port_base, 8, NULL);
    device_vga_vmem = device_map_new_space(device_vga_screen_size());
    device_add_mmio_map("vmem", DEVICE_VGA_FB_ADDR, device_vga_vmem, device_vga_screen_size(), NULL);
    if(DEVICE_VGA_SHOW_SCREEN){
        device_vga_init_screen();
        memset(device_vga_vmem, 0, device_vga_screen_size());
    }
}

//========== Device-Serial ==========

void device_serial_putchar(uint64_t device_serial_mem_write_data){
    char device_serial_to_print = (char) device_serial_mem_write_data;
    putc(device_serial_to_print, stderr);
    return;
}

//========== Host timer ==========

uint64_t host_timer_get_time_internal(){
    struct timeval host_timer_now;
    gettimeofday(&host_timer_now, NULL);
    uint64_t host_timer_us = host_timer_now.tv_sec * 1000000 + host_timer_now.tv_usec;
    return host_timer_us;
}

uint64_t host_timer_get_time(){
    if(host_timer_boot_time == 0) {host_timer_boot_time = host_timer_get_time_internal();}
    uint64_t now = host_timer_get_time_internal();
    return now - host_timer_boot_time;
}

//========== Device-Timer (RTC) ==========

uint32_t device_timer_write_time_to_sim(bool low_high){
    uint64_t time_us = host_timer_get_time();
    //printf("time_us = 0x%lx\n", time_us);
    //uint32_t ret;
    if(low_high == 0){return (uint32_t)time_us;}
    if(low_high == 1){return time_us >> 32;    }

    // should not reach here!
    assert(0);
    return -1;
}

//========== Differencial Testing ==========

void diff_difftest_init(long img_size){
    if(difftest_enable){
        if(print_debug_informations) {printf("[difftest] enabled, initializing\n");}
        assert(monitor_diff_so_file != NULL);

        void *handle;
        handle = dlopen(monitor_diff_so_file, RTLD_LAZY);
        assert(handle);

        ref_difftest_memcpy = (void (*)(uint64_t, void*, size_t, bool)) dlsym(handle, "difftest_memcpy");
        assert(ref_difftest_memcpy);

        ref_difftest_regcpy = (void (*)(void*, bool)) dlsym(handle, "difftest_regcpy");
        assert(ref_difftest_regcpy);

        ref_difftest_exec = (void (*)(uint64_t)) dlsym(handle, "difftest_exec");
        assert(ref_difftest_exec);

        ref_difftest_raise_intr = (void (*)(uint64_t)) dlsym(handle, "difftest_raise_intr"); // Not implemented in NEMU
        assert(ref_difftest_raise_intr);

        void (*ref_difftest_init)(int) = (void (*)(int)) dlsym(handle, "difftest_init");
        assert(ref_difftest_init);

        ref_difftest_init(1234);
        ref_difftest_memcpy(mem_start_addr, mem_guest_to_host(mem_start_addr), img_size, DIFFTEST_TO_REF);
        ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF); // Need later changes
    }else{
        if(print_debug_informations) {printf("[difftest] not enabled, skipping\n");}
        return;
    }
    //difftest_memcpy();
}

void diff_difftest_one_exec(){
    if(diff_skip_ref_exec){
        ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF);
        //printf("[difftest] skipped ref\n");
        diff_skip_ref_exec = false;
        return;
    }
    ref_difftest_exec(1);
}

bool diff_difftest_check_reg(){
    if(diff_skip_ref_exec){
        return true;
    }
    riscv64_CPU_State ref;
    ref_difftest_regcpy(&ref, DIFFTEST_TO_DUT);
    for(int i = 0; i < 32; i = i + 1){
        if(cpu.gpr[i] != ref.gpr[i]){
            printf("[difftest] at pc = 0x%lx, gpr x%d different, difftest failed, NSIM's val: 0x%16lx, NEMU's val: 0x%16lx\n", reg_pc, i, cpu.gpr[i], ref.gpr[i]);
            state_set_state(NSIM_ABORT);
            //assert(0);
            return false;
        }
    }
    if(cpu.pc != ref.pc)
    {
        printf("[difftest] pc different, difftest failed, dut = 0x%lx, ref = 0x%lx\n", cpu.pc, ref.pc);
        state_set_state(NSIM_ABORT);
        return false;
    }
    if(print_debug_informations) {printf("[difftest] success at current pc\n");}
    return true;
}

//========== RTL simulation ==========

void sim_sim_init(){
    printf("\33[1;33m[sim] initializing simulation\33[0m\n");
    if(generate_dump_wave_file) {contextp = new VerilatedContext;}
    if(generate_dump_wave_file) {tfp = new VerilatedVcdC;}
    top = new Vnpc;
    if(generate_dump_wave_file) {contextp -> traceEverOn(true);}
    if(generate_dump_wave_file) {top -> trace(tfp, 0);}
    if(generate_dump_wave_file) {tfp -> open("dump.vcd");}
    printf("\33[1;33m[sim] initialize finished\33[0m\n");

    // tell NPC the correct start PC
    top -> clock = 0;
    top -> io_NPC_startPC = mem_start_addr;
    top -> reset = 1;
    //top -> eval();
    sim_step_and_dump_wave();
    top -> clock = 1;
    //top -> eval();
    sim_step_and_dump_wave();
    top -> reset = 0;
    top -> eval();
    //sim_step_and_dump_wave();

    //tfp -> close();
    printf("\33[1;33m[sim] module's start PC is 0x%x\33[0m\n", mem_start_addr);
    return;
}

void sim_sim_exit(){
    printf("\33[1;33m[sim] exit simulation\33[0m\n");
    sim_step_and_dump_wave();
    if(generate_dump_wave_file) {tfp -> close();}
    return;
}

void sim_one_exec(){
    if(nsim_state.state != NSIM_CONTINUE){
        printf("\33[1;33m[sim] current state indicates simulation can not continue\33[0m\n");
        return;
    }

    uint64_t sim_start_time = host_timer_get_time();

    if(print_debug_informations) {printf("\33[1;33m[sim] execution one round\33[0m\n");}
    top -> clock = 0;// simulate posedge

    // Step I: fetch instruction and send back
    if(print_debug_informations) {printf("\33[1;33m[sim] Phase I: Instruction fetch\33[0m\n");}
    uint64_t sim_getCurrentPC = top -> io_NPC_sendCurrentPC;
    if(print_debug_informations) {printf("\33[1;33m[sim] current pc is 0x%lx\33[0m\n", sim_getCurrentPC);}

    trace_pc = sim_getCurrentPC; // Update current pc counter's val so trace can work

    uint32_t sim_currentInst = mem_paddr_read(sim_getCurrentPC, 4);
    top -> io_NPC_getInst = sim_currentInst;
    if(print_debug_informations) {printf("\33[1;33m[sim] current instruction is 0x%x\33[0m\n", sim_currentInst);}

    #ifdef trace_enable_itrace
    trace_itrace_write(sim_currentInst);
    #endif

    top -> eval();

    // Step II: decode instruction
    if(print_debug_informations) {printf("\33[1;33m[sim] Phase II: Instruction decode\33[0m\n");}
    //top -> eval();

    // Step III: EXU execution
    if(print_debug_informations) {printf("\33[1;33m[sim] Phase III: execute\33[0m\n");}
    //top -> eval();

    // Step IV: Load and store
    if(print_debug_informations) {printf("\33[1;33m[sim] Phase IV: load and store\33[0m\n");}
    if(top -> io_NPC_LSU_O_accessMem == 0b1)
    {
        if(top -> io_NPC_LSU_O_memRW == 0b1){
            // memory write
            uint64_t sim_mem_write_addr = top -> io_NPC_LSU_O_memAddr;
            switch(top -> io_NPC_LSU_O_len){
                case 0b00: mem_pmem_write(sim_mem_write_addr, 1, top -> io_NPC_LSU_O_memW);    break;
                case 0b01: mem_pmem_write(sim_mem_write_addr, 2, top -> io_NPC_LSU_O_memW);    break;
                case 0b10: mem_pmem_write(sim_mem_write_addr, 4, top -> io_NPC_LSU_O_memW);    break;
                case 0b11: mem_pmem_write(sim_mem_write_addr, 8, top -> io_NPC_LSU_O_memW);    break;
                default:   printf("[sim] NPC returned an unknown memory length\n"); assert(0); break;
            }
            top -> io_NPC_LSU_I_memR = 0;
        }else{
            // memory read
            u_int64_t sim_mem_read_addr = top -> io_NPC_LSU_O_memAddr;
            switch(top -> io_NPC_LSU_O_len){
                case 0b00: top -> io_NPC_LSU_I_memR = mem_pmem_read(sim_mem_read_addr, 1);     break;
                case 0b01: top -> io_NPC_LSU_I_memR = mem_pmem_read(sim_mem_read_addr, 2);     break;
                case 0b10: top -> io_NPC_LSU_I_memR = mem_pmem_read(sim_mem_read_addr, 4);     break;
                case 0b11: top -> io_NPC_LSU_I_memR = mem_pmem_read(sim_mem_read_addr, 8);     break;
                default:   printf("[sim] NPC returned an unknown memory length\n"); assert(0); break;
            }
        }
    }
    else{
        top -> io_NPC_LSU_I_memR = 0;
    }
    //top -> eval();

    // Step V: Write back
    if(print_debug_informations) {printf("\33[1;33m[sim] Phase V: write back\33[0m\n");}
    //top -> eval();

    sim_step_and_dump_wave();

    top -> clock = 1; //simulate posedge

    top -> eval();

    // We will update devices after the posedge
    device_update();

    reg_get_reg_from_sim();
    //reg_get_pcreg_from_sim();
    /*for(int i = 0; i < 32; i = i + 1){
        cpu.gpr[i] = nsim_gpr[i].value;
    }*/
    //cpu.pc = top -> io_NPC_sendCurrentPC;
    reg_display(false);

    #ifdef trace_enable_rtrace
        trace_rtrace_context context_in_sim;
        for(int i = 0; i < 32; i = i + 1){
            context_in_sim.gpr[i] = nsim_gpr[i].value;
        }
        context_in_sim.pc = top -> io_NPC_sendCurrentPC;
        trace_rtrace_write(context_in_sim);
    #endif

    //nsim_state.state = NSIM_CONTINUE;
    nsim_state.halt_pc = reg_pc;

    if(difftest_enable)
    {
        diff_difftest_one_exec();
        if(!diff_difftest_check_reg())
        {
            state_set_state(NSIM_ABORT);
        }
    }

    if(top -> io_NPC_halt == 0b1){
        if(nsim_gpr[10].value == 0){
            printf("\33[1;33m[sim] \33[1;32mHIT GOOD TRAP\33[1;33m at pc 0x%lx\33[0m\n", top -> io_NPC_sendCurrentPC - 4);
        }else{
            printf("\33[1;33m[sim] \33[1;31mHIT BAD  TRAP\33[1;33m at pc 0x%lx\33[0m\n", top -> io_NPC_sendCurrentPC - 4);
        }
        sim_step_and_dump_wave();
        state_set_state(NSIM_END);
        return;
    }

    /*if(top -> io_NPC_error == 0b1){
        // NPC raised error, stop simulation
        state_set_state(NSIM_ABORT);
        //nsim_state.state = NSIM_ABORT;
    }*/

    sim_step_and_dump_wave();

    uint64_t sim_finish_time = host_timer_get_time();

    statistics_time_exec += (sim_finish_time - sim_start_time);
    statistics_nr_exec = statistics_nr_exec + 1;

    return;
}

void sim_step_and_dump_wave(){
    top -> eval();
    if(generate_dump_wave_file) {contextp -> timeInc(1);}
    if(generate_dump_wave_file) {tfp -> dump(contextp -> time());}
    return;
}

//========== Register manipulations ==========

void reg_get_reg_from_sim(){
    //printf("[reg] getting GPR No.%d from simulation environment\n", reg_idx);
    cpu.gpr[0]  = top -> io_NPC_GPR00;
    cpu.gpr[1]  = top -> io_NPC_GPR01;
    cpu.gpr[2]  = top -> io_NPC_GPR02;
    cpu.gpr[3]  = top -> io_NPC_GPR03;
    cpu.gpr[4]  = top -> io_NPC_GPR04;
    cpu.gpr[5]  = top -> io_NPC_GPR05;
    cpu.gpr[6]  = top -> io_NPC_GPR06;
    cpu.gpr[7]  = top -> io_NPC_GPR07;
    cpu.gpr[8]  = top -> io_NPC_GPR08;
    cpu.gpr[9]  = top -> io_NPC_GPR09;
    cpu.gpr[10] = top -> io_NPC_GPR10;
    cpu.gpr[11] = top -> io_NPC_GPR11;
    cpu.gpr[12] = top -> io_NPC_GPR12;
    cpu.gpr[13] = top -> io_NPC_GPR13;
    cpu.gpr[14] = top -> io_NPC_GPR14;
    cpu.gpr[15] = top -> io_NPC_GPR15;
    cpu.gpr[16] = top -> io_NPC_GPR16;
    cpu.gpr[17] = top -> io_NPC_GPR17;
    cpu.gpr[18] = top -> io_NPC_GPR18;
    cpu.gpr[19] = top -> io_NPC_GPR19;
    cpu.gpr[20] = top -> io_NPC_GPR20;
    cpu.gpr[21] = top -> io_NPC_GPR21;
    cpu.gpr[22] = top -> io_NPC_GPR22;
    cpu.gpr[23] = top -> io_NPC_GPR23;
    cpu.gpr[24] = top -> io_NPC_GPR24;
    cpu.gpr[25] = top -> io_NPC_GPR25;
    cpu.gpr[26] = top -> io_NPC_GPR26;
    cpu.gpr[27] = top -> io_NPC_GPR27;
    cpu.gpr[28] = top -> io_NPC_GPR28;
    cpu.gpr[29] = top -> io_NPC_GPR29;
    cpu.gpr[30] = top -> io_NPC_GPR30;
    cpu.gpr[31] = top -> io_NPC_GPR31;

    cpu.pc = top -> io_NPC_sendCurrentPC;
    return;
}

void reg_get_pcreg_from_sim(){
    if(print_debug_informations) {printf("\33[1;34m[reg] getting PC registers from simulation environment\33[0m\n");}
    //cpu.pc = top -> io_NPC_sendCurrentPC - 4;
    //reg_snpc = top -> io_NPC_sendCurrentPC + 4;
    //reg_dnpc = top -> io_NPC_sendNextPC;
    return;
}
void reg_display(bool sdb_print_regs){
    if(print_debug_informations || sdb_print_regs){
        printf("\33[1;34m[reg] print registers\n");
        printf("pc = 0x%lx, snpc = 0x%lx, dnpc = 0x%lx\n", reg_pc, reg_snpc, reg_dnpc);
        for(int i = 0; i < 32; i = i + 1){
            printf("x%2d = 0x%16lx\t", i, cpu.gpr[i]);
            if((i + 1) % 4 == 0) {printf("\n");}
        }
        printf("\33[0m");
    }
    return;
}

//========== NSIM states ==========

bool state_check_can_continue(){
    switch(nsim_state.state){
        case NSIM_CONTINUE: /*printf("[state] state is NSIM_CONTINUE, can continue\n");*/ return true;  break;
        case NSIM_STOP:     /*printf("[state] state is NSIM_STOP, can continue\n");*/     return true;  break;
        case NSIM_ABORT:    /*printf("[state] state is NSIM_ABORT, can't continue\n");*/  return false; break;
        case NSIM_END:      /*printf("[state] state is NSIM_END, can't continue\n");*/    return false; break;
        case NSIM_HALT:     /*printf("[state] state is NSIM_HALT, can't continue\n");*/   return false; break;
        case NSIM_QUIT:     /*printf("[state] state is NSIM_QUIT, can't continue\n");*/   return false; break;
        default:            printf("[state] unknown state, error\n"); assert(0);      return false; break;
    }
    return false;
}

void state_get_state(){
    state_check_can_continue();
}

void state_set_state(int state_get_state){
    bool state_continue               = state_get_state == NSIM_CONTINUE || state_get_state == NSIM_STOP;
    bool state_normal_cant_continue   = state_get_state == NSIM_END      || state_get_state == NSIM_QUIT;
    bool state_abnormal_cant_continue = state_get_state == NSIM_ABORT    || state_get_state == NSIM_HALT;
    assert(state_continue || (state_normal_cant_continue || state_abnormal_cant_continue));
    //if(state_continue)                {printf("[state] state update to a can continue state\n");}
    //if(state_normal_cant_continue)    {printf("[state] state update to a normal can't continue state\n");}
    //if(state_abnormal_cant_continue)  {printf("[state] state update to an abnormal can't continue state\n");}
    nsim_state.state = state_get_state;
}

//========== Memory manipulations ==========

static uint8_t *mem_pmem = NULL;

// static uint8_t mem_pmem[mem_size] __attribute((aligned(4096))) = {};

static inline uint64_t mem_host_read(uint8_t *mem_addr, int mem_length){
    //printf("[memory] in host read, address is %p, length is %d\n", mem_addr, mem_length);
    switch (mem_length){
        case 1: return *(uint8_t  *)mem_addr;
        case 2: return *(uint16_t *)mem_addr;
        case 4: return *(uint32_t *)mem_addr;
        case 8: return *(uint64_t *)mem_addr;
        default: printf("[memory] invalid read length\n"); assert(0); return 0;
    }
}

static void mem_host_write(void *mem_addr, int mem_length, uint64_t mem_data){
    switch (mem_length){
        case 1: *(uint8_t  *)mem_addr = mem_data; return;
        case 2: *(uint16_t *)mem_addr = mem_data; return;
        case 4: *(uint32_t *)mem_addr = mem_data; return;
        case 8: *(uint64_t *)mem_addr = mem_data; return;
        default: printf("[memory] invalid write length\n"); assert(0); return;
    }
}

bool mem_addr_in_bound(uint32_t mem_addr){
    // For a 64-bit system, the mem address space should be uint64_t, however, if we use uint64_t here, when we scan memory, there will be segmentation error, so we set to uint32_t first
    // but need later refinements!
    if(mem_addr - mem_start_addr > mem_size || mem_addr < mem_start_addr)
        {/*printf("[memory] address 0x%x out of bound [0x%x,0x%x]\n", mem_addr, mem_start_addr, mem_end_addr);*/ /*assert(0);*/ return false; }
    return true;
}

void mem_init_mem(){
    mem_pmem = (uint8_t *)malloc(mem_size);
    //printf("[memory] host memory starts at %p\n", mem_pmem);
    assert(mem_pmem);
    printf("[memory] physical memory area [0x%x,0x%x] size 0x%x\n", mem_start_addr, mem_end_addr, mem_size);
}

uint8_t* mem_guest_to_host(uint64_t paddr) { uint64_t temp = paddr - mem_start_addr; /*printf("[memory] temp is 0x%0x mem_pmem is %p, guest to host addr %p, paddr 0x%x, memstart addr 0x%x\n", temp, mem_pmem, mem_pmem + paddr, paddr, mem_start_addr);*/ return mem_pmem + (paddr - mem_start_addr); }
uint64_t mem_host_to_guest(uint8_t *haddr) { return haddr - mem_pmem + mem_start_addr; }

uint64_t mem_pmem_read(uint64_t mem_addr, int mem_length){
    //printf("[memory] mem_pmem_read: mem_guest_to_host(mem_addr) = %p\n", mem_guest_to_host(mem_addr));

    //printf("addr = 0x%lx\n", mem_addr);

    //if(device_have_rtc && mem_addr == DEVICE_RTC_ADDR_LO){/*printf("[device] access rtc_lo\n");*/ return device_timer_write_time_to_sim(0);}
    //if(device_have_rtc && mem_addr == DEVICE_RTC_ADDR_HI){/*printf("[device] access rtc_hi\n");*/ return device_timer_write_time_to_sim(1);}

    if(mem_addr_in_bound(mem_addr)){
        uint64_t ret = mem_host_read(mem_guest_to_host(mem_addr), mem_length);

        #ifdef trace_enable_mtrace
        trace_mtrace_write(mem_addr, false, ret, mem_length);
        #endif

        return ret;
    }else{
        // Address is not Physical Memory, implement device
        //printf("addr = 0x%lx\n", mem_addr);
        uint64_t ret = device_mmio_read(mem_addr, mem_length);

        #ifdef trace_enable_dtrace 
        trace_dtrace_write((char *)device_mmio_fetch_mmio_map(mem_addr) -> name, mem_addr, mem_length, false, ret);
        #endif

        return ret;
    }
}
void mem_pmem_write(uint64_t mem_addr, int mem_length, uint64_t mem_data){

    //if(device_have_serial && mem_addr == DEVICE_SERIAL_ADDR){device_serial_putchar(mem_data); return;}

    if(mem_addr_in_bound(mem_addr)){
        //printf("[memory] normal write, addr = 0x%lx, len = %d, data = 0x%lx\n", mem_addr, mem_length, mem_data);
        mem_host_write(mem_guest_to_host(mem_addr), mem_length, mem_data);

        #ifdef trace_enable_mtrace 
        trace_mtrace_write(mem_addr, true, mem_data, mem_length);
        #endif

        return;
    }else{
        //printf("[memory] device write, addr = 0x%lx, len = %d, data = 0x%lx\n", mem_addr, mem_length, mem_data);
        device_mmio_write(mem_addr, mem_length, mem_data);

        #ifdef trace_enable_dtrace 
        trace_dtrace_write((char *)device_mmio_fetch_mmio_map(mem_addr) -> name, mem_addr, mem_length, false, mem_data);
        #endif

        return;
    }

    // should not reach here!
    printf("[mem-pmem-write] error: address not in physical memory bound and failed with device mem write\n");
    assert(0);
    return;
    
}

uint64_t mem_paddr_read(uint64_t mem_addr, int mem_length){
    //printf("[memory] mem_paddr_read: mem_addr = 0x%x\n", mem_addr);
    if(mem_addr_in_bound(mem_addr))
        return mem_pmem_read(mem_addr, mem_length);
    // should not reach here!
    assert(0);
    return 0;
}

void mem_paddr_write(uint64_t mem_addr, int mem_length, uint64_t mem_data){
    if(mem_addr_in_bound(mem_addr))
        mem_pmem_write(mem_addr, mem_length, mem_data);
    // should not reach here!
    assert(0);
    return;
}

uint64_t mem_vaddr_ifetch(uint64_t mem_addr, int mem_length){
    return mem_paddr_read(mem_addr, mem_length);
}

uint64_t mem_vaddr_read(uint64_t mem_addr, int mem_length){
    return mem_paddr_read(mem_addr, mem_length);
}

void mem_vaddr_write(uint64_t mem_addr, int mem_length, uint64_t mem_data){
    mem_paddr_write(mem_addr, mem_length, mem_data);
}

//========== Simple Debugger (SDB) Monitor ==========

long monitor_load_img(){
    if(monitor_img_file == NULL){
        printf("[monitor] no image file is given, using built-in RISCV image\n");
        memcpy(mem_guest_to_host(mem_start_addr), img, sizeof(img));
        monitor_img_size = sizeof(img);
        return -1;
    }

    FILE *fp = fopen(monitor_img_file, "rb");
    assert(fp);

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    monitor_img_size = size;

    printf("[monitor] image is %s size is %ld\n", monitor_img_file, size);

    fseek(fp, 0, SEEK_SET);
    int ret = fread(mem_guest_to_host(mem_start_addr), size, 1, fp);
    assert(ret == 1);

    fclose(fp);
    return 0;
}

int monitor_parse_args(int argc, char*argv[]){
    const struct option table[] = {
        {"batch"    , no_argument      , NULL, 'b'},
        {"log"      , required_argument, NULL, 'l'},
        {"diff"     , required_argument, NULL, 'd'},
        {"port"     , required_argument, NULL, 'p'},
        {"help"     , no_argument      , NULL, 'h'},
        {"readbin"  , required_argument, NULL, 'i'},
        {"readelf"  , required_argument, NULL, 'r'},
        {"readdiasm", required_argument, NULL, 'a'},
        {"script"   , no_argument      , NULL, 's'},
        {0          , 0                , NULL,  0 },
    };
    int o;
    while ( (o = getopt_long(argc, argv, "-sbhl:d:p:r:a:", table, NULL)) != -1) {
        switch (o) {
            //case 'b': sdb_set_batch_mode(); break;
            case 'p': sscanf(optarg, "%d", &monitor_difftest_port); break;
            case 'l': monitor_log_file = optarg; printf("log_file = \"%s\"\n", monitor_log_file); break;
            case 'd': monitor_diff_so_file = optarg; printf("diff_so_file = \"%s\"\n", monitor_diff_so_file); break;
            case 'r': monitor_elf_file = optarg; printf("elf_file = \"%s\"\n", monitor_elf_file); break;
            case 'a': monitor_das_file = optarg; printf("das_file = \"%s\"\n", monitor_das_file); break;
            case 'i': monitor_img_file = optarg; printf("bin_file = \"%s\"\n", monitor_img_file); break;
            case 's': monitor_script_mode = true; printf("using script mode, automatically execute\n"); break;
            case 1:   monitor_img_file = optarg; printf("img_file = \"%s\"\n", monitor_img_file); return 0;
            default:
                printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
                printf("\t-b,--batch              run with batch mode\n");
                printf("\t-l,--log=FILE           output log to FILE\n");
                printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
                printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
                printf("\n");
                exit(0);
        }
    }
    return 0;
}

void monitor_init_monitor(int argc, char*argv[]){
    monitor_parse_args(argc, argv);
    monitor_load_img();
    return;
}

//========== Simple Debugger (SDB) User Interface ==========

char* sdb_rl_gets(){
    char* sdb_line_read = NULL;

    if(sdb_line_read){
        free(sdb_line_read);
        sdb_line_read = NULL;
    }

    sdb_line_read = readline("(nsim) ");

    if (sdb_line_read && *sdb_line_read){
        add_history(sdb_line_read);
    }

    return sdb_line_read;
}

int sdb_cmd_c(char* args){
    //printf("[sdb] continue NPC execution\n");
    if(nsim_state.state == NSIM_STOP){state_set_state(NSIM_CONTINUE);}
    while((nsim_state.state == NSIM_CONTINUE)){
        sim_one_exec();
    }
    statistics_show();
    return 0;
} // continue execution

int sdb_cmd_s(char* args){
    if (args == NULL){
        //printf("[sdb] 1 setp NPC execution\n");
        sim_one_exec();
        //cpu_exec(1);
    }
    else{
        int sdb_cmd_si_n = atoi(args);
        if(sdb_cmd_si_n < -1){
        printf("Invalid input\n");
        return 0;
    }
    //printf("[sdb] %d setp NPC execution\n", sdb_cmd_si_n);
    for(int i = 0; i < sdb_cmd_si_n; i = i + 1){
        sim_one_exec();
    }
    //cpu_exec(cmd_si_n);
    }
    return 0;
} // single-step execution

int sdb_cmd_i(char* args){
    if (args == NULL){
        printf("No Subcommand\n");
        return 0;
    }
    else{
        if (strcmp(args, "r") == 0){
        //printf("[sdb] list registers\n");
        reg_display(true);
        //isa_gpr_display();
    }
    else if (strcmp(args, "w") == 0){
        //printf("[sdb] list watchpoints\n");
        //print_WP();
    }
    else{
        printf("Subcommand Not Defined\n");
    }
  }
    return 0;
} // informations (register and watchpoint)

int sdb_cmd_x(char* args){
    //printf("[sdb] scan and print memory\n");
    int print_length;
    uint64_t start_memory_address;
    char *last_part_of_args;
    char *string_token_first = strtok_r(args, " ", &last_part_of_args);
    print_length = atoi(string_token_first);
    sscanf(last_part_of_args, "%lx", &start_memory_address);
    //printf("start mem addr is 0x%lx\n", start_memory_address);
    printf("******************************************************************************\n");
    printf("|  Address   | 4b Phys (Hex) | 4b Virt (Hex) | 4b Phys (Dec) | 4b Virt (Dec) |\n");
    for (uint64_t i = start_memory_address; i < start_memory_address + print_length; i = i + 4){
        //printf("[sdb] reading memory at address 0x%x\n", i);
        printf("| 0x%lx | 0x   %8lx | 0x   %8lx | 0x %10ld | 0x %10ld |\n", i, mem_paddr_read(i, 4), mem_vaddr_read(i, 4), mem_paddr_read(i, 4), mem_vaddr_read(i, 4));
    }
    printf("******************************************************************************\n\n");
    return 0;
} // scan and print memory

int sdb_cmd_p(char* args){
    //printf("[sdb] expression evaluation\n");
    bool expression_success;
    expression_success = false;
    u_int64_t cmd_p_result = 0;
    //cmd_p_result = expr(args, &expression_success);
    printf("%ld\t", cmd_p_result);
    printf("0x%lx\n", cmd_p_result);
    return 0;
} // expression evaluation

int sdb_cmd_w(char* args){
    //printf("[sdb] add watchpoint\n");
    return 0;
} // add watchpoint

int sdb_cmd_d(char* args){
    //printf("[sdb] delete watchpoint\n");
    return 0;
} // delete watchpoint

int sdb_cmd_q(char* args){
    //printf("[sdb] quit NSIM\n");
    return -1;
} // quit NSIM

int sdb_cmd_h(char* args){
    for(int i = 0; i < SDB_NR_CMD; i = i + 1){
        printf("%s - %s\n", sdb_cmd_table[i].name, sdb_cmd_table[i].description);
    }
    return 0;
} // help

/*void sdb_set_batch_mode(){
    sdb_is_batch_mode = true;
    return;
}*/

void sdb_main_loop(){
    /*if(sdb_is_batch_mode){
        sdb_cmd_c(NULL);
        return;
    }*/

    if(monitor_script_mode){
        sdb_cmd_c(NULL);
        return;
    }

    for(char* str; (str = sdb_rl_gets()) != NULL; ){
        char* str_end = str + strlen(str);
        char* cmd = strtok(str, " ");
        if (cmd == NULL) {continue;}
        
        char* args = cmd + strlen(cmd) + 1;
        if(args >= str_end) {args = NULL;}

        device_sdl_clear_event_queue();

        int i;
        for(i = 0; i < SDB_NR_CMD; i = i + 1){
            if(strcmp(cmd, sdb_cmd_table[i].name) == 0){
                if(sdb_cmd_table[i].handler(args) < 0) {return;}
                break;
            }
        }

        if(i == SDB_NR_CMD) {printf("[sdb] unknown command '%s'\n", cmd);}
    }
    return;
}

void sdb_init_sdb(){
    // No expr and wp currently, so no function implementations
    printf("[sdb] initialized\n");
    return;
}

//========== Main function ==========
int main(int argc, char *argv[]){
    mem_init_mem();
    device_init_devices();
    trace_init_trace();
    monitor_init_monitor(argc, argv);
    //memcpy(mem_guest_to_host(mem_start_addr), img, sizeof(img));
    state_set_state(NSIM_CONTINUE);
    diff_difftest_init(monitor_img_size);
    printf("Welcome to riscv64-nsim\n");
    sim_sim_init();
    sdb_main_loop();
    sim_sim_exit();
    return 0;
}
