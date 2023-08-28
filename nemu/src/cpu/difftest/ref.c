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
#include <cpu/cpu.h>
#include <difftest-def.h>
#include <memory/paddr.h>

void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {
  if(direction == DIFFTEST_TO_REF){
    memcpy(guest_to_host(addr), buf, n);
  } else{
    assert(0);
  }
}

void difftest_regcpy(void *dut, bool direction) {
  CPU_state *_dut = (CPU_state*)dut;
  if(direction == DIFFTEST_TO_REF){
    for(int i = 0; i < 32; i = i + 1){
      cpu.gpr[i] = _dut->gpr[i];
    }
    #ifdef CONFIG_RV_Privileged
    for(int i = 0; i < 4096; i = i + 1){
      cpu.csr[i] = _dut->csr[i];
    }
    #endif
    cpu.pc = _dut->pc;
  }else{
    for(int i = 0; i < 32; i = i + 1){
      _dut->gpr[i] = cpu.gpr[i];
      //Log("gpr x%d is 0x%lx", i, cpu.gpr[i]);
    }
    #ifdef CONFIG_RV_Privileged
    for(int i = 0; i < 4096; i = i + 1){
      _dut->csr[i] = cpu.csr[i];
    }
    _dut->csr[0x300] = 0xa00001800; // need later refinements
    //Log("csr0x300 is 0x%lx", cpu.csr[0x300]);
    //Log("csr0x305 is 0x%lx", cpu.csr[0x305]);
    //Log("csr0x341 is 0x%lx", cpu.csr[0x341]);
    //Log("csr0x342 is 0x%lx", cpu.csr[0x342]);
    #endif
    _dut->pc = cpu.pc;
    //Log("pc is 0x%lx", cpu.pc);
  }
}

void difftest_exec(uint64_t n) {
  cpu_exec(n);
  Log("Use NEMU as ref model, CPU execute at PC = 0x%x", cpu.pc);
}

void difftest_raise_intr(word_t NO) {
  assert(0);
}

void difftest_init(int port) {
  /* Perform ISA dependent initialization. */
  init_isa();
}
