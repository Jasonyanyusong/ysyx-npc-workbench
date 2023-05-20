#include <stdio.h>
#include "./obj_dir/Vnpc.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
using namespace std;

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;
static Vnpc* top;

void rtl_step_and_dump_wave(){
  top -> eval();
  contextp -> timeInc(1);
  tfp -> dump(contextp -> time());
}

extern "C" void rtl_sim_init(){
  printf("Initialize for RTL simulation\n");
  contextp = new VerilatedContext;
  tfp = new VerilatedVcdC;
  top = new Vnpc;
  contextp -> traceEverOn(true);
  top -> trace(tfp, 0);
  tfp -> open("dump.vcd");
}

extern "C" void rtl_sim_exit(){
  rtl_step_and_dump_wave();
  tfp -> close();
}

extern "C" void attach_rtl(){
  printf("Attach RTL\n");
}


extern "C" void rtl_exec_once(){
  // TODO: implement Verilated headers
  printf("Verilated RTL exec once\n");
  uint64_t rtl_pc = top -> io_NPC_sendCurrentPC;
  printf("\t RTL: current pc is %lx\n");
  return;
}
