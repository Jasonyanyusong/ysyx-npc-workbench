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

// CSR list: mstatus. mtvec mepc macuse

void get_regs(){
    assert(top);

    cpu.pc = top -> ioNPCDebug_PC;

    cpu.csr[0] = top -> ioNPCDebug_MSTATUS;
    cpu.csr[1] = top -> ioNPCDebug_MTVEC;
    cpu.csr[2] = top -> ioNPCDebug_MEPC;
    cpu.csr[3] = top -> ioNPCDebug_MCAUSE;

    if(NR_GPRs == 16 || NR_GPRs == 32){
        // TODO: get first 16 registers from top
    }else{
        printf("[register] wrong GPR number");
        assert(0);
    }

    if(NR_GPRs == 32){
        // TODO: get last 16 regsiters from top
    }

    return;
}

void display_regs(){
    assert(top);
    assert(&cpu);

    printf("PC = 0x%lx\n", cpu.pc);

    for(int i = 0; i < NR_GPRs; i = i + 1){
        printf("GPR[%d] = 0x%lx\t", cpu.gpr[i]);
        if((i + 1) % 8 == 0){
            printf("\n")
        }
    }

    for(int i = 0; i < NR_CSRs; i = i + 1){
        printf("CSR[%d] = 0x%lx\t", cpu.csr[i]);
        if((i + 1) % 8 == 0){
            printf("\n")
        }
    }

    return;
}