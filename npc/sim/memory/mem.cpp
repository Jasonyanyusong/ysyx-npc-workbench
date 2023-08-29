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
#include <device.h>

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
  if(in_pmem(addr)){
    // this is inside pmem, just call guest_to_host and host_read
    word_t pmem_ret = host_read(guest_to_host(addr), len);
    return pmem_ret;
  }else{
    // this is outside pmem, call device mmio read
    word_t mmio_ret = mmio_read(addr, len);
    return mmio_ret;
  }

  // should not reach here
  printf("[memory] should not reach here, check implementation!\n");
  assert(0);
  return -1;
}

void pmem_write(word_t addr, int len, word_t data) {
  if(in_pmem(addr)){
    // this is inside pmem, just call guest_to_host and host_write
    host_write(guest_to_host(addr), len, data);
    return;
  }else{
    // this is outside pmem, call device mmio read
    mmio_write(addr, len, data);
    return;
  }
  
  // should not reach here
  printf("[memory] should not reach here, check implementation!\n");
  assert(0);
  return;
}