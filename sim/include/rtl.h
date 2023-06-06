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

// ---------- difftest ----------

void diff_difftest_init(long img_size);
void diff_difftest_one_exec();
bool diff_difftest_check_reg();

void (*ref_difftest_memcpy)(uint64_t addr, void *buf, size_t n, bool direction) = NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;
void (*ref_difftest_raise_intr)(uint64_t NO) = NULL;

bool rtl_difftest_checkGPR();
bool rtl_difftest_checkFPR();
bool rtl_difftest_checkCSR();

// ---------- registers ----------

void rv64rtl_gpr_get(int index);
void rv64rtl_fpr_get(int index);
void rv64rtl_csr_get(int index);

void rv64rtl_gpr_display(int index);
void rv64rtl_fpr_display(int index);
void rv64rtl_csr_display(int index);

// ----------simulation ----------

void rtl_sim_init();
void rtl_exec_once(bool printRegs);
void rtl_sim_exit();

/*// decode
typedef struct {
  union {
    uint32_t val;
  } inst;
} riscv64_ISADecodeInfo;*/

//#define isa_mmu_check(vaddr, len, type) (MMU_DIRECT)

#endif
