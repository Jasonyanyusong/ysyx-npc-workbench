#include <utils.h>

NSIMState nsim_state = { .state = NSIM_STOP};

int is_exit_status_bad() {
  int good = (nsim_state.state == NSIM_END && nsim_state.halt_ret == 0) ||
    (nsim_state.state == NSIM_QUIT);
  return !good;
}