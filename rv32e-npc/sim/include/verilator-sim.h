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

#include <common.h>

#include "verilated.h"
#include "verilated_vcd_c.h"
#include "../../obj_dir/VNPC.h"

#define NR_GPRs 16
#define NR_CSRs 4096

typedef struct {
  word_t gpr[NR_GPRs];
  word_t csr[NR_CSRs];
  word_t pc;
} rtl_CPU_State;

extern rtl_CPU_State cpu;

typedef struct {
  int state;
  word_t halt_pc;
  uint32_t halt_ret;
} NPCState;

extern NPCState npc_state;

#define CONFIG_VCD_OUTPUT

void sim_init();
void sim_exit();
void sim_one_cycle();
void step_and_dump_wave();

void get_regs();

extern VerilatedContext* contextp;
extern VerilatedVcdC* tfp;
extern VNPC* top;

extern uint64_t cycle;
extern word_t instruction;
extern word_t time_of_exec;

double compute_ipc();

void sim_mem(int delay_cycle);

void display_regs();