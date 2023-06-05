#include "verilated.h"
#include "verilated_vcd_c.h"
#include "obj_dir/Vnpc.h"

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;
static Vnpc* top;

riscv64_CPU_State cpu;

void sim_sim_init();
void sim_sim_exit();
void sim_one_exec();
void sim_step_and_dump_wave();