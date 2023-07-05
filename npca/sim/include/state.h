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

enum {NSIM_CONTINUE = 11, NSIM_STOP = 12, NSIM_ABORT = 13, NSIM_END = 14, NSIM_HALT = 15, NSIM_QUIT = 16};

/*
Informations about NSIM sate:
NSIM_CONTINUE indicates that NSIM will continue normally
NSIM_STOP indicates that watchpoint(s)'s value changed, so the simulation will be paused, which can be manually continued
NSIM_ABORT indicates that NPC failed DiffTest, so we stop the simualation, which can not be manually continued
NSIM_END indicates that NPC's program execution have finished, so we stop the simulation, which can be restarted by quit NSIM and run again
NSIM_HALT indicates that NPC have raised an error, so we stop the simulation, which can not be manually continued
NSIM_QUIT indicates that user quit SIM using SDB Q command
*/

typedef struct{
    int state;
    uint64_t halt_pc;
    uint32_t halt_ret;
} NSIMState;

NSIMState nsim_state;

bool state_check_can_continue();
void state_set_state(int state_get_state);
void state_show_state();