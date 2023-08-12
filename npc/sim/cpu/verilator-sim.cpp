/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing 101 High School
* Copyright (c) 2023 Yusong Yan, University of Washington - Seattle
*
* YSYX-NPC-SIM is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <verilator-sim.h>

//VerilatedContext* contextp = NULL;
//VerilatedVcdC* tfp = NULL;
//static VNPC* top;

void sim_init(){
    printf("[simulation] initializing simulation\n");

    #ifdef CONFIG_VCD_OUTPUT
    contextp = new VerilatedContext;
    tfp = new VerilatedVcdC;
    #endif

    top = new VNPC;

    #ifdef CONFIG_VCD_OUTPUT
    contextp -> traceEverOn(true);
    top -> trace(tfp, 0);
    tfp -> open("dump.vcd");
    #endif

    printf("[simulation] simulation initialized, now reset NPC\n");

    top -> clock = 0;
    top -> reset = 1;
    step_and_dump_wave();
    
    top -> clock = 1;
    step_and_dump_wave();
    top -> reset = 0;
    top -> eval();

    printf("[simulation] NPC has been resetted\n");
    return;
}

void sim_exit(){
    assert(0);
    // TODO: implement this function
}

void sim_one_exec(){
    assert(0);
    // TODO: implement this function
}

void step_and_dump_wave(){
    assert(0);
    // TODO: implement his function
}