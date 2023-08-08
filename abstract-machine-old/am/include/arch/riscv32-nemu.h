#ifndef ARCH_H__
#define ARCH_H__

struct Context {
  // TODO: fix the order of these members to match trap.S
  //uintptr_t mepc, mcause, gpr[32], mstatus;

  //#define CONTEXT_SIZE  ((32 + 3 + 1) * XLEN)
  //#define OFFSET_SP     ( 2 * XLEN)
  //#define OFFSET_CAUSE  (32 * XLEN)
  //#define OFFSET_STATUS (33 * XLEN)
  //#define OFFSET_EPC    (34 * XLEN)

  uintptr_t gpr[32], mcause, mstatus, mepc;
  void *pdir;
};

#define GPR1 gpr[17] // a7
#define GPR2 gpr[10] // a0
#define GPR3 gpr[11] // a1
#define GPR4 gpr[12] // a2
#define GPRx gpr[10] // a0
#endif
