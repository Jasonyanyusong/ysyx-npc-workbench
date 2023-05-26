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
  "$0", "ra", "tp", "sp", "a0", "a1", "a2", "a3",
  "a4", "a5", "a6", "a7", "t0", "t1", "t2", "t3",
  "t4", "t5", "t6", "t7", "t8", "rs", "fp", "s0",
  "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8"
};

void isa_reg_display() {
  printf("*********************************************************\n");
  printf("| Name |      Hex       |     Dec      |      Oct       |\n");
  for (int i = 0; i < 32; i = i + 1)
  {
    printf("| %4s | 0x%12x | %12d | 0o%12o |\n", regs[i], cpu.gpr[i], cpu.gpr[i], cpu.gpr[i]);
  }
  printf("*********************************************************\n");
}

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