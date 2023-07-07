#include <common.h>
#include "syscall.h"

enum {
  SYS_exit,
  SYS_yield,
  SYS_open,
  SYS_read,
  SYS_write,
  SYS_kill,
  SYS_getpid,
  SYS_close,
  SYS_lseek,
  SYS_brk,
  SYS_fstat,
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times,
  SYS_gettimeofday
};

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield: sys_yield(c); break;
    case SYS_exit:  sys_exit(c);  break;
    case SYS_write:{
      uintptr_t i = 0;
      if(a[1] == 1 || a[1] == 2){
        for(; a[3] > 0; a[3] --){
          putch(((char*)a[2])[i]);
          i ++;
        }
        c->GPRx = i;
      }
      else{
        c->GPRx = -1;
      }
      break;
  }
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