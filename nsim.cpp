/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing No. 101 High School
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

//========== Include Headers ==========

#include "verilated.h"
#include "verilated_vcd_c.h"
#include "obj_dir/Vnpc.h"

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>

//========== List functions that will be used later ==========

uint64_t mem_host_read(void *mem_addr, int mem_length);

//========== Memory manipulations ==========
uint64_t mem_host_read(void *mem_addr, int mem_length){
    switch (mem_length){
        case 1: return *(uint8_t  *)mem_addr;
        case 2: return *(uint16_t *)mem_addr;
        case 4: return *(uint32_t *)mem_addr;
        case 8: return *(uint64_t *)mem_addr;
        default: printf("[memory] invalid read length\n"); assert(0); return 0;
    }
}