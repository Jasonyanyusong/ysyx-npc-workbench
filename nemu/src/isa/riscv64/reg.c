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
#include <string.h>
#include <math.h>

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

word_t ans[32] = {0};

void isa_display_gpr(){
  printf("GPR:\n");
  for(int i = 0; i < 32; i = i + 1){
    printf("x%2d = %16lx ", i, cpu.gpr[i]);
    if((i + 1) % 8 == 0){
      printf("\n");
    }
  }
  printf("\n");
  return;
}

void isa_display_fpr(){
  printf("FPR:\n");
  for(int i = 0; i < 32; i = i + 1){
    printf("f%2d = %16lx ", i, cpu.fpr[i]);
    if((i + 1) % 8 == 0){
      printf("\n");
    }
  }
  printf("\n");
  return;
}

void isa_display_csr(){
  printf("CSR:\n");
  for(int i = 0; i < 4096; i = i + 1){
    printf("csr0x%3x = %16lx ", i, cpu.csr[i]);
    if((i + 1) % 8 == 0){
      printf("\n");
    }
  }
  printf("\n");
  return;
}

void isa_reg_display() {
  isa_display_gpr();
  isa_display_fpr();
  isa_display_csr();
  return;
}

/*void isa_gpr_display() {  
  isa_display_gpr();
  isa_display_fpr();
  isa_display_csr();
}*/

word_t isa_reg_str2val(const char *s, bool *success) {
  *success = true;
  for(int current_string_compare_index = 0; current_string_compare_index < 32; current_string_compare_index = current_string_compare_index + 1)
  {
    if(strcmp(regs[current_string_compare_index], s) == 0)
    {
      return cpu.gpr[current_string_compare_index];
    }
  }
  *success = false;
  return 0;
}
