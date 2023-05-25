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
    Linux: "verilator --cc --exe --build --trace nsim.cpp npc.v -LDFLAGS -lreadline"
*/

//========== Macro Configurations ==========
#define mem_start_addr  0x010000000
#define mem_end_addr    0x01fffffff
#define mem_size        mem_end_addr - mem_start_addr + 1

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

//========== List functions and variables that will be used later ==========

//---------- Memory manipulations ----------
static inline uint64_t mem_host_read(uint8_t *mem_addr, int mem_length);
static void mem_host_write(void *mem_addr, int mem_length, uint64_t mem_data);
bool mem_addr_in_bound(uint64_t mem_addr);

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
void reg_display();

//---------- RTL simulation ----------

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;
static Vnpc* top;

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

//========== RTL simulation ==========

void sim_sim_init(){
    printf("\33[1;33m[sim] initializing simulation\33[0m\n");
    contextp = new VerilatedContext;
    tfp = new VerilatedVcdC;
    top = new Vnpc;
    contextp -> traceEverOn(true);
    top -> trace(tfp, 0);
    tfp -> open("dump.vcd");
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
    tfp -> close();
    return;
}

void sim_one_exec(){
    if(!(nsim_state.state == NSIM_CONTINUE || nsim_state.state == NSIM_STOP)){
        printf("\33[1;33m[sim] current state indicates simulation can not continue\33[0m\n");
        return;
    }

    printf("\33[1;33m[sim] execution one round\33[0m\n");
    top -> clock = 0;// simulate posedge

    // Step I: fetch instruction and send back
    printf("\33[1;33m[sim] Phase I: Instruction fetch\33[0m\n");
    uint64_t sim_getCurrentPC = top -> io_NPC_sendCurrentPC;
    printf("\33[1;33m[sim] current pc is 0x%lx\33[0m\n", sim_getCurrentPC);
    uint32_t sim_currentInst = mem_paddr_read(sim_getCurrentPC, 4);
    top -> io_NPC_getInst = sim_currentInst;
    printf("\33[1;33m[sim] current instruction is 0x%x\33[0m\n", sim_currentInst);
    top -> eval();

    // Step II: decode instruction
    printf("\33[1;33m[sim] Phase II: Instruction decode\33[0m\n");
    top -> eval();

    // Step III: EXU execution
    printf("\33[1;33m[sim] Phase III: execute\33[0m\n");
    top -> eval();

    // Step IV: Load and store
    printf("\33[1;33m[sim] Phase IV: load and store\33[0m\n");
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
    printf("\33[1;33m[sim] Phase V: write back\33[0m\n");
    top -> eval();

    sim_step_and_dump_wave();

    top -> clock = 1; //simulate posedge

    top -> eval();

    for(int i = 0; i < 32; i = i + 1){
        reg_get_reg_from_sim(i);
    }
    reg_get_pcreg_from_sim();
    reg_display();

    nsim_state.state = NSIM_CONTINUE;
    nsim_state.halt_pc = reg_pc;

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

    if(top -> io_NPC_error == 0b1){
        // NPC raised error, stop simulation
        state_set_state(NSIM_ABORT);
        //nsim_state.state = NSIM_ABORT;
    }

    sim_step_and_dump_wave();

    return;
}

void sim_step_and_dump_wave(){
    top -> eval();
    contextp -> timeInc(1);
    tfp -> dump(contextp -> time());
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
        default:  printf("\33[1;34m[reg] unknown register index\33[0m\n"); assert(0);                              break;
    }
    return;
}
void reg_get_pcreg_from_sim(){
    printf("\33[1;34m[reg] getting PC registers from simulation environment\33[0m\n");
    reg_pc = top -> io_NPC_sendCurrentPC;
    reg_snpc = top -> io_NPC_sendCurrentPC + 4;
    reg_dnpc = top -> io_NPC_sendNextPC;
    return;
}
void reg_display(){
    printf("\33[1;34m[reg] print registers\n");
    printf("pc = 0x%lx, snpc = 0x%lx, dnpc = 0x%lx\n", reg_pc, reg_snpc, reg_dnpc);
    for(int i = 0; i < 32; i = i + 1){
        printf("x%2d = 0x%lx\t", i, nsim_gpr[i].value);
        if((i + 1) % 4 == 0) {printf("\n");}
    }
    printf("\33[0m");
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

bool mem_addr_in_bound(uint64_t mem_addr){
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
    uint64_t ret = mem_host_read(mem_guest_to_host(mem_addr), mem_length);
    return ret;
}
void mem_pmem_write(uint64_t mem_addr, int mem_length, uint64_t mem_data){
    mem_host_write(mem_guest_to_host(mem_addr), mem_length, mem_data);
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
    }

    FILE *fp = fopen(monitor_img_file, "rb");
    assert(fp);

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);

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
        reg_display();
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
    monitor_init_monitor(argc, argv);
    mem_init_mem();
    //sdb_cmd_h(NULL);
    memcpy(mem_guest_to_host(mem_start_addr), img, sizeof(img));
    state_set_state(NSIM_CONTINUE);
    printf("Welcome to riscv64-nsim\n");
    sim_sim_init();
    sdb_main_loop();
    sim_sim_exit();
    return 0;
}
