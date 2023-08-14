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
    const struct option table[] = {
        {"batch"    , no_argument      , NULL, 'b'},
        {"diff"     , required_argument, NULL, 'd'},
        {"help"     , no_argument      , NULL, 'h'},
        {"img"      , required_argument, NULL, 'i'},
    };
    // TODO: implement functionality of parse_args here
    assert(0);
    return;
}

void load_image(){
    if(image_path == NULL){
        printf("[monitor] no image file is given, using built-in RISC-V image, support both RV32 and RV64\n");
        assert(default_img);
        assert(sizeof(default_img) > 0);
        memcpy(guest_to_host(MEM_START), default_img, sizeof(default_img));
        img_size = sizeof(default_img);
        return;
    }

    FILE *fp = fopen(image_path, "rb");
    assert(fp);

    fseek(fp, 0, SEEK_END);
    word_t image_size = ftell(fp);
    assert(image_size > 0);

    printf("[monitor] image %s with size %d\n", image_path, image_size);

    fseek(fp, 0, SEEK_SET);
    int fread_ret = fread(guest_to_host(MEM_START), image_size, 1, fp);
    assert(fread_ret == 1);

    fclose(fp);

    return;
}