/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
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

#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if(cpu.pc != ref_r -> pc)
  {
    printf("PC = 0x%lx, Difftest failed at PC, Difftest get 0x%lx, NEMU get 0x%lx\n", cpu.pc, ref_r -> pc, cpu.pc);
    return false;
  }

  #ifdef CONFIG_RV_Privileged
  int check_csr_index[4] = {0x300, 0x305, 0x341, 0x342};
  for(int i = 0; i < 4; i = i + 1){
    if(cpu.csr[i] != ref_r -> csr[i]){
      printf("PC = 0x%lx, Difftest failed at CSR[0x%3x], Difftest get 0x%lx, NEMU get 0x%lx\n", cpu.pc, i, ref_r -> csr[i], cpu.csr[i]);
      return false;
    }
  }
  #endif

  for(int integer_register_index = 0; integer_register_index < 32; integer_register_index = integer_register_index + 1)
  {
    if(cpu.gpr[integer_register_index] != ref_r -> gpr[integer_register_index])
    {
      printf("PC = 0x%lx, Difftest Reg Compare failed at GPR[%d], Difftest Get 0x%lx, NEMU Get 0x%lx\n", cpu.pc, integer_register_index, ref_r -> gpr[integer_register_index], cpu.gpr[integer_register_index]);
      return false;
    }
  }

  // M-State CSR checkings
  //printf("PC = 0x%lx, Difftest success\n", cpu.pc);
  return true;
}

void isa_difftest_attach() {
}
