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

#include<monitor.h>

long monitor_load_img(){
    if(monitor_img_file == NULL){
        printf("[monitor] no image file is given, using built-in RISCV image\n");
        memcpy(mem_guest_to_host(mem_start_addr), img, sizeof(img));
        monitor_img_size = sizeof(img);
        return -1;
    }

    FILE *fp = fopen(monitor_img_file, "rb");
    assert(fp);

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    monitor_img_size = size;

    printf("[monitor] image is %s size is %ld\n", monitor_img_file, size);

    fseek(fp, 0, SEEK_SET);
    int ret = fread(mem_guest_to_host(mem_start_addr), size, 1, fp);
    assert(ret == 1);

    fclose(fp);
    return 0;
}

int monitor_parse_args(int argc, char*argv[]){
    const struct option table[] = {
        {"batch"    , no_argument      , NULL, 'b'},
        {"log"      , required_argument, NULL, 'l'},
        {"diff"     , required_argument, NULL, 'd'},
        {"port"     , required_argument, NULL, 'p'},
        {"help"     , no_argument      , NULL, 'h'},
        {"readbin"  , required_argument, NULL, 'i'},
        {"readelf"  , required_argument, NULL, 'r'},
        {"readdiasm", required_argument, NULL, 'a'},
        {"script"   , no_argument      , NULL, 's'},
        {0          , 0                , NULL,  0 },
    };
    int o;
    while ( (o = getopt_long(argc, argv, "-sbhl:d:p:r:a:", table, NULL)) != -1) {
        switch (o) {
            //case 'b': sdb_set_batch_mode(); break;
            case 'p': sscanf(optarg, "%d", &monitor_difftest_port); break;
            case 'l': monitor_log_file = optarg; printf("log_file = \"%s\"\n", monitor_log_file); break;
            case 'd': monitor_diff_so_file = optarg; printf("diff_so_file = \"%s\"\n", monitor_diff_so_file); break;
            case 'r': monitor_elf_file = optarg; printf("elf_file = \"%s\"\n", monitor_elf_file); break;
            case 'a': monitor_das_file = optarg; printf("das_file = \"%s\"\n", monitor_das_file); break;
            case 'i': monitor_img_file = optarg; printf("bin_file = \"%s\"\n", monitor_img_file); break;
            case 's': monitor_script_mode = true; printf("using script mode, automatically execute\n"); break;
            case 1:   monitor_img_file = optarg; printf("img_file = \"%s\"\n", monitor_img_file); return 0;
            default:
                printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
                printf("\t-b,--batch              run with batch mode\n");
                printf("\t-l,--log=FILE           output log to FILE\n");
                printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
                printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
                printf("\n");
                exit(0);
        }
    }
    return 0;
}

void monitor_init_monitor(int argc, char*argv[]){
    monitor_parse_args(argc, argv);
    monitor_load_img();
    return;
}

char* sdb_rl_gets(){
    char* sdb_line_read = NULL;

    if(sdb_line_read){
        free(sdb_line_read);
        sdb_line_read = NULL;
    }

    sdb_line_read = readline("(npca-nsim) ");

    if (sdb_line_read && *sdb_line_read){
        add_history(sdb_line_read);
    }

    return sdb_line_read;
}

int sdb_cmd_c(char* args){
    //printf("[sdb] continue NPC execution\n");
    if(nsim_state.state == NSIM_STOP){state_set_state(NSIM_CONTINUE);}
    while((nsim_state.state == NSIM_CONTINUE)){
        sim_one_exec();
    }
    statistics_show();
    return 0;
} // continue execution

int sdb_cmd_s(char* args){
    if (args == NULL){
        //printf("[sdb] 1 setp NPC execution\n");
        sim_one_exec();
        //cpu_exec(1);
    }
    else{
        int sdb_cmd_si_n = atoi(args);
        if(sdb_cmd_si_n < -1){
        printf("Invalid input\n");
        return 0;
    }
    //printf("[sdb] %d setp NPC execution\n", sdb_cmd_si_n);
    for(int i = 0; i < sdb_cmd_si_n; i = i + 1){
        sim_one_exec();
    }
    //cpu_exec(cmd_si_n);
    }
    return 0;
} // single-step execution

int sdb_cmd_i(char* args){
    if (args == NULL){
        printf("No Subcommand\n");
        return 0;
    }
    else{
        if (strcmp(args, "r") == 0){
        //printf("[sdb] list registers\n");
        reg_display(true);
        //isa_gpr_display();
    }
    else if (strcmp(args, "w") == 0){
        //printf("[sdb] list watchpoints\n");
        //print_WP();
    }
    else{
        printf("Subcommand Not Defined\n");
    }
  }
    return 0;
} // informations (register and watchpoint)

int sdb_cmd_x(char* args){
    //printf("[sdb] scan and print memory\n");
    int print_length;
    uint64_t start_memory_address;
    char *last_part_of_args;
    char *string_token_first = strtok_r(args, " ", &last_part_of_args);
    print_length = atoi(string_token_first);
    sscanf(last_part_of_args, "%lx", &start_memory_address);
    //printf("start mem addr is 0x%lx\n", start_memory_address);
    printf("******************************************************************************\n");
    printf("|  Address   | 4b Phys (Hex) | 4b Virt (Hex) | 4b Phys (Dec) | 4b Virt (Dec) |\n");
    for (uint64_t i = start_memory_address; i < start_memory_address + print_length; i = i + 4){
        //printf("[sdb] reading memory at address 0x%x\n", i);
        printf("| 0x%lx | 0x   %8lx | 0x   %8lx | 0x %10ld | 0x %10ld |\n", i, mem_paddr_read(i, 4), mem_vaddr_read(i, 4), mem_paddr_read(i, 4), mem_vaddr_read(i, 4));
    }
    printf("******************************************************************************\n\n");
    return 0;
} // scan and print memory

int sdb_cmd_p(char* args){
    //printf("[sdb] expression evaluation\n");
    bool expression_success;
    expression_success = false;
    u_int64_t cmd_p_result = 0;
    //cmd_p_result = expr(args, &expression_success);
    printf("%ld\t", cmd_p_result);
    printf("0x%lx\n", cmd_p_result);
    return 0;
} // expression evaluation

int sdb_cmd_w(char* args){
    //printf("[sdb] add watchpoint\n");
    return 0;
} // add watchpoint

int sdb_cmd_d(char* args){
    //printf("[sdb] delete watchpoint\n");
    return 0;
} // delete watchpoint

int sdb_cmd_q(char* args){
    //printf("[sdb] quit NSIM\n");
    return -1;
} // quit NSIM

int sdb_cmd_h(char* args){
    for(int i = 0; i < SDB_NR_CMD; i = i + 1){
        printf("%s - %s\n", sdb_cmd_table[i].name, sdb_cmd_table[i].description);
    }
    return 0;
} // help

/*void sdb_set_batch_mode(){
    sdb_is_batch_mode = true;
    return;
}*/

void sdb_main_loop(){
    /*if(sdb_is_batch_mode){
        sdb_cmd_c(NULL);
        return;
    }*/

    if(monitor_script_mode){
        sdb_cmd_c(NULL);
        return;
    }

    for(char* str; (str = sdb_rl_gets()) != NULL; ){
        char* str_end = str + strlen(str);
        char* cmd = strtok(str, " ");
        if (cmd == NULL) {continue;}
        
        char* args = cmd + strlen(cmd) + 1;
        if(args >= str_end) {args = NULL;}

        device_sdl_clear_event_queue();

        int i;
        for(i = 0; i < SDB_NR_CMD; i = i + 1){
            if(strcmp(cmd, sdb_cmd_table[i].name) == 0){
                if(sdb_cmd_table[i].handler(args) < 0) {return;}
                break;
            }
        }

        if(i == SDB_NR_CMD) {printf("[sdb] unknown command '%s'\n", cmd);}
    }
    return;
}

void sdb_init_sdb(){
    // No expr and wp currently, so no function implementations
    printf("[sdb] initialized\n");
    return;
}
