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

void trace_dtrace_init(){
    printf("[trace-dtrace] initializing\n");
    if(remove("dtrace.txt") == 0){
        printf("[trace-dtrace] previous dtrace record deleted\n");
    }
    return;
}

void trace_dtrace_write(char* dtrace_device_name, uint64_t dtrace_addr, int dtrace_len, bool dtrace_is_write, uint64_t dtrace_data){
    FILE *trace_dtrace_file = fopen("dtrace.txt", "a+");
    if(trace_dtrace_file == NULL){
        // should not reach here!
        printf("[trace-dtrace] error: dtrace file can not be opened\n");
        assert(0);
        return;
    }
    char mtrace_to_be_written[128];
    if(dtrace_is_write == true){
        sprintf(mtrace_to_be_written, "[dtrace] pc: 0x%lx, mmio_W, name = \"%s\", addr = 0x%lx, len = %d, data = 0x%lx\n", trace_pc, dtrace_device_name, dtrace_addr, dtrace_len, dtrace_data);
    }else{
        sprintf(mtrace_to_be_written, "[dtrace] pc: 0x%lx, mmio_R, name = \"%s\", addr = 0x%lx, len = %d, data = 0x%lx\n", trace_pc, dtrace_device_name, dtrace_addr, dtrace_len, dtrace_data);
    }
    fputs(mtrace_to_be_written, trace_dtrace_file);
    fclose(trace_dtrace_file);
    return;
}