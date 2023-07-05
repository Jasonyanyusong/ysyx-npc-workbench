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

static inline uint64_t mem_host_read(uint8_t *mem_addr, int mem_length);
static void mem_host_write(void *mem_addr, int mem_length, uint64_t mem_data);
bool mem_addr_in_bound(uint32_t mem_addr);

uint64_t mem_vaddr_ifetch(uint64_t mem_addr, int mem_length);
uint64_t mem_vaddr_read(uint64_t mem_addr, int mem_length);
void mem_vaddr_write(uint64_t mem_addr, int mem_length, uint64_t mem_data);

uint8_t* mem_guest_to_host(uint64_t paddr);
uint64_t mem_host_to_guest(uint8_t *haddr);
uint8_t* mem_guest_to_host(uint64_t mem_physical_addr);
uint64_t mem_host_to_guest(uint8_t *mem_host_addr);
uint64_t mem_pmem_read(uint64_t mem_addr, int mem_length);
void mem_pmem_write(uint64_t mem_addr, int mem_length, uint64_t mem_data);
uint64_t mem_paddr_read(uint64_t mem_addr, int mem_length);
void mem_paddr_write(uint64_t mem_addr, int mem_length, uint64_t mem_data);

void mem_init_mem();