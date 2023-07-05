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

uint64_t trace_pc = 0;

void trace_init_trace();

void trace_itrace_init();
void trace_itrace_write(uint32_t itrace_inst);

void trace_mtrace_init();
void trace_mtrace_write(uint64_t mtrace_addr, bool mtrace_is_write, uint64_t mtrace_data, int mtrace_len);

typedef struct{
    uint64_t gpr[32];
    uint64_t pc;
} trace_rtrace_context;
void trace_rtrace_init();
void trace_rtrace_write(trace_rtrace_context rtrace_context);

void trace_dtrace_init();
void trace_dtrace_write(char* dtrace_device_name, uint64_t dtrace_addr, int dtrace_len, bool dtrace_is_write, uint64_t dtrace_data);

void trace_init_trace(){
    printf("[trace] initializing tracer(s)\n");
    trace_itrace_init();
    trace_mtrace_init();
    trace_rtrace_init();
    trace_dtrace_init();
    return;
}