//========== include headers ==========

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <malloc.h>

#include "verilated.h"
#include "verilated_vcd_c.h"
#include "obj_dir/Vnpc.h"

//========== macro definations ==========
#define mem_start 0x90000000
#define mem_end   0x9fffffff
#define mem_size mem_end - mem_start

//========== Verilator/Simulation functions ==========

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;

static Vnpc* top;

void step_and_dump_wave(){
    top -> eval();
    contextp -> timeInc(1);
    tfp -> dump(contextp -> time());
}

void sim_init(){
    contextp = new VerilatedContext;
    tfp = new VerilatedVcdC;
    top = new Vnpc;
    contextp -> traceEverOn(true);
    top -> trace(tfp, 0);
    tfp -> open("dump.vcd");
}

void sim_exit(){
    step_and_dump_wave();
    tfp -> close();
}

//========== Memory manipulation functions ==========

static uint8_t *pmem = NULL;

static inline uint64_t host_read(void *addr, int len) {
    switch (len) {
        case 1: return *(uint8_t  *)addr;
        case 2: return *(uint16_t *)addr;
        case 4: return *(uint32_t *)addr;
        case 8: return *(uint64_t *)addr;
        default: printf("Host Read: Invalid length\n"); assert(0);
  }
}

static inline void host_write(void *addr, int len, uint64_t data) {
    switch (len) {
        case 1: *(uint8_t  *)addr = data; return;
        case 2: *(uint16_t *)addr = data; return;
        case 4: *(uint32_t *)addr = data; return;
        case 8: *(uint64_t *)addr = data; return;
        default: printf("Host Write: Invalid length\n"); assert(0);
  }
}

void mem_init(){
    pmem = (uint8_t *)malloc(mem_size);
    assert(pmem);
    printf("Physical Memory area is [0x%x,0x%x], size is 0x%x\n", mem_start, mem_end, mem_size);
    return;
}

uint8_t* guest_to_host(uint64_t paddr)  {return pmem + paddr - mem_start;}
uint64_t host_to_guest(uint8_t *haddr)  {return haddr - pmem + mem_start;}
bool     mem_in_bound(uint64_t addr)    {return addr - mem_start < mem_size;}

static uint64_t pmem_read(uint64_t addr, int len)                {uint64_t ret = host_read(guest_to_host(addr), len); return ret;}
static void pmem_write(uint64_t addr, int len, uint64_t data)    {host_write(guest_to_host(addr), len, data);}

uint64_t paddr_read(uint64_t addr, int len) {
    if (mem_in_bound(addr)) return pmem_read(addr, len);
    printf("Physical Memory Read : ADDRESS OUT OF BOUND at %lx\n", addr);
    assert(addr - mem_start < mem_size);
    return 0;
}

void paddr_write(uint64_t addr, int len, uint64_t data) {
    if (mem_in_bound(addr)) { pmem_write(addr, len, data); return; }
    printf("Physical Memory Write : ADDRESS OUT OF BOUND at %lx\n", addr);
    assert(addr - mem_start < mem_size);
    return;
}

uint64_t mem_read(uint64_t addr, int len)                   {return paddr_read(addr, len);}
uint64_t mem_write(uint64_t addr, int len, uint64_t val)    {paddr_write(addr, len, val); return val;}

//========== Debugger User Interface functions ==========

void sdb_c(){
    return;
}

void sdb_si(){
    return;
}

void sdb_showreg(){
    return;
}

void sdb_showmem(){
    return;
}

//========== main function ==========
int main(int argc, char *argv[]){
    printf("Welcome to NPC-Basic Simulation Environment\n");
    return 0;
}