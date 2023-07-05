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

#include<sim.h>

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

    cpu.csr[0x300] = top -> io_NPC_CSR_mstatus;
    cpu.csr[0x305] = top -> io_NPC_CSR_mtvec;
    cpu.csr[0x341] = top -> io_NPC_CSR_mepc;
    cpu.csr[0x342] = top -> io_NPC_CSR_mcause;

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
        printf("mstatus = 0x%lx\n", top -> io_NPC_CSR_mstatus);
        printf("mtvec = 0x%lx\n",   top -> io_NPC_CSR_mtvec);
        printf("mepc = 0x%lx\n",    top -> io_NPC_CSR_mepc);
        printf("mcause = 0x%lx\n",  top -> io_NPC_CSR_mcause);
        printf("\33[0m");
    }
    return;
}