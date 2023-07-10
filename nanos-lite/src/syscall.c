#include <common.h>
#include "syscall.h"

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield: sys_yield(c); break;
    case SYS_exit:  sys_exit(c);  break;
    case SYS_write: c->GPRx = fs_write(a[1], (void *)a[2], a[3]); break;
    case SYS_brk: sys_brk(c); break;
    case SYS_read: c->GPRx = fs_read(a[1], (void *)a[2], a[3]); break;
    case SYS_close: c->GPRx = fs_close(a[1]); break;
    case SYS_lseek: c->GPRx = fs_lseek(a[1], a[2], a[3]); break;
    case SYS_open: c->GPRx = fs_open((char *)a[1], a[2], a[3]); break;
    case SYS_gettimeofday: AM_TIMER_UPTIME_T syscallUptime = io_read(AM_TIMER_UPTIME); c->GPRx = syscallUptime.us; break;
    default: panic("Unhandled syscall ID = %d", a[0]); break;
  }
}

void sys_yield(Context *c){
  Log("Do SYSCALL: YIELD");
  yield();
  c->GPRx = 0;
}

void sys_exit(Context *c){
  Log("Do SYSCALL: EXIT");
  halt(c->gpr[17]);
}

void sys_brk(Context *c){
  Log("Do SYSCALL: BRK");
  c->GPRx = 0;
}