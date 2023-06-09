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
#include "cpu/difftest.h"

word_t vaddr_ifetch(vaddr_t addr, int len) {
  return paddr_read(addr, len);
}

word_t vaddr_read(vaddr_t addr, int len) {
  word_t ret = paddr_read(addr, len);
  //printf("In vaddr_read at pc = 0x%8lx, addr = 0x%16lx, len = %d, ret  = 0x%16lx\n", cpu.pc, addr, len, ret);
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  //printf("In vaddr_writeat pc = 0x%8lx, addr = 0x%16lx, len = %d, data = 0x%16lx\n", cpu.pc, addr, len, data);
  paddr_write(addr, len, data);
  //ref_difftest_memcpy(addr - 8, (void *)guest_to_host(addr), len + 16, DIFFTEST_TO_REF);
}
