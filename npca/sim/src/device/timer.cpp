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

void device_init_timer(){
    device_rtc_port_base = (uint32_t *)device_map_new_space(8);
    device_add_mmio_map("rtc-timer", DEVICE_RTC_BASE, device_rtc_port_base, 8, device_rtc_io_handler);
    //device_add_alarm_handle(device_rtc_time_interrupt);
    return;
}

void device_rtc_io_handler(uint32_t offset, int len, bool is_write){
    assert(offset == 0 || offset == 4);
    if(!is_write && offset == 4){
        uint64_t us = host_timer_get_time();
        device_rtc_port_base[0] = (uint32_t)us;
        device_rtc_port_base[1] = us >> 32;
    }
    return;
}

void device_rtc_time_interrupt(){
    // TODO
    assert(0);
    return;
} // Not implemented yet