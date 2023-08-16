/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing 101 High School
* Copyright (c) 2023 Yusong Yan, University of Washington - Seattle
*
* YSYX-NPC-SIM is licensed under Mulan PSL v2.
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

#include <common.h>
#include <mem.h>

static uint8_t *pmem = NULL;

void init_mem(){
    printf("Initializing Memory\n");  
    pmem = (uint8_t *)malloc(MEM_END - MEM_START + 1);
    assert(pmem);
    printf("Memory [0x%x, 0x%x]\n", MEM_START, MEM_END);
    return;
}

bool in_pmem(word_t addr){
    return (addr >= MEM_START) && (addr <= MEM_END);
}

uint8_t* guest_to_host(word_t paddr) { return pmem + paddr - MEM_START; }
word_t host_to_guest(uint8_t *haddr) { return haddr - pmem + MEM_START; }

void out_of_bound(word_t addr){
    printf("[memory] address = 0x%x is out of bound of pmem@[0x%x,0x%x]\n", addr, MEM_START, MEM_END);
    assert(0);
}

word_t pmem_read(word_t addr, int len) {
  word_t ret = host_read(guest_to_host(addr), len);
  //printf("[mem : pmem_read] read mem @ 0x%lx with len %d get data 0x%lx\n", addr, len, ret);
  return ret;
}

void pmem_write(word_t addr, int len, word_t data) {
  //printf("[mem : pmem_write] write mem @ 0x%lx with len %d and data 0x%lx\n", addr, len, data);
  host_write(guest_to_host(addr), len, data);
}