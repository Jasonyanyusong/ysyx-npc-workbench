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
#include <verilator-sim.h>

void init_i8042();
void init_serial();
void init_timer();
void init_vga();

void init_map();

void send_key(uint8_t scancode, bool is_keydown);
void vga_update_screen();

word_t device_last = 0;

void sdl_clear_event_queue(){
    SDL_Event event;
    while (SDL_PollEvent(&event));
    return;
}

void device_update(){
    word_t device_now = get_time();
    if(device_now - device_last < (1000000 / 60)){
        return;
    }
    device_last = device_now;

    #ifdef CONFIG_VGA_CTL_MMIO
    vga_update_screen();
    #endif
    
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:{
                npc_state.state = NPC_STOP;
                break;
            }
            case SDL_KEYDOWN:
            case SDL_KEYUP:{

                #ifdef CONFIG_I8042_DATA_MMIO
                //printf("[device-keyboard] SDL event catch key with scancode %d, have event \"%s\"\n", event.key.keysym.scancode, event.key.type == SDL_KEYDOWN ? "KEY DOWN" : "KEY UP");
                uint8_t k = event.key.keysym.scancode;
                bool is_keydown = (event.key.type == SDL_KEYDOWN);
                send_key(k, is_keydown);
                break;
                #else
                break;
                #endif

            }
            default: break;
        }
    }
    return;
}

void init_device(){
    printf("[device] initializing devices\n");
    
    init_map();

    #ifdef CONFIG_I8042_DATA_MMIO
    init_i8042();
    #endif

    #ifdef CONFIG_SERIAL_MMIO
    init_serial();
    #endif

    #ifdef CONFIG_RTC_MMIO
    init_timer();
    #endif

    #ifdef CONFIG_VGA_CTL_MMIO
    init_vga();
    #endif

    printf("[device] device initialize finished\n");
    
    return;
}