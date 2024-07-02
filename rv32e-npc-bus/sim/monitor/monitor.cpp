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

//#define INSERT_NOP_INSTR

char* image_path = NULL;
char* diff_so_file = NULL;

uint32_t default_img[] = {
    // need to support both RV32 and RV64
    // this image add some nop instruction so it will not cause structural hazard
    0x00000297, // auipc t0,0
    #ifdef INSERT_NOP_INSTR
    0x00000033, // addi, zero, 0(zero)
    0x00000033, // addi, zero, 0(zero)
    0x00000033, // addi, zero, 0(zero)
    0x00000033, // addi, zero, 0(zero)
    0x00000033, // addi, zero, 0(zero)
    #endif

    0x0002a023, // sw zero ,0(t0)
    #ifdef INSERT_NOP_INSTR
    0x00000033, // addi, zero, 0(zero)
    0x00000033, // addi, zero, 0(zero)
    0x00000033, // addi, zero, 0(zero)
    0x00000033, // addi, zero, 0(zero)
    0x00000033, // addi, zero, 0(zero)
    #endif

    0x0002a503, // lw a0, 0(t0)
    #ifdef INSERT_NOP_INSTR
    0x00000033, // addi, zero, 0(zero)
    0x00000033, // addi, zero, 0(zero)
    0x00000033, // addi, zero, 0(zero)
    0x00000033, // addi, zero, 0(zero)
    0x00000033, // addi, zero, 0(zero)
    #endif
    
    0x00100073, // ebreak
};

word_t img_size = -1;

void init_monitor(int argc, char* argv[]){
    printf("[monitor] parse args\n");
    parse_args(argc, argv);
    printf("[monitor] loading images\n");
    load_image();

    #ifdef CONFIG_DIFFTEST
    assert(diff_so_file != NULL);
    assert(img_size > 0);
    difftest_init(diff_so_file, img_size);
    #else
    printf("[monitor] difftest not enabled, to enable difftest, unannotate the CONFIG_DIFFTEST macro in difftest.h\n");
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
    int o;
    while ( (o = getopt_long(argc, argv, "-bhd:i:", table, NULL)) != -1) {
        switch (o) {
            case 'b': sdb_set_batch_mode(); break;
            case 'd': assert(optarg != NULL); diff_so_file = optarg; printf("diff_so_file = \"%s\"\n", diff_so_file); break;
            case 'i': assert(optarg != NULL); image_path = optarg; printf("image_path = \"%s\"\n", image_path); break;
            default:
                printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
                printf("\t-b,--batch              run with batch mode\n");
                printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
                printf("\t-i,--image=IMAGE_PATH   run NPC with image at IMAGE_PATH\n");
                printf("\n");
                exit(0);
        }
    }
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

    assert(image_path != NULL);

    FILE *fp = fopen(image_path, "rb");
    assert(fp);

    fseek(fp, 0, SEEK_END);
    word_t image_size = ftell(fp);
    assert(image_size > 0);

    img_size = image_size;
    assert(img_size == image_size);

    printf("[monitor] image %s with size %d\n", image_path, image_size);

    fseek(fp, 0, SEEK_SET);
    int fread_ret = fread(guest_to_host(MEM_START), image_size, 1, fp);
    assert(fread_ret == 1);

    int fclose_ret = fclose(fp);
    assert(fclose_ret == 0);

    return;
}