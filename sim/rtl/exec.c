#include "rtl.h"

void sim_sim_init(){
    printf("[sim] initializing simulation\n");
    contextp = new VerilatedContext;
    tfp = new VerilatedVcdC;
    top = new Vnpc;
    contextp -> traceEverOn(true);
    top -> trace(tfp, 0);
    tfp -> open("dump.vcd");
    printf("[sim] initialize finished\n");

    top -> clock = 0;
    top -> io_NPC_startPC = CONFIG_MBASE + CONFIG_PC_RESET_OFFSET;
    top -> reset = 1;
    top -> eval();
    sim_step_and_dump_wave();
    top -> clock = 1;
    top -> eval();
    sim_step_and_dump_wave();
    top -> reset = 0;
    top -> eval();

    printf("[sim] module's start PC is 0x%x\n", CONFIG_MBASE + CONFIG_PC_RESET_OFFSET);
    return;
}

void sim_sim_exit(){
    printf("[sim] exit simulation\n");
    sim_step_and_dump_wave();
    tfp -> close();
    return;
}

void sim_one_exec();

void sim_step_and_dump_wave();