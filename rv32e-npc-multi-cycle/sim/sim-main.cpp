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

#define CONFIG_PLATFORM_VERILATOR
//#define CONFIG_PLATFORM_IVERILOG

#include <stdio.h>
#include <monitor.h>
#include <device.h>
#include <common.h>
#include <verilator-sim.h>

#include<mem.h>

NPCState npc_state;

bool is_exit_status_bad(){
    bool good = npc_state.state != NPC_ABORT;
    return !good;
}

int main(int argc, char* argv[]){
    printf("Welcome to NPC-SIM!\n");
    printf("[npc-sim] initializing memory & devices\n");
    init_mem();
    init_device();
    printf("[npc-sim] initializing monitor\n");
    init_monitor(argc, argv);
    printf("[npc-sim] initializing verilator-sim\n");
    sim_init();
    printf("[npc-sim] entering sdb main loop\n");
    sdb_main_loop();
    printf("[npc-sim] leving sdb main loop\n");
    sim_exit();
    printf("Goodbye\n");

    return is_exit_status_bad();
}