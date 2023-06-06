#ifndef __ISA_RISCV64_H__
#define __ISA_RISCV64_H__

#include <common.h>
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "obj_dir/Vnpc.h"

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;
static Vnpc* top;

typedef struct {
  uint64_t gpr[32];
  uint64_t fpr[32];
  vaddr_t pc;
} riscv64_CPU_state;

riscv64_CPU_state cpu;

void rv64rtl_gpr_display(int index);
void rv64rtl_fpr_display(int index);
void rv64rtl_csr_display(int index);

void rtl_sim_init();
void rtl_exec_once(bool printRegs);
void rtl_sim_exit();

bool rtl_difftest_checkGPR();
bool rtl_difftest_checkFPR();
bool rtl_difftest_checkCSR();

/*// decode
typedef struct {
  union {
    uint32_t val;
  } inst;
} riscv64_ISADecodeInfo;*/

//#define isa_mmu_check(vaddr, len, type) (MMU_DIRECT)

#endif
