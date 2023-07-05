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

uint64_t host_timer_boot_time = 0;
uint64_t host_timer_get_time_internal();
uint64_t host_timer_get_time();

uint64_t host_timer_get_time_internal(){
    struct timeval host_timer_now;
    gettimeofday(&host_timer_now, NULL);
    uint64_t host_timer_us = host_timer_now.tv_sec * 1000000 + host_timer_now.tv_usec;
    return host_timer_us;
}

uint64_t host_timer_get_time(){
    if(host_timer_boot_time == 0) {host_timer_boot_time = host_timer_get_time_internal();}
    uint64_t now = host_timer_get_time_internal();
    return now - host_timer_boot_time;
}