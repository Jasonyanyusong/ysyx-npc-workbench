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
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_display_gpr(){
  printf("GPR:\n");
  for(int i = 0; i < 32; i = i + 1){
    printf("x%2d = %8lx ", i, cpu.gpr[i]);
    if((i + 1) % 4 == 0){
      printf("\n");
    }
  }
  printf("\n");
  return;
}

void isa_display_csr(){
  printf("CSR:\n");
  /*for(int i = 0; i < 4096; i = i + 1){
    printf("csr0x%3x = %16lx ", i, cpu.csr[i]);
    if((i + 1) % 8 == 0){
      printf("\n");
    }
  }*/

  // M state CSRs
  #ifdef CONFIG_RV_Privileged
  printf("(csr[0xF11] MRO) mvendorid  = 0x%lx\n", cpu.csr[0xF11]);
  printf("(csr[0xF12] MRO) marchid    = 0x%lx\n", cpu.csr[0xF12]);
  printf("(csr[0xF13] MRO) mimpid     = 0x%lx\n", cpu.csr[0xF13]);
  printf("(csr[0xF14] MRO) mhartid    = 0x%lx\n", cpu.csr[0xF14]);
  printf("(csr[0xF15] MRO) mconfigptr = 0x%lx\n", cpu.csr[0xF15]);
  printf("(csr[0x300] MRW) mstatus    = 0x%lx\n", cpu.csr[0x300]);
  printf("(csr[0x301] MRW) misa       = 0x%lx\n", cpu.csr[0x301]);
  printf("(csr[0x302] MRW) medeleg    = 0x%lx\n", cpu.csr[0x302]);
  printf("(csr[0x303] MRW) mideleg    = 0x%lx\n", cpu.csr[0x303]);
  printf("(csr[0x304] MRW) mie        = 0x%lx\n", cpu.csr[0x304]);
  printf("(csr[0x305] MRW) mtvec      = 0x%lx\n", cpu.csr[0x305]);
  printf("(csr[0x306] MRW) mcounteren = 0x%lx\n", cpu.csr[0x306]);
  printf("(csr[0x340] MRW) mscratch   = 0x%lx\n", cpu.csr[0x340]);
  printf("(csr[0x341] MRW) mepc       = 0x%lx\n", cpu.csr[0x341]);
  printf("(csr[0x342] MRW) mcause     = 0x%lx\n", cpu.csr[0x342]);
  printf("(csr[0x343] MRW) mtval      = 0x%lx\n", cpu.csr[0x343]);
  printf("(csr[0x344] MRW) mscratch   = 0x%lx\n", cpu.csr[0x344]);
  printf("(csr[0x34A] MRW) mtinst     = 0x%lx\n", cpu.csr[0x34A]);
  printf("(csr[0x34B] MRW) mtval2     = 0x%lx\n", cpu.csr[0x34B]);
  #endif

  printf("\n");
  return;
}

void isa_reg_display() {
  printf("PC: 0x%lx\n\n", cpu.pc);
  isa_display_gpr();
  //isa_display_fpr();
  isa_display_csr();
  return;
}

word_t isa_reg_str2val(const char *s, bool *success) {
  return 0;
}
