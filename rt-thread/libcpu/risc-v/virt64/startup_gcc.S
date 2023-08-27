// See LICENSE for license details.


/* This is defined in sifive/platform.h, but that can't be included from
 * assembly. */
#include <cpuport.h>

#ifdef FLASH

	.section .init
	.globl _start
	.type _start,@function

_start:
	.cfi_startproc
	.cfi_undefined ra
.option push
.option norelax
	la gp, __global_pointer$
.option pop
	la sp, __stack_start__


	/* Load data section */
	la a0, _data_lma
	la a1, _data
	la a2, _edata
	bgeu a1, a2, 2f
1:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b
2:

	/* Clear bss section */
	la a0, __bss_start
	la a1, _end
	bgeu a0, a1, 2f
1:
	sw zero, (a0)
	addi a0, a0, 4
	bltu a0, a1, 1b
2:

	/* Call global constructors */
	//la a0, __libc_fini_array
	//call atexit
	//call __libc_init_array

	auipc ra, 0
	addi sp, sp, -16
#if __riscv_xlen == 32
	sw ra, 8(sp)
#else
	sd ra, 8(sp)
#endif

	/* argc = argv = 0 */
	li a0, 0
	li a1, 0

	csrw SRC_XIE, 0                   # clear Interrupt Registers
	csrw SRC_XIP, 0

	la t0, trap_entry
	csrw SRC_XTVEC, t0

	call entry
	/* tail exit */

1:
	j 1b

	.cfi_endproc

#else
/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/10/01     Bernard      The first version
 * 2018/12/27     Jesven       Add SMP support
 * 2020/6/12      Xim          Port to QEMU and remove SMP support
 */

#define XSTATUS_FS         (3 << 13) /* initial state of FPU, clear to disable */
#define XSTATUS_PUM        (1 << 18) 

  .global	_start
  .section ".start", "ax"
_start:
#ifndef RISCV_S_MODE
  # setup stacks per hart
  csrr t0, mhartid                  # read current hart id
  slli t0, t0, 10                   # shift left the hart id by 1024

  # park harts with id != 0
  csrr a0, mhartid                  # read current hart id
  bnez a0, park                     # if we're not on the hart 0
#endif

  csrw SRC_XIE, 0                   # clear Interrupt Registers
  csrw SRC_XIP, 0

  la t0, trap_entry
  csrw SRC_XTVEC, t0                # set Trap Vector Base Address Register

  /* set to disable FPU */
  li t0, XSTATUS_FS                 # close fpu
  csrc SRC_XSTATUS, t0
#ifdef RISCV_S_MODE
  li t0, XSTATUS_PUM                # PUM has no effect
  csrs SRC_XSTATUS, t0
#endif

.option push
.option norelax
  la gp, __global_pointer$
.option pop

  la   sp, __stack_start__
  li   t0, __STACKSIZE__
  add  sp, sp, t0
  csrw SRC_XSCRATCH, sp
  j primary_cpu_entry

park:
    wfi
    j park

#endif
