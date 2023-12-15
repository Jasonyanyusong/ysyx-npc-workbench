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
#include <difftest.h>
#include <device.h>

#ifdef CONFIG_DIFFTEST
word_t last_diff_pc = 0;
#endif

//VerilatedContext* contextp = NULL;
//VerilatedVcdC* tfp = NULL;
//static VNPC* top;

#define NPC_RUNNING 0
#define NPC_STOPPED 1

//#define CONFIG_RUNTIME_MESSAGE

int memory_delay = 0;

void pmem_write(word_t, int, word_t);
word_t pmem_read(word_t, int);

VerilatedContext* contextp;
VerilatedVcdC* tfp;
VNPC* top;

uint64_t cycle;
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
    top -> eval();
    top -> reset = 1;
    top -> eval();
    step_and_dump_wave();
    
    top -> clock = 1;
    top -> eval();
    top -> reset = 0;
    top -> eval();
    step_and_dump_wave();

    cycle = cycle + 1;

    printf("[simulation] NPC has been resetted\n");
    return;
}

void sim_exit(){
    assert(top);

    printf("[simulation] simulation exitted, total cycles = %d\n", cycle);
    step_and_dump_wave();

    #ifdef CONFIG_VCD_OUTPUT
    tfp -> close();
    #endif

    return;
}

void sim_one_cycle(){
    assert(top);

    cycle = cycle + 1;

    top -> clock = 0;
    top -> eval();
    sim_mem(0);
    step_and_dump_wave();

    top -> clock = 1;
    top -> eval();
    //sim_mem(0);
    step_and_dump_wave();

    //cycle = cycle + 1;

    get_regs(); // used as print registers or difftest

    if(top -> ioNPCDebug_Worked){
        #ifdef CONFIG_RUNTIME_MESSAGE
        printf("[verilator-sim : sim_one_cycle] NPC commited an instruction at pc = 0x%x\n", top -> ioNPCDebug_PC_COMMIT);
        #endif
    }

    #ifdef CONFIG_DIFFTEST
    if(top -> ioNPCDebug_Worked && top -> ioNPCDebug_PC_COMMIT != last_diff_pc){
        #ifdef CONFIG_RUNTIME_MESSAGE
        printf("[verilator-sim : sim_one_cycle] WBU indicated it worked, so do a difftest at pc = 0x%x\n", cpu.pc);
        #endif

        if(top -> ioNPCDebug_LS_Taken && (!in_pmem(top -> ioNPCDebug_LS_Addr))){
            difftest_skip_ref();
        }

        difftest_one_exec();
        if(!difftest_check_reg()){
            npc_state.state = NPC_ABORT;
        }
        last_diff_pc = top -> ioNPCDebug_PC_COMMIT;
    }
    #endif

    if(top -> ioNPCDebug_Halt){
        printf("NPC simulation finished at cycle = %ld, a0 = %d, ", cycle - 1, top -> ioNPCDebug_GPR10);
        if(top -> ioNPCDebug_GPR10 == 0){
            printf("HIT GOOD TRAP\n");
        }else{
            printf("HIT BAD  TRAP\n");
        }
        npc_state.state = NPC_END;

        #ifdef CONFIG_DIFFTEST
        difftest_one_exec();
        #endif
    }

    #ifdef CONFIG_RUNTIME_MESSAGE
    printf("\n\n");
    #endif
    
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
    #ifdef CONFIG_RUNTIME_MESSAGE
    printf("[verilator-sim : sim-mem] at cycle %d\n", cycle);
    #endif

    assert(top);

    word_t LS_MemAddr = (uint32_t)top -> ioNPC_iLoadStore_oMemoryAddr;
    int LS_MemLen = (int)pow(2, top -> ioNPC_iLoadStore_oMemoryLen);

    #ifdef CONFIG_RUNTIME_MESSAGE
    printf("[verilator-sim : sim_mem] LS_MemAddr is 0x%x, len = %d, op = %d\n", LS_MemAddr, LS_MemLen, top -> ioNPC_iLoadStore_oMemoryOP);
    #endif

    assert(LS_MemLen >= 1 && LS_MemLen <= 8);
    
    switch(top -> ioNPC_iLoadStore_oMemoryOP){
        case(MEM_NOP):{
            #ifdef CONFIG_RUNTIME_MESSAGE
            printf("[verilator-sim : sim_mem] LS do not access mem here\n");
            #endif

            top -> ioNPC_iLoadStore_iMemoryRead = 0;
            break;
        }
        case(MEM_READ):{
            #ifdef CONFIG_RUNTIME_MESSAGE
            printf("[verilator-sim : sim_mem] LS do read mem here");
            #endif

            word_t ret = pmem_read(LS_MemAddr, LS_MemLen);
            top -> ioNPC_iLoadStore_iMemoryRead = ret;

            #ifdef CONFIG_RUNTIME_MESSAGE
            printf(", data is 0x%lx\n", ret);
            #endif

            break;
        }
        case(MEM_WRITE):{
            #ifdef CONFIG_RUNTIME_MESSAGE
            printf("[verilator-sim : sim_mem] LS do write mem here, data is 0x%lx\n", top -> ioNPC_iLoadStore_oMemoryWrite);
            #endif

            top -> ioNPC_iLoadStore_iMemoryRead = 0;
            pmem_write(LS_MemAddr, LS_MemLen, top -> ioNPC_iLoadStore_oMemoryWrite);
            break;
        }
        default: {printf("[simulation] unknown LSU operation\n"); assert(0); break;}
    }

    top -> eval();

    word_t IF_MemAddr = (uint32_t)top -> ioNPC_iFetch_oPC;

    #ifdef CONFIG_RUNTIME_MESSAGE
    printf("[verilator-sim : sim_mem] IF_MemAddr is 0x%x\n", IF_MemAddr);
    #endif

    if(top -> ioNPC_iFetch_oMemEnable){
        #ifdef CONFIG_RUNTIME_MESSAGE
        printf("[verilator-sim] at cycle %d, get iFetch Memory request, addr 0x%x, inst 0x%x\n", cycle, IF_MemAddr, pmem_read(IF_MemAddr, 4));
        #endif

        top -> ioNPC_iFetch_iInst = pmem_read(IF_MemAddr, 4);
    }else{
        #ifdef CONFIG_RUNTIME_MESSAGE
        printf("[verilator-sim] at cycle %d, no iFetch Memory request\n", cycle);
        #endif

        top -> ioNPC_iFetch_iInst = 0;
    }

    top -> eval();

    return;
}