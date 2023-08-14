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

#include<stdio.h>
#include<monitor.h>
#include<device.h>
#include<verilator-sim.h>

#include<mem.h>

int main(int argc, char* argv[]){
    printf("Welcome to NPC-SIM!\n");
    init_mem();
    init_device();
    init_monitor(argc, argv);
    sim_init();
    sdb_main_loop();
    sim_exit();
    printf("Goodbye\n");
    //return 0;
    // TODO: judge exit status is good
}