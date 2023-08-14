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

#include <monitor.h>
#include <mem.h>
#include <difftest.h>

char* image_path = NULL;
char* diff_so_file = NULL;

uint32_t default_img[] = {
    // TODO: write binary of default image
    // need to support both RV32 and RV64
    0x800002b7, // lui t0, 0x80000
    0x0002a023, // sw zero ,0(t0)
    0x0002a503, // lw a0, 0(t0)
    0x00100073, // ebreak
};

word_t img_size = -1;

void init_monitor(int argc, char* argv[]){
    parse_args(argc, argv);
    load_image();

    #ifdef CONFIG_DIFFTEST
    assert(diff_so_file != NULL);
    assert(img_size > 0);
    difftest_init(diff_so_file, img_size);
    #endif

    printf("[monitor] monitor initialized\n");

    return;
}

void parse_args(int argc, char* argv[]){
    assert(0);
    // TODO: implement this function
    return;
}

void load_image(){
    assert(0);
    // TODO: implememt this function
    return;
}