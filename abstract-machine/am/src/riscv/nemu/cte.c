#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    /*switch (c->mcause) {
      case -1: ev.event = EVENT_YIELD; break;
      default: ev.event = EVENT_ERROR; break;
    }*/

    if(c -> mcause == 11) {
      if(c -> GPR1 == -1){
        ev.event  = EVENT_YIELD;
        c -> mepc = c -> mepc + 4;
      }
      else if(c -> GPR1 >= 0 && c -> GPR1 <= 19){
        // 0: SYS_exit
        // 19: SYS_gettimeofday
        ev.event  = EVENT_SYSCALL;
        c -> mepc = c -> mepc + 4;
      }
      else{
        ev.event = EVENT_ERROR;
      }
    }

    c = user_handler(ev, c);

    // c -> mepc = c -> mepc + 4; // set error pc to the next inst using software

    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
