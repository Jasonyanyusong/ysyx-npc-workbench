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

    cpu.pc = top -> top_debug_io_debug_pc_o;

    if(NR_GPRs == 16 || NR_GPRs == 32){
        cpu.gpr[0]  = top -> top_debug_io_debug_gpr_00_o;
        cpu.gpr[1]  = top -> top_debug_io_debug_gpr_01_o;
        cpu.gpr[2]  = top -> top_debug_io_debug_gpr_02_o;
        cpu.gpr[3]  = top -> top_debug_io_debug_gpr_03_o;
        cpu.gpr[4]  = top -> top_debug_io_debug_gpr_04_o;
        cpu.gpr[5]  = top -> top_debug_io_debug_gpr_05_o;
        cpu.gpr[6]  = top -> top_debug_io_debug_gpr_06_o;
        cpu.gpr[7]  = top -> top_debug_io_debug_gpr_07_o;
        cpu.gpr[8]  = top -> top_debug_io_debug_gpr_08_o;
        cpu.gpr[9]  = top -> top_debug_io_debug_gpr_09_o;
        cpu.gpr[10] = top -> top_debug_io_debug_gpr_10_o;
        cpu.gpr[11] = top -> top_debug_io_debug_gpr_11_o;
        cpu.gpr[12] = top -> top_debug_io_debug_gpr_12_o;
        cpu.gpr[13] = top -> top_debug_io_debug_gpr_13_o;
        cpu.gpr[14] = top -> top_debug_io_debug_gpr_14_o;
        cpu.gpr[15] = top -> top_debug_io_debug_gpr_15_o;
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