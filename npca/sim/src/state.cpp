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

#include<state.h>

bool state_check_can_continue(){
    switch(nsim_state.state){
        case NSIM_CONTINUE: /*printf("[state] state is NSIM_CONTINUE, can continue\n");*/ return true;  break;
        case NSIM_STOP:     /*printf("[state] state is NSIM_STOP, can continue\n");*/     return true;  break;
        case NSIM_ABORT:    /*printf("[state] state is NSIM_ABORT, can't continue\n");*/  return false; break;
        case NSIM_END:      /*printf("[state] state is NSIM_END, can't continue\n");*/    return false; break;
        case NSIM_HALT:     /*printf("[state] state is NSIM_HALT, can't continue\n");*/   return false; break;
        case NSIM_QUIT:     /*printf("[state] state is NSIM_QUIT, can't continue\n");*/   return false; break;
        default:            printf("[state] unknown state, error\n"); assert(0);      return false; break;
    }
    return false;
}

void state_get_state(){
    state_check_can_continue();
}

void state_set_state(int state_get_state){
    bool state_continue               = state_get_state == NSIM_CONTINUE || state_get_state == NSIM_STOP;
    bool state_normal_cant_continue   = state_get_state == NSIM_END      || state_get_state == NSIM_QUIT;
    bool state_abnormal_cant_continue = state_get_state == NSIM_ABORT    || state_get_state == NSIM_HALT;
    assert(state_continue || (state_normal_cant_continue || state_abnormal_cant_continue));
    //if(state_continue)                {printf("[state] state update to a can continue state\n");}
    //if(state_normal_cant_continue)    {printf("[state] state update to a normal can't continue state\n");}
    //if(state_abnormal_cant_continue)  {printf("[state] state update to an abnormal can't continue state\n");}
    nsim_state.state = state_get_state;
}