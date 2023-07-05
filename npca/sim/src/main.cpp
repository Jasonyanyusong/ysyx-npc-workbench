/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing No.101 High School
* Copyright (c) 2023 Yusong Yan, University of Washington - Seattle
*
* NSIM is licensed under Mulan PSL v2.
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

#include<common.h>

int main(int argc, char *argv[]){
    if(generate_dump_wave_file == true){
        printf("Do you want to make this computer have no space left?\n");
        //assert(0);
    }
    mem_init_mem();
    device_init_devices();
    trace_init_trace();
    monitor_init_monitor(argc, argv);
    //memcpy(mem_guest_to_host(mem_start_addr), img, sizeof(img));
    state_set_state(NSIM_CONTINUE);
    diff_difftest_init(monitor_img_size);
    printf("Welcome to riscv64-nsim\n");
    sim_sim_init();
    sdb_main_loop();
    sim_sim_exit();
    return 0;
}