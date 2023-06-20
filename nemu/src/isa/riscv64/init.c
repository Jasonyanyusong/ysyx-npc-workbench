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
#include <memory/paddr.h>

// this is not consistent with uint8_t
// but it is ok since we do not access the array directly
#ifdef CONFIG_RandomInstructionImage
#include "inst_img.h"
#endif

#ifndef CONFIG_RandomInstructionImage
static const uint32_t img [] = {
  0x00000297,  // auipc t0,0
  0x0002b823,  // sd  zero,16(t0)
  0x0102b503,  // ld  a0,16(t0)
  0x00100073,  // ebreak (used as nemu_trap)
  0xdeadbeef,  // some data
};
#endif

static void restart() {
  /* Set the initial program counter. */
  cpu.pc = RESET_VECTOR;

  /* The zero register is always 0. */
  cpu.gpr[0] = 0;

  IFDEF(CONFIG_RV_Privileged, cpu.csr[0x300] = 0xa00000000); // initialize mstatus to 0xa00001800
}

void init_isa() {
  /* Load built-in image. */
  memcpy(guest_to_host(RESET_VECTOR), img, sizeof(img));

#ifdef CONFIG_RandomInstructionImage
#include <stdio.h>
#include <stdlib.h>
for(int i = 0; i < 32; i = i + 1){
  cpu.gpr[i] = reg[i];
  printf("Random Instruction Image: set cpu.gpr[%d] to 0x%lx\n", i, cpu.gpr[i]);
}
#endif

  /* Initialize this virtual computer system. */
  restart();
}
