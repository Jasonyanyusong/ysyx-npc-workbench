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

void device_serial_io_handler(uint32_t offset, int len, bool is_write){
    assert(len == 1);
    switch(offset){
        case DEVICE_SERIO_CH_OFFSET:
            if (is_write == true) {putc((char)device_serial_base[0], stderr);}
            else {printf("[decive-serio] error: serio do not support read\n"); assert(0);}
            break;
        default: {printf("[device-serio] error: do not support offset = %d\n", offset); assert(0); break;}
    }
}

void device_init_serial(){
    device_serial_base = device_map_new_space(8);
    device_add_mmio_map("serial", DEVICE_SERIAL_BASE, device_serial_base, 8, device_serial_io_handler);
    return;
}
