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

void trace_mtrace_init(){
    printf("[trace-mtrace] initializing\n");
    if(remove("mtrace.txt") == 0){
        printf("[trace-mtrace] previous mtrace record deleted\n");
    }
    return;
}

void trace_mtrace_write(uint64_t mtrace_addr, bool mtrace_is_write, uint64_t mtrace_data, int mtrace_len){
    FILE *trace_mtrace_file = fopen("mtrace.txt", "a+");
    if(trace_mtrace_file == NULL){
        // should not reach here!
        printf("[trace-mtrace] error: mtrace file can not be opened\n");
        assert(0);
        return;
    }
    char mtrace_to_be_written[128];
    if(mtrace_is_write == true){
        sprintf(mtrace_to_be_written, "[mtrace] pc: 0x%lx, mem_W, addr = 0x%lx, len = %d, data = 0x%lx\n", trace_pc, mtrace_addr, mtrace_len, mtrace_data);
    }else{
        sprintf(mtrace_to_be_written, "[mtrace] pc: 0x%lx, mem_R, addr = 0x%lx, len = %d, data = 0x%lx\n", trace_pc, mtrace_addr, mtrace_len, mtrace_data);
    }
    fputs(mtrace_to_be_written, trace_mtrace_file);
    fclose(trace_mtrace_file);
    return;
}