#include <common.h>
#include "syscall.h"

//#define SYSCALL_LOG

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield: sys_yield(c); break;
    case SYS_exit:  sys_exit(c);  break;
    case SYS_write: sys_write(c); break;
    case SYS_brk: sys_brk(c); break;
    case SYS_read: sys_read(c); break;
    case SYS_close: sys_close(c); break;
    case SYS_lseek: sys_lseek(c); break;
    case SYS_open: sys_open(c); break;
    case SYS_gettimeofday: AM_TIMER_UPTIME_T syscallUptime = io_read(AM_TIMER_UPTIME); c->GPRx = syscallUptime.us; break;
    default: panic("Unhandled syscall ID = %d", a[0]); break;
  }
}

void sys_close(Context *c){
  #ifdef SYSCALL_LOG
  Log("Do SYSCALL: CLOSE");
  #endif
  c->GPRx = fs_close(c->GPR2);
}

void sys_lseek(Context *c){
  #ifdef SYSCALL_LOG
  Log("Do SYSCALL: LSEEK");
  #endif
  c->GPRx = fs_lseek(c->GPR2, c->GPR3, c->GPR4);
}

void sys_open(Context *c){
  #ifdef SYSCALL_LOG
  Log("Do SYSCALL: OPEN");
  #endif
  c->GPRx = fs_open((const char *)c->GPR2, c->GPR3, c->GPR4);
}

void sys_yield(Context *c){
  #ifdef SYSCALL_LOG
  Log("Do SYSCALL: YIELD");
  #endif
  yield();
  c->GPRx = 0;
}

void sys_exit(Context *c){
  #ifdef SYSCALL_LOG
  Log("Do SYSCALL: EXIT");
  #endif
  halt(c->gpr[17]);
}

void sys_write(Context *c){
  #ifdef SYSCALL_LOG
  Log("Do SYSCALL: WRITE");
  #endif
  c->GPRx = fs_write(c->GPR2, (void *)c->GPR3, c->GPR4);
}

void sys_read(Context *c){
  #ifdef SYSCALL_LOG
  Log("Do SYSCALL: READ");
  #endif  
  c->GPRx = fs_read(c->GPR2, (void *)c->GPR3, c->GPR4);
}

void sys_brk(Context *c){
  #ifdef SYSCALL_LOG
  Log("Do SYSCALL: BRK");
  #endif
  c->GPRx = 0;
}