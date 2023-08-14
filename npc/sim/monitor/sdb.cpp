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
#include <reg.h>
#include <mem.h>
#include <verilator-sim.h>

bool is_batch_mode = false;

static struct {
    const char *name;
    const char *description;
    int (*handler) (char *);
} sdb_cmd_table [] = {
    { "h", "help",                                     cmd_h},
    { "c", "continue execution",                       cmd_c},
    { "q", "quit NSIM",                                cmd_q},
    { "s", "single-step execution",                    cmd_s},
    { "r", "informations about register ",             cmd_r},
    { "m", "scan and print memory",                    cmd_m},
};

#define NR_CMD 6

static char* rl_gets(){
    static char* line_read = NULL;

    if(line_read){
        free(line_read);
        line_read = NULL;
    }

    line_read = readline("(npc-sim) ");

    if (line_read && *line_read){
        add_history(line_read);
    }

    return line_read;
}

int cmd_c(char* args){
    while(npc_state.state == NPC_RUNNING || npc_state.state == NPC_STOP){
        sim_one_cycle();
    }
    printf("[sdb] NPC's state is not NPC_RUNNING or NPC_STOP, can not continue\n");
    return 0;
}

int cmd_q(char* args){
    assert(0);
    // TODO: implement this function
    return 0;
}

int cmd_m(char* args){
    if(args == NULL){
        printf("[sdb] this function need arguments, plese try again\n");
        return 0;
    }
    assert(0);
    // TODO: implement this function
    return 0;
}

int cmd_r(char* args){
    assert(0);
    // TODO: implement this function
    return 0;
}

int cmd_s(char* args){
    if(args == NULL){
        printf("[sdb] you can use \"s N\" to execute N steps\n");
        if(npc_state.state == NPC_RUNNING || npc_state.state == NPC_STOP){
            sim_one_cycle();
        }
        return 0;
    }
    // TODO: implement this function
    return 0;
}

int cmd_h(char* args){
    assert(0);
    // TODO: implement this function
    return 0;
}

void sdb_main_loop(){
    if(is_batch_mode){
        cmd_c(NULL);
        return;
    }

    for(char* str; (str = rl_gets()) != NULL; ){
        char* str_end = str + strlen(str);
        char* cmd = strtok(str, " ");
        if (cmd == NULL) {continue;}
        
        char* args = cmd + strlen(cmd) + 1;
        if(args >= str_end) {args = NULL;}

        //device_sdl_clear_event_queue();
        // TODO: the code above need to be implemented later

        int i;
        for(i = 0; i < NR_CMD; i = i + 1){
            if(strcmp(cmd, sdb_cmd_table[i].name) == 0){
                if(sdb_cmd_table[i].handler(args) < 0) {return;}
                break;
            }
        }

        if(i == NR_CMD) {printf("[sdb] unknown command '%s'\n", cmd);}
    }
    
    return;
}

void sdb_init_sdb(){
    printf("[sdb] nothing to do, initialized\n");
    return;
}

void sdb_set_batch_mode(){
    is_batch_mode = true;
    assert(is_batch_mode);
    return;
}