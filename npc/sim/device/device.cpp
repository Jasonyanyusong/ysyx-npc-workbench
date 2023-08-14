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

#include <device.h>
#include <SDL2/SDL.h>

void send_key(uint8_t scancode, bool is_keydown);
void vga_update_screen();

word_t device_last = 0;

void sdl_clear_event_queue(){
    SDL_Event event;
    while (SDL_PollEvent(&event));
    return;
}

// TODO: move macro "device_have_vga" and "device_have_keyboard" to "device.h"

void device_update(){
    word_t device_now = get_time();
    if(device_now - device_last < (1000000 / 60)){
        return;
    }
    device_last = device_now;

    if(device_have_vga){
        vga_update_screen();
    }
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:{
                npc_state.state = NPC_STOP;
                break;
            }
            case SDL_KEYDOWN:
            case SDL_KEYUP:{
                if(device_have_keyboard){
                    //printf("[device-keyboard] SDL event catch key with scancode %d, have event \"%s\"\n", event.key.keysym.scancode, event.key.type == SDL_KEYDOWN ? "KEY DOWN" : "KEY UP");
                    uint8_t k = event.key.keysym.scancode;
                    bool is_keydown = (event.key.type == SDL_KEYDOWN);
                    send_key(k, is_keydown);
                    break;
                }
                else{
                    break;
                }
                break;
            }
            default: break;
        }
    }
    return;
}

void init_device(){
    // TODO: implement this function
    assert(0);
    return;
}