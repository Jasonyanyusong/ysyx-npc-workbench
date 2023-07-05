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

uint32_t device_vga_screen_width(){
    return DEVICE_VGA_SCREEN_W;
}

uint32_t device_vga_screen_height(){
    return DEVICE_VGA_SCREEN_H;
}

uint32_t device_vga_screen_size(){
    return device_vga_screen_width() * device_vga_screen_height() * sizeof(uint32_t);
}

void device_vga_init_screen(){
    SDL_Window *device_vga_window = NULL;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(DEVICE_VGA_SCREEN_W * 2, DEVICE_VGA_SCREEN_H * 2, 0, &device_vga_window, &device_vga_renderer);
    SDL_SetWindowTitle(device_vga_window, "YSYX NPC (Advanced Level) Simulator By Jasonyanyusong");
    device_vga_texture = SDL_CreateTexture(device_vga_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, DEVICE_VGA_SCREEN_W, DEVICE_VGA_SCREEN_H);
    return;
}

void device_vga_update_screen(){
    //if(true) {printf("[device-vga] calling SDL2 functions to update screen\n");}
    SDL_UpdateTexture(device_vga_texture, NULL, device_vga_vmem, DEVICE_VGA_SCREEN_W * sizeof(uint32_t));
    SDL_RenderClear(device_vga_renderer);
    SDL_RenderCopy(device_vga_renderer, device_vga_texture, NULL, NULL);
    SDL_RenderPresent(device_vga_renderer);
}

void device_vga_vga_update_screen(){
    //if(print_debug_informations) {printf("[device-vga] vga sync register hold val = 0x%x\n", device_vga_ctl_port_base[1]);}
    if(device_vga_ctl_port_base[1] != 0){
        //if(true) {printf("[device-vga] ready to update screen\n");}
        if(DEVICE_VGA_SHOW_SCREEN) {device_vga_update_screen();}
        device_vga_ctl_port_base[1] = 0;
        return;
    }
    return;
}

void device_vga_init_vga(){
    device_vga_ctl_port_base = (uint32_t *)device_map_new_space(8);
    device_vga_ctl_port_base[0] = (device_vga_screen_width() << 16) | device_vga_screen_height();
    device_add_mmio_map("vgactl", DEVICE_VGA_CTRL_BASE, device_vga_ctl_port_base, 8, NULL);
    device_vga_vmem = device_map_new_space(device_vga_screen_size());
    device_add_mmio_map("vmem", DEVICE_VGA_FB_ADDR, device_vga_vmem, device_vga_screen_size(), NULL);
    if(DEVICE_VGA_SHOW_SCREEN){
        device_vga_init_screen();
        memset(device_vga_vmem, 0, device_vga_screen_size());
    }
}