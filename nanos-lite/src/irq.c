#include <common.h>

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD: {
      Log("find EVENT_YIELD"); 
      c -> mepc = c -> mepc + 4; // set error pc to the next inst using software
      break;
    }
    case EVENT_SYSCALL: {
      Log("find EVENT_SYSCALL"); 
      c -> mepc = c -> mepc + 4; // set error pc to the next inst using software
      break;
    }
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
