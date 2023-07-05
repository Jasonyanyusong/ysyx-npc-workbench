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

void device_keyboard_key_enqueue(uint32_t am_scancode){
    device_keyboard_key_queue[device_keyboard_key_r] = am_scancode;
    device_keyboard_key_r = (device_keyboard_key_r + 1) % DEVICE_KEYBOARD_KEY_QUEUE_LEN;
    if(device_keyboard_key_r != device_keyboard_key_f){
        // should not reach here, however, this does not seems to be wrong, so do not assert :)
        //printf("[device-keyboard] error: key queue overflow\n");
        //assert(0);
        return;
    }
    return;
} 

uint32_t device_keyboard_key_dequeue(){
    uint32_t key = _KEY_NONE;
    if(device_keyboard_key_f != device_keyboard_key_r){
        key = device_keyboard_key_queue[device_keyboard_key_f];
        device_keyboard_key_f = (device_keyboard_key_f + 1) % DEVICE_KEYBOARD_KEY_QUEUE_LEN;
    }
    return key;
}

void device_keyboard_send_key(uint8_t scancode, bool is_keydown){
    /*if(nsim_state.state == NSIM_CONTINUE){
        printf("[device-keyboard] nsim's state is NSIM_CONTINUE\n");
    }
    if(device_keyboard_keymap[scancode] != _KEY_NONE){
        printf("[device-keyboard] device_keyboard_keymap[scancode] != _KEY_NONE\n");
    }
    printf("[device-keyboard] device_keyboard_keymap[scancode] = %d\n", device_keyboard_keymap[scancode]);*/
    if(nsim_state.state == NSIM_CONTINUE && device_keyboard_keymap[scancode] != _KEY_NONE){
        uint32_t am_scancode = device_keyboard_keymap[scancode] | (is_keydown ? DEVICE_KEYBOARD_KEYDOWN_MASK : 0);
        //printf("[device-keyboard] send key with am_scancode = %d\n", am_scancode);
        device_keyboard_key_enqueue(am_scancode);
    }
    return;
}

void device_keyboard_i8042_data_io_handler(uint32_t offset, int len, bool is_write){
    if(is_write){
        // should not reach here
        printf("[device-keyboard] error: can not handle write to keyboard MMIO\n");
        assert(0);
        return;
    }
    if(offset != 0){
        // should not reach here
        printf("[device-keyboard] error: can not process offset %d, only can process offset = 0\n", offset);
        assert(0);
        return;
    }
    device_keyboard_i8042_data_port_base[0] = device_keyboard_key_dequeue();
}

void device_keyboard_init_i8042(){
    device_keyboard_i8042_data_port_base = (uint32_t *)device_map_new_space(4);
    device_keyboard_i8042_data_port_base[0] = _KEY_NONE;
    device_add_mmio_map("keyboard", DEVICE_KEYBOARD_I8042_BASE, device_keyboard_i8042_data_port_base, 4, device_keyboard_i8042_data_io_handler);
    device_keyboard_init_keymap();
}

void device_keyboard_init_keymap(){
    //MAP(_KEYS, SDL_KEYMAP)
    device_keyboard_keymap[SDL_SCANCODE_ESCAPE] = _KEY_ESCAPE;
    device_keyboard_keymap[SDL_SCANCODE_F1] = _KEY_F1;
    device_keyboard_keymap[SDL_SCANCODE_F2] = _KEY_F2;
    device_keyboard_keymap[SDL_SCANCODE_F3] = _KEY_F3;
    device_keyboard_keymap[SDL_SCANCODE_F4] = _KEY_F4;
    device_keyboard_keymap[SDL_SCANCODE_F5] = _KEY_F5;
    device_keyboard_keymap[SDL_SCANCODE_F6] = _KEY_F6;
    device_keyboard_keymap[SDL_SCANCODE_F7] = _KEY_F7;
    device_keyboard_keymap[SDL_SCANCODE_F8] = _KEY_F8;
    device_keyboard_keymap[SDL_SCANCODE_F9] = _KEY_F9;
    device_keyboard_keymap[SDL_SCANCODE_F10] = _KEY_F10;
    device_keyboard_keymap[SDL_SCANCODE_F11] = _KEY_F11;
    device_keyboard_keymap[SDL_SCANCODE_F12] = _KEY_F12;
    
    device_keyboard_keymap[SDL_SCANCODE_GRAVE] = _KEY_GRAVE;
    device_keyboard_keymap[SDL_SCANCODE_1] = _KEY_1;
    device_keyboard_keymap[SDL_SCANCODE_2] = _KEY_2;
    device_keyboard_keymap[SDL_SCANCODE_3] = _KEY_3;
    device_keyboard_keymap[SDL_SCANCODE_4] = _KEY_4;
    device_keyboard_keymap[SDL_SCANCODE_5] = _KEY_5;
    device_keyboard_keymap[SDL_SCANCODE_6] = _KEY_6;
    device_keyboard_keymap[SDL_SCANCODE_7] = _KEY_7;
    device_keyboard_keymap[SDL_SCANCODE_8] = _KEY_8;
    device_keyboard_keymap[SDL_SCANCODE_9] = _KEY_9;
    device_keyboard_keymap[SDL_SCANCODE_0] = _KEY_0;
    device_keyboard_keymap[SDL_SCANCODE_MINUS] = _KEY_MINUS;
    device_keyboard_keymap[SDL_SCANCODE_EQUALS] = _KEY_EQUALS;
    device_keyboard_keymap[SDL_SCANCODE_BACKSPACE] = _KEY_BACKSPACE;
    
    device_keyboard_keymap[SDL_SCANCODE_TAB] = _KEY_TAB;
    device_keyboard_keymap[SDL_SCANCODE_Q] = _KEY_Q;
    device_keyboard_keymap[SDL_SCANCODE_W] = _KEY_W;
    device_keyboard_keymap[SDL_SCANCODE_E] = _KEY_E;
    device_keyboard_keymap[SDL_SCANCODE_R] = _KEY_R;
    device_keyboard_keymap[SDL_SCANCODE_T] = _KEY_T;
    device_keyboard_keymap[SDL_SCANCODE_Y] = _KEY_Y;
    device_keyboard_keymap[SDL_SCANCODE_U] = _KEY_U;
    device_keyboard_keymap[SDL_SCANCODE_I] = _KEY_I;
    device_keyboard_keymap[SDL_SCANCODE_O] = _KEY_O;
    device_keyboard_keymap[SDL_SCANCODE_P] = _KEY_P;
    device_keyboard_keymap[SDL_SCANCODE_LEFTBRACKET] = _KEY_LEFTBRACKET;
    device_keyboard_keymap[SDL_SCANCODE_RIGHTBRACKET] = _KEY_RIGHTBRACKET;
    device_keyboard_keymap[SDL_SCANCODE_BACKSLASH] = _KEY_BACKSLASH;
    
    device_keyboard_keymap[SDL_SCANCODE_CAPSLOCK] = _KEY_CAPSLOCK;
    device_keyboard_keymap[SDL_SCANCODE_A] = _KEY_A;
    device_keyboard_keymap[SDL_SCANCODE_S] = _KEY_S;
    device_keyboard_keymap[SDL_SCANCODE_D] = _KEY_D;
    device_keyboard_keymap[SDL_SCANCODE_F] = _KEY_F;
    device_keyboard_keymap[SDL_SCANCODE_G] = _KEY_G;
    device_keyboard_keymap[SDL_SCANCODE_H] = _KEY_H;
    device_keyboard_keymap[SDL_SCANCODE_J] = _KEY_J;
    device_keyboard_keymap[SDL_SCANCODE_K] = _KEY_K;
    device_keyboard_keymap[SDL_SCANCODE_L] = _KEY_L;
    device_keyboard_keymap[SDL_SCANCODE_SEMICOLON] = _KEY_SEMICOLON;
    device_keyboard_keymap[SDL_SCANCODE_APOSTROPHE] = _KEY_APOSTROPHE;
    device_keyboard_keymap[SDL_SCANCODE_RETURN] = _KEY_RETURN;

    device_keyboard_keymap[SDL_SCANCODE_LSHIFT] = _KEY_LSHIFT;
    device_keyboard_keymap[SDL_SCANCODE_Z] = _KEY_Z;
    device_keyboard_keymap[SDL_SCANCODE_X] = _KEY_X;
    device_keyboard_keymap[SDL_SCANCODE_C] = _KEY_C;
    device_keyboard_keymap[SDL_SCANCODE_V] = _KEY_V;
    device_keyboard_keymap[SDL_SCANCODE_B] = _KEY_B;
    device_keyboard_keymap[SDL_SCANCODE_N] = _KEY_N;
    device_keyboard_keymap[SDL_SCANCODE_M] = _KEY_M;
    device_keyboard_keymap[SDL_SCANCODE_COMMA] = _KEY_COMMA;
    device_keyboard_keymap[SDL_SCANCODE_PERIOD] = _KEY_PERIOD;
    device_keyboard_keymap[SDL_SCANCODE_SLASH] = _KEY_SLASH;
    device_keyboard_keymap[SDL_SCANCODE_RSHIFT] = _KEY_RSHIFT;
    
    device_keyboard_keymap[SDL_SCANCODE_LCTRL] = _KEY_LCTRL;
    device_keyboard_keymap[SDL_SCANCODE_APPLICATION] = _KEY_APPLICATION;
    device_keyboard_keymap[SDL_SCANCODE_LALT] = _KEY_LALT;
    device_keyboard_keymap[SDL_SCANCODE_SPACE] = _KEY_SPACE;
    device_keyboard_keymap[SDL_SCANCODE_RALT] = _KEY_RALT;
    device_keyboard_keymap[SDL_SCANCODE_RCTRL] = _KEY_RCTRL;
    
    device_keyboard_keymap[SDL_SCANCODE_UP] = _KEY_UP;
    device_keyboard_keymap[SDL_SCANCODE_DOWN] = _KEY_DOWN;
    device_keyboard_keymap[SDL_SCANCODE_LEFT] = _KEY_LEFT;
    device_keyboard_keymap[SDL_SCANCODE_RIGHT] = _KEY_RIGHT;
    device_keyboard_keymap[SDL_SCANCODE_INSERT] = _KEY_INSERT;
    device_keyboard_keymap[SDL_SCANCODE_DELETE] = _KEY_DELETE;
    device_keyboard_keymap[SDL_SCANCODE_HOME] = _KEY_HOME;
    device_keyboard_keymap[SDL_SCANCODE_END] = _KEY_END;
    device_keyboard_keymap[SDL_SCANCODE_PAGEUP] = _KEY_PAGEUP;
    device_keyboard_keymap[SDL_SCANCODE_PAGEDOWN] = _KEY_PAGEDOWN;
    return;
}