#include "rtl.h"

riscv64_CPU_state cpu;

void sim_sim_init();
void sim_sim_exit();
void sim_one_exec();
void sim_step_and_dump_wave();