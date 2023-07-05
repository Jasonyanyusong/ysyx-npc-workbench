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

void trace_itrace_init(){
    printf("[trace-itrace] initializing\n");
    if(remove("itrace.txt") == 0){
        printf("[trace-itrace] previous itrace record deleted\n");
    }
    return;
}

void trace_itrace_write(uint32_t itrace_inst){
    FILE *trace_itrace_file = fopen("itrace.txt", "a+");
    if(trace_itrace_file == NULL){
        // should not reach here!
        printf("[trace-itrace] error: itrace file can not be opened\n");
        assert(0);
        return;
    }
    char itrace_to_be_written[128];
    sprintf(itrace_to_be_written, "[itrace] pc: 0x%lx, inst: 0x%x\n", trace_pc, itrace_inst);
    fputs(itrace_to_be_written, trace_itrace_file);
    fclose(trace_itrace_file);
    return;
}