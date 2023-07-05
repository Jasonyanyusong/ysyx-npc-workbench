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

uint64_t statistics_nr_exec = 0;
uint64_t statistics_time_exec = 0;

void statistics_show();

void statistics_show(){
    printf("[statistics] total insturction executed by NSIM: %ld instructions\n", statistics_nr_exec);
    printf("[statistics] total time cost for execution instructions: %ld us\n", statistics_time_exec);
    if(statistics_time_exec == 0){
        printf("[statistics] time is so low, can not show statistics\n");
    }else{
        printf("[statistics] NSIM simulation frequency: %ld inst/second\n", (statistics_nr_exec * 1000000 / statistics_time_exec));
    }
    return;
}