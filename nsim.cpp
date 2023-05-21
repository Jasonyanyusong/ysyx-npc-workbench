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

//========== Macro Configurations ==========
#define mem_start_addr  0x90000000
#define mem_end_addr    0x9fffffff
#define mem_size        mem_end_addr - mem_start_addr + 1

//========== Include Headers ==========

#include "verilated.h"
#include "verilated_vcd_c.h"
#include "obj_dir/Vnpc.h"

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <getopt.h>

//========== List functions that will be used later ==========

//---------- Memory manipulations ----------
uint64_t mem_host_read(void *mem_addr, int mem_length);
void mem_host_write(void *mem_addr, int mem_length, uint64_t mem_data);
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
void monitor_init_monitor();

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

//========== Memory manipulations ==========

static uint8_t *mem_pmem = NULL;

uint64_t mem_host_read(void *mem_addr, int mem_length){
    switch (mem_length){
        case 1: return *(uint8_t  *)mem_addr;
        case 2: return *(uint16_t *)mem_addr;
        case 4: return *(uint32_t *)mem_addr;
        case 8: return *(uint64_t *)mem_addr;
        default: printf("[memory] invalid read length\n"); assert(0); return 0;
    }
}

void mem_host_write(void *mem_addr, int mem_length, uint64_t mem_data){
    switch (mem_length){
        case 1: *(uint8_t  *)mem_addr = mem_data; return;
        case 2: *(uint16_t *)mem_addr = mem_data; return;
        case 4: *(uint32_t *)mem_addr = mem_data; return;
        case 8: *(uint64_t *)mem_addr = mem_data; return;
        default: printf("[memory] invalid write length\n"); assert(0); return;
    }
}

bool mem_addr_in_bound(uint64_t mem_addr){
    if(!mem_addr - mem_start_addr < mem_size)
        printf("[memory] address 0x%x out of bound [0x%x,0x%x]\n", mem_addr, mem_start_addr, mem_end_addr);
    return mem_addr - mem_start_addr < mem_size;
}

void mem_init_mem(){
    mem_pmem = (uint8_t *)malloc(mem_size);
    assert(mem_pmem);
    printf("[memory] physical memory area [0x%x,0x%x] size 0x%x\n", mem_start_addr, mem_end_addr, mem_size);
}

uint8_t* mem_guest_to_host(uint64_t paddr) { return mem_pmem + paddr - mem_start_addr; }
uint64_t mem_host_to_guest(uint8_t *haddr) { return haddr - mem_pmem + mem_start_addr; }

uint64_t mem_pmem_read(uint64_t mem_addr, int mem_length){
    uint64_t ret = mem_host_read(mem_guest_to_host(mem_addr), mem_length);
    return ret;
}
void mem_pmem_write(uint64_t mem_addr, int mem_length, uint64_t mem_data){
    mem_host_write(mem_guest_to_host(mem_addr), mem_length, mem_data);
}

uint64_t mem_paddr_read(uint64_t mem_addr, int mem_length){
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
    // TODO
    return 0;
}

int monitor_parse_args(int argc, char*argv[]){
    // TODO
    return 0;
}

void monitor_init_monitor(){
    // TODO
    return;
}

//========== Simple Debugger (SDB) User Interface ==========

char* sdb_rl_gets(){
    // TODO
    return NULL;
}

int sdb_cmd_c(char* args){
    // TODO
    return -1;
} // continue execution

int sdb_cmd_s(char* args){
    // TODO
    return -1;
} // single-step execution

int sdb_cmd_i(char* args){
    // TODO
    return -1;
} // informations (register and watchpoint)

int sdb_cmd_x(char* args){
    // TODO
    return -1;
} // scan and print memory

int sdb_cmd_p(char* args){
    // TODO
    return -1;
} // expression evaluation

int sdb_cmd_w(char* args){
    // TODO
    return -1;
} // add watchpoint

int sdb_cmd_d(char* args){
    // TODO
    return -1;
} // delete watchpoint

int sdb_cmd_q(char* args){
    // TODO
    return -1;
} // quit NSIM

int sdb_cmd_h(char* args){
    for(int i = 0; i < SDB_NR_CMD; i = i + 1){
        printf("%s - %s\n", sdb_cmd_table[i].name, sdb_cmd_table[i].description);
    }
    return -1;
} // help

void sdb_set_batch_mode(){
    // TODO
    return;
}

void sdb_main_loop(){
    // TODO
    return;
}

void sdb_init_sdb(){
    // TODO
    return;
}

//========== Main function ==========
int main(int argc, char *argv[]){
    mem_init_mem();
    sdb_cmd_h(NULL);
    printf("Welcome to riscv64-nsim\n");
    return 0;
}