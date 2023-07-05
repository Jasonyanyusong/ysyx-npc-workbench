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

#define mem_start_addr  0x080000000
#define mem_end_addr    0x08fffffff
#define mem_size        mem_end_addr - mem_start_addr + 1

#define print_debug_informations false
#define generate_dump_wave_file  true

#define difftest_enable true

#define device_have_serial   true
#define device_have_rtc      true
#define device_have_keyboard true
#define device_have_vga      true
#define device_have_audio    false
#define device_have_disk     false
#define device_have_sdcard   false

//#define trace_enable_itrace 
//#define trace_enable_mtrace 
//#define trace_enable_rtrace 
//#define trace_enable_dtrace 
//#define trace_enable_mtrace 
//#define trace_enable_etrace 

#define DEVICE_VGA_SHOW_SCREEN true