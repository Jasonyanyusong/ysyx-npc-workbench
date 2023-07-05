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

#include<common.h>

typedef struct{
    int index;
    uint64_t value;
    //char alias[4];
} NSIMGetGPR;

NSIMGetGPR nsim_gpr[32];

uint64_t reg_pc, reg_snpc, reg_dnpc;

void reg_get_reg_from_sim();
void reg_get_pcreg_from_sim();
void reg_display(bool sdb_print_regs);

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;
static Vnpc* top;

typedef struct{
    uint64_t gpr[32];
    uint64_t csr[4096];
    uint64_t pc = mem_start_addr;
} riscv64_CPU_State;

riscv64_CPU_State cpu;

void sim_sim_init();
void sim_sim_exit();
void sim_one_exec();
void sim_step_and_dump_wave();