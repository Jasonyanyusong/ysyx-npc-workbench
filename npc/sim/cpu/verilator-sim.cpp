/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing 101 High School
* Copyright (c) 2023 Yusong Yan, University of Washington - Seattle
*
* YSYX-NPC-SIM is licensed under Mulan PSL v2.
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

#include <verilator-sim.h>
#include <mem.h>
#include <common.h>
#include <math.h>

//VerilatedContext* contextp = NULL;
//VerilatedVcdC* tfp = NULL;
//static VNPC* top;

int memory_delay = 0;

void pmem_write(word_t, int, word_t);
word_t pmem_read(word_t, int);

VerilatedContext* contextp;
VerilatedVcdC* tfp;
VNPC* top;

word_t cycle;
word_t instruction;
word_t time_of_exec;

void sim_init(){
    printf("[simulation] initializing simulation\n");

    #ifdef CONFIG_VCD_OUTPUT
    contextp = new VerilatedContext;
    tfp = new VerilatedVcdC;
    #endif

    top = new VNPC;

    #ifdef CONFIG_VCD_OUTPUT
    contextp -> traceEverOn(true);
    top -> trace(tfp, 0);
    tfp -> open("dump.vcd");
    #endif

    cycle = 0;
    instruction = 0;
    time_of_exec = 0;

    printf("[simulation] simulation initialized, now reset NPC\n");

    top -> clock = 0;
    top -> reset = 1;
    step_and_dump_wave();
    
    top -> clock = 1;
    step_and_dump_wave();
    top -> reset = 0;
    top -> eval();

    printf("[simulation] NPC has been resetted\n");
    return;
}

void sim_exit(){
    assert(top);

    printf("[simulation] simulation exitted\n");
    step_and_dump_wave();

    #ifdef CONFIG_VCD_OUTPUT
    tfp -> close();
    #endif

    return;
}

void sim_one_cycle(){
    assert(top);

    top -> clock = 0;
    step_and_dump_wave();

    sim_mem(0);

    top -> clock = 1;
    step_and_dump_wave();

    cycle = cycle + 1;
    
    return;
}

void step_and_dump_wave(){
    #ifdef CONFIG_VCD_OUTPUT
    assert(contextp);
    assert(tfp);
    #endif

    top -> eval();

    #ifdef CONFIG_VCD_OUTPUT
    contextp -> timeInc(1);
    tfp -> dump(contextp -> time());
    #endif

    return;
}

double compute_ipc(){
    assert(cycle > 0);
    assert(instruction > 0);

    double ans = (double)instruction / (double)cycle;
    printf("[simulation] IPC is %lf\n", ans);
    
    return ans;
}

#define MEM_NOP 0b00
#define MEM_READ 0b01
#define MEM_WRITE 0b10

void sim_mem(int delay_cycle){
    assert(top);

    word_t LS_MemAddr = top -> ioNPC_iLoadStore_oMemoryAddr;
    int LS_MemLen = (int)pow(2, top -> ioNPC_iLoadStore_oMemoryLen);

    assert(LS_MemLen >= 1 && LS_MemLen <= 8);
    
    switch(top -> ioNPC_iLoadStore_oMemoryOP){
        case(MEM_NOP):{
            top -> ioNPC_iLoadStore_iMemoryRead = 0;
            break;
        }
        case(MEM_READ):{
            top -> ioNPC_iLoadStore_iMemoryRead = pmem_read(LS_MemAddr, LS_MemLen);
            break;
        }
        case(MEM_WRITE):{
            top -> ioNPC_iLoadStore_iMemoryRead = 0;
            pmem_write(LS_MemAddr, LS_MemLen, top -> ioNPC_iLoadStore_oMemoryWrite);
            break;
        }
        default: {printf("[simulation] unknown LSU operation\n"); assert(0); break;}
    }

    top -> eval();

    word_t IF_MemAddr = top -> ioNPC_iFetch_oPC;

    if(top -> ioNPC_iFetch_oMemEnable){
        top -> ioNPC_iFetch_iInst = pmem_read(IF_MemAddr, 4);
    }else{
        top -> ioNPC_iFetch_iInst = 0;
    }

    top -> eval();

    return;
}