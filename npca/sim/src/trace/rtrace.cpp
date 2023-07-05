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

#include<trace.h>

void trace_rtrace_init(){
    printf("[trace-rtrace] initializing\n");
    if(remove("rtrace.txt") == 0){
        printf("[trace-rtrace] previous rtrace record deleted\n");
    }
    return;
}

void trace_rtrace_write(trace_rtrace_context rtrace_context){
    FILE *trace_rtrace_file = fopen("rtrace.txt", "a+");
    if(trace_rtrace_file == NULL){
        // should not reach here!
        printf("[trace-rtrace] error: rtrace file can not be opened\n");
        assert(0);
        return;
    }
    char rtrace_to_be_written[1024];
    sprintf(rtrace_to_be_written, "[trace-rtrace] pc: 0x%lx, ", trace_pc);
    for(int i = 0; i < 32; i = i + 1){
        char rtrace_single_gpr[64];
        sprintf(rtrace_single_gpr, "x%2d: 0x%16lx, ", i, rtrace_context.gpr[i]);
        strcat(rtrace_to_be_written, rtrace_single_gpr);
    }
    char rtrace_pc_reg[64];
    sprintf(rtrace_pc_reg,  "pc: 0x%16lx\n\0", trace_pc);
    strcat(rtrace_to_be_written, rtrace_pc_reg);
    fputs(rtrace_to_be_written, trace_rtrace_file);
    fclose(trace_rtrace_file);
    return;
}