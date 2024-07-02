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

// CSR list: mstatus mtvec mepc macuse

void get_regs(){
    assert(top);

    cpu.pc = top -> ioNPCDebug_PC_DYNAMIC;

    if(NR_GPRs == 16 || NR_GPRs == 32){
        cpu.gpr[0]  = top -> ioNPCDebug_GPR00;
        cpu.gpr[1]  = top -> ioNPCDebug_GPR01;
        cpu.gpr[2]  = top -> ioNPCDebug_GPR02;
        cpu.gpr[3]  = top -> ioNPCDebug_GPR03;
        cpu.gpr[4]  = top -> ioNPCDebug_GPR04;
        cpu.gpr[5]  = top -> ioNPCDebug_GPR05;
        cpu.gpr[6]  = top -> ioNPCDebug_GPR06;
        cpu.gpr[7]  = top -> ioNPCDebug_GPR07;
        cpu.gpr[8]  = top -> ioNPCDebug_GPR08;
        cpu.gpr[9]  = top -> ioNPCDebug_GPR09;
        cpu.gpr[10] = top -> ioNPCDebug_GPR10;
        cpu.gpr[11] = top -> ioNPCDebug_GPR11;
        cpu.gpr[12] = top -> ioNPCDebug_GPR12;
        cpu.gpr[13] = top -> ioNPCDebug_GPR13;
        cpu.gpr[14] = top -> ioNPCDebug_GPR14;
        cpu.gpr[15] = top -> ioNPCDebug_GPR15;
    }else{
        printf("[register] wrong GPR number\n");
        assert(0);
    }

    return;
}

void display_regs(){
    assert(top);
    assert(&cpu);

    printf("PC = 0x%lx\n", cpu.pc);

    for(int i = 0; i < NR_GPRs; i = i + 1){
        printf("GPR[%2d] = 0x%lx\t", i, cpu.gpr[i]);
        if((i + 1) % 8 == 0){
            printf("\n");
        }
    }

    printf("\n");

    return;
}