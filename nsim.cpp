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
    Linux: "verilator -Wno-lint -Wno-style --cc --exe --build --trace nsim.cpp npc.v -LDFLAGS -lreadline -lSDL2"
*/

//========== Macro Configurations ==========
#define mem_start_addr  0x080000000
#define mem_end_addr    0x08fffffff
#define mem_size        mem_end_addr - mem_start_addr + 1

#define print_debug_informations false
#define generate_dump_wave_file  false

#define difftest_enable false

#define device_have_serial   true
#define device_have_rtc      true
#define device_have_keyboard false
#define device_have_vga      false
#define device_have_audio    false
#define device_have_disk     false
#define device_have_sdcard   false

//========== Include Headers ==========

#include "verilated.h"
#include "verilated_vcd_c.h"
#include "obj_dir/Vnpc.h"

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

//---------- Simple Debugger (SDB) Monitor ----------

char* monitor_log_file = NULL;
char* monitor_diff_so_file = NULL;
char* monitor_img_file = NULL;
char* monitor_elf_file = NULL;
char* monitor_das_file = NULL;
int   monitor_difftest_port = 1234;
long  monitor_img_size = -1;

long monitor_load_img();
int monitor_parse_args(int argc, char*argv[]);
void monitor_init_monitor(int argc, char*argv[]);

//---------- Simple Debugger (SDB) User Interface ----------

int sdb_is_batch_mode = false;

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

void sdb_set_batch_mode();
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

void reg_get_reg_from_sim(int reg_idx);
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
void device_init_vga()
void device_init_i8042();
void device_init_audio();
void device_init_disk();
void device_init_sdcard();
void device_init_alarm();
void device_send_key(uint8_t, bool);
void device_vga_update_screen();
void device_update();
void device_init_devices();

//typedef uint16_t ioaddr_t;

//---------- Device: Map & MMIO----------

#define DEVICE_MAP_IO_SPACE_MAX (2 * 1024 * 1024)
#define DEVICE_NR_MAP 16

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

uint64_t device_map_read(uint64_t addr, int len, IOMap *map);
void device_map_write(uint64_t addr, int len, uint64_t data, IOMap *map);

uint8_t* device_map_new_space(int size);
static void device_check_bound(IOMap *map, uint64_t addr);
static void device_invoke_callback(io_callback_t c, uint64_t offset, int len, bool is_write);

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
            //difftest_skip_ref(); //As we use NEMU to difftest, the behavior of decices are same with NSIM
            return i;
        }
    }
    // should not reach here!
    return -1;
}

static IOMap* device_mmio_fetch_mmio_map(uint64_t addr){
    //printf("[fetch_mmio_map] paddr is 0x%8x\n", addr);
    int mapid = device_find_mapID_by_addr(device_maps, device_nr_map, addr);
    return (mapid == -1 ? NULL : &device_maps[mapid]);
}

static void device_report_mmio_overlap(const char* name1, uint64_t left1, uint64_t right1, const char* name2, uint64_t left2, uint64_t right2){
    printf("[device] error: MMIO region \"%s\"@[0x%lx,0x%lx] is overlapped with \"%s\"@[0x%lx,0x%lx]\n", name1, left1, right1, name2, left2, right2);
    // should not reach here!
    assert(0);
    return;
}

void device_add_mmio_map(const char *name, uint64_t addr, void *space, uint32_t len, io_callback_t callback){
    assert(device_nr_map < DEVICE_NR_MAP); // so that we have space to add another device
    uint64_t left = addr;
    uint64_t right = addr + len - 1;
    printf("[device] name \"%s\", left 0x%lx, right 0x%lx check before add to mmio map\n", name, left, right);
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
    printf("[device] name \"%s\", left 0x%lx, right 0x%lx check passed, adding to mmio map\n", name, left, right);
    device_maps[device_nr_map] = (IOMap){ .name = name, .low = addr, .high = addr + len - 1, .space = space, .callback = callback};
    printf("[device] name \"%s\", left 0x%lx, right 0x%lx, added to mmio map\n", name, left, right);
    return;
}

//========== Devices ==========

void device_init_map(){
    // TODO
    assert(0);
    return;
}

//---------- Device-Serial ----------

#define DEVICE_SERIAL_ADDR 0xa00003f8
void device_serial_putchar(uint64_t device_serial_mem_write_data);

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
    ref_difftest_exec(1);
}

bool diff_difftest_check_reg(){
    riscv64_CPU_State ref;
    ref_difftest_regcpy(&ref, DIFFTEST_TO_DUT);
    for(int i = 0; i < 32; i = i + 1){
        if(cpu.gpr[i] != ref.gpr[i]){
            printf("[difftest] gpr x%d different, difftest failed, NSIM's val: 0x%16lx, NEMU's val: 0x%16lx\n", i, cpu.gpr[i], ref.gpr[i]);
            state_set_state(NSIM_ABORT);
            //assert(0);
            return false;
        }
    }
    if(cpu.pc != ref.pc)
    {
        printf("[difftest] pc different, difftest failed\n");
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
    top -> eval();
    sim_step_and_dump_wave();
    top -> clock = 1;
    top -> eval();
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
    if(!(nsim_state.state == NSIM_CONTINUE || nsim_state.state == NSIM_STOP)){
        printf("\33[1;33m[sim] current state indicates simulation can not continue\33[0m\n");
        return;
    }

    if(print_debug_informations) {printf("\33[1;33m[sim] execution one round\33[0m\n");}
    top -> clock = 0;// simulate posedge

    // Step I: fetch instruction and send back
    if(print_debug_informations) {printf("\33[1;33m[sim] Phase I: Instruction fetch\33[0m\n");}
    uint64_t sim_getCurrentPC = top -> io_NPC_sendCurrentPC;
    if(print_debug_informations) {printf("\33[1;33m[sim] current pc is 0x%lx\33[0m\n", sim_getCurrentPC);}
    uint32_t sim_currentInst = mem_paddr_read(sim_getCurrentPC, 4);
    top -> io_NPC_getInst = sim_currentInst;
    if(print_debug_informations) {printf("\33[1;33m[sim] current instruction is 0x%x\33[0m\n", sim_currentInst);}
    top -> eval();

    // Step II: decode instruction
    if(print_debug_informations) {printf("\33[1;33m[sim] Phase II: Instruction decode\33[0m\n");}
    top -> eval();

    // Step III: EXU execution
    if(print_debug_informations) {printf("\33[1;33m[sim] Phase III: execute\33[0m\n");}
    top -> eval();

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
    top -> eval();

    // Step V: Write back
    if(print_debug_informations) {printf("\33[1;33m[sim] Phase V: write back\33[0m\n");}
    top -> eval();

    sim_step_and_dump_wave();

    top -> clock = 1; //simulate posedge

    top -> eval();

    for(int i = 0; i < 32; i = i + 1){
        reg_get_reg_from_sim(i);
    }
    reg_get_pcreg_from_sim();
    for(int i = 0; i < 32; i = i + 1){
        cpu.gpr[i] = nsim_gpr[i].value;
    }
    cpu.pc = top -> io_NPC_sendCurrentPC;
    reg_display(false);

    nsim_state.state = NSIM_CONTINUE;
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
            printf("\33[1;33m[sim] \33[1;32mHIT GOOD TRAP\33[1;33m at pc 0x%lx\33[0m\n", top -> io_NPC_sendCurrentPC);
        }else{
            printf("\33[1;33m[sim] \33[1;31mHIT BAD  TRAP\33[1;33m at pc 0x%lx\33[0m\n", top -> io_NPC_sendCurrentPC);
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

    return;
}

void sim_step_and_dump_wave(){
    top -> eval();
    if(generate_dump_wave_file) {contextp -> timeInc(1);}
    if(generate_dump_wave_file) {tfp -> dump(contextp -> time());}
    return;
}

//========== Register manipulations ==========

void reg_get_reg_from_sim(int reg_idx){
    //printf("[reg] getting GPR No.%d from simulation environment\n", reg_idx);
    switch(reg_idx){
        case 0:   nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR00; break;
        case 1:   nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR01; break;
        case 2:   nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR02; break;
        case 3:   nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR03; break;
        case 4:   nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR04; break;
        case 5:   nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR05; break;
        case 6:   nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR06; break;
        case 7:   nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR07; break;
        case 8:   nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR08; break;
        case 9:   nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR09; break;
        case 10:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR10; break;
        case 11:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR11; break;
        case 12:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR12; break;
        case 13:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR13; break;
        case 14:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR14; break;
        case 15:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR15; break;
        case 16:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR16; break;
        case 17:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR17; break;
        case 18:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR18; break;
        case 19:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR19; break;
        case 20:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR20; break;
        case 21:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR21; break;
        case 22:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR22; break;
        case 23:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR23; break;
        case 24:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR24; break;
        case 25:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR25; break;
        case 26:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR26; break;
        case 27:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR27; break;
        case 28:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR28; break;
        case 29:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR29; break;
        case 30:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR30; break;
        case 31:  nsim_gpr[reg_idx].index = reg_idx; nsim_gpr[reg_idx].value = top -> io_NPC_GPR31; break;
        default:  printf("\33[1;34m[reg] unknown register index\33[0m\n"); assert(0);               break;
    }
    return;
}
void reg_get_pcreg_from_sim(){
    if(print_debug_informations) {printf("\33[1;34m[reg] getting PC registers from simulation environment\33[0m\n");}
    reg_pc = top -> io_NPC_sendCurrentPC;
    reg_snpc = top -> io_NPC_sendCurrentPC + 4;
    reg_dnpc = top -> io_NPC_sendNextPC;
    return;
}
void reg_display(bool sdb_print_regs){
    if(print_debug_informations || sdb_print_regs){
        printf("\33[1;34m[reg] print registers\n");
        printf("pc = 0x%lx, snpc = 0x%lx, dnpc = 0x%lx\n", reg_pc, reg_snpc, reg_dnpc);
        for(int i = 0; i < 32; i = i + 1){
            printf("x%2d = 0x%16lx\t", i, nsim_gpr[i].value);
            if((i + 1) % 4 == 0) {printf("\n");}
        }
        printf("\33[0m");
    }
    return;
}

//========== NSIM states ==========

bool state_check_can_continue(){
    switch(nsim_state.state){
        case NSIM_CONTINUE: printf("[state] state is NSIM_CONTINUE, can continue\n"); return true;  break;
        case NSIM_STOP:     printf("[state] state is NSIM_STOP, can continue\n");     return true;  break;
        case NSIM_ABORT:    printf("[state] state is NSIM_ABORT, can't continue\n");  return false; break;
        case NSIM_END:      printf("[state] state is NSIM_END, can't continue\n");    return false; break;
        case NSIM_HALT:     printf("[state] state is NSIM_HALT, can't continue\n");   return false; break;
        case NSIM_QUIT:     printf("[state] state is NSIM_QUIT, can't continue\n");   return false; break;
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
    if(state_continue)                {printf("[state] state update to a can continue state\n");}
    if(state_normal_cant_continue)    {printf("[state] state update to a normal can't continue state\n");}
    if(state_abnormal_cant_continue)  {printf("[state] state update to an abnormal can't continue state\n");}
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
    if(mem_addr - mem_start_addr > mem_size || mem_addr < mem_start_addr)
        {printf("[memory] address 0x%lx out of bound [0x%x,0x%x]\n", mem_addr, mem_start_addr, mem_end_addr); assert(0); return false; }
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

    if(device_have_rtc && mem_addr == DEVICE_RTC_ADDR_LO){/*printf("[device] access rtc_lo\n");*/ return device_timer_write_time_to_sim(0);}
    if(device_have_rtc && mem_addr == DEVICE_RTC_ADDR_HI){/*printf("[device] access rtc_hi\n");*/ return device_timer_write_time_to_sim(1);}

    uint64_t ret = mem_host_read(mem_guest_to_host((uint32_t)mem_addr), mem_length);
    return ret;
}
void mem_pmem_write(uint64_t mem_addr, int mem_length, uint64_t mem_data){

    if(device_have_serial && mem_addr == DEVICE_SERIAL_ADDR){device_serial_putchar(mem_data); return;}

    mem_host_write(mem_guest_to_host((uint32_t)mem_addr), mem_length, mem_data);
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
        {0          , 0                , NULL,  0 },
    };
    int o;
    while ( (o = getopt_long(argc, argv, "-bhl:d:p:r:a:", table, NULL)) != -1) {
        switch (o) {
            case 'b': sdb_set_batch_mode(); break;
            case 'p': sscanf(optarg, "%d", &monitor_difftest_port); break;
            case 'l': monitor_log_file = optarg; printf("log_file = \"%s\"\n", monitor_log_file); break;
            case 'd': monitor_diff_so_file = optarg; printf("diff_so_file = \"%s\"\n", monitor_diff_so_file); break;
            case 'r': monitor_elf_file = optarg; printf("elf_file = \"%s\"\n", monitor_elf_file); break;
            case 'a': monitor_das_file = optarg; printf("das_file = \"%s\"\n", monitor_das_file); break;
            case 'i': monitor_img_file = optarg; printf("bin_file = \"%s\"\n", monitor_img_file); break;
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
    printf("[sdb] continue NPC execution\n");
    while((nsim_state.state == NSIM_CONTINUE || nsim_state.state == NSIM_STOP)){
        sim_one_exec();
    }
    return 0;
} // continue execution

int sdb_cmd_s(char* args){
    if (args == NULL){
        printf("[sdb] 1 setp NPC execution\n");
        sim_one_exec();
        //cpu_exec(1);
    }
    else{
        int sdb_cmd_si_n = atoi(args);
        if(sdb_cmd_si_n < -1){
        printf("Invalid input\n");
        return 0;
    }
    printf("[sdb] %d setp NPC execution\n", sdb_cmd_si_n);
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
        printf("[sdb] list registers\n");
        reg_display(true);
        //isa_gpr_display();
    }
    else if (strcmp(args, "w") == 0){
        printf("[sdb] list watchpoints\n");
        //print_WP();
    }
    else{
        printf("Subcommand Not Defined\n");
    }
  }
    return 0;
} // informations (register and watchpoint)

int sdb_cmd_x(char* args){
    printf("[sdb] scan and print memory\n");
    int print_length;
    int start_memory_address;
    char *last_part_of_args;
    char *string_token_first = strtok_r(args, " ", &last_part_of_args);
    print_length = atoi(string_token_first);
    sscanf(last_part_of_args, "%x", &start_memory_address);
    printf("start mem addr is 0x%x\n", start_memory_address);
    printf("******************************************************************************\n");
    printf("|  Address   | 4b Phys (Hex) | 4b Virt (Hex) | 4b Phys (Dec) | 4b Virt (Dec) |\n");
    for (int i = start_memory_address; i < start_memory_address + print_length; i = i + 4){
        //printf("[sdb] reading memory at address 0x%x\n", i);
        printf("| 0x%x | 0x   %8lx | 0x   %8lx | 0x %10ld | 0x %10ld |\n", i, mem_paddr_read(i, 4), mem_vaddr_read(i, 4), mem_paddr_read(i, 4), mem_vaddr_read(i, 4));
    }
    printf("******************************************************************************\n\n");
    return 0;
} // scan and print memory

int sdb_cmd_p(char* args){
    printf("[sdb] expression evaluation\n");
    bool expression_success;
    expression_success = false;
    u_int64_t cmd_p_result = 0;
    //cmd_p_result = expr(args, &expression_success);
    printf("%ld\t", cmd_p_result);
    printf("0x%lx\n", cmd_p_result);
    return 0;
} // expression evaluation

int sdb_cmd_w(char* args){
    printf("[sdb] add watchpoint\n");
    return 0;
} // add watchpoint

int sdb_cmd_d(char* args){
    printf("[sdb] delete watchpoint\n");
    return 0;
} // delete watchpoint

int sdb_cmd_q(char* args){
    printf("[sdb] quit NSIM\n");
    return -1;
} // quit NSIM

int sdb_cmd_h(char* args){
    for(int i = 0; i < SDB_NR_CMD; i = i + 1){
        printf("%s - %s\n", sdb_cmd_table[i].name, sdb_cmd_table[i].description);
    }
    return 0;
} // help

void sdb_set_batch_mode(){
    sdb_is_batch_mode = true;
    return;
}

void sdb_main_loop(){
    if(sdb_is_batch_mode){
        sdb_cmd_c(NULL);
        return;
    }

    for(char* str; (str = sdb_rl_gets()) != NULL; ){
        char* str_end = str + strlen(str);
        char* cmd = strtok(str, " ");
        if (cmd == NULL) {continue;}
        
        char* args = cmd + strlen(cmd) + 1;
        if(args >= str_end) {args = NULL;}

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
