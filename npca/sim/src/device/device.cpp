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

#include<device.h>

void device_init_map(){
    device_map_io_space = (uint8_t *)malloc(DEVICE_MAP_IO_SPACE_MAX);
    assert(device_map_io_space);
    device_map_p_space = device_map_io_space;
    return;
}

void device_init_devices(){
    device_init_map();
    if(device_have_serial)   {device_init_serial();}
    if(device_have_rtc)      {device_init_timer();}
    if(device_have_keyboard) {device_keyboard_init_i8042();}
    if(device_have_vga)      {device_vga_init_vga();}
    return;
}

void device_update(){
    uint64_t device_now = host_timer_get_time();
    if(device_now - device_last < (1000000 / 60)){
        return;
    }
    device_last = device_now;

    if(device_have_vga){
        device_vga_vga_update_screen();
    }
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:{
                state_set_state(NSIM_STOP);
                break;
            }
            case SDL_KEYDOWN:
            case SDL_KEYUP:{
                if(device_have_keyboard){
                    //printf("[device-keyboard] SDL event catch key with scancode %d, have event \"%s\"\n", event.key.keysym.scancode, event.key.type == SDL_KEYDOWN ? "KEY DOWN" : "KEY UP");
                    uint8_t k = event.key.keysym.scancode;
                    bool is_keydown = (event.key.type == SDL_KEYDOWN);
                    device_keyboard_send_key(k, is_keydown);
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

void device_sdl_clear_event_queue(){
    SDL_Event event;
    while (SDL_PollEvent(&event));
}