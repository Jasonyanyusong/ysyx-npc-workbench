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

#include<memory.h>

static uint8_t *mem_pmem = NULL;

// static uint8_t mem_pmem[mem_size] __attribute((aligned(4096))) = {};

static inline uint64_t mem_host_read(uint8_t *mem_addr, int mem_length){
    //printf("[memory] in host read, address is %p, length is %d\n", mem_addr, mem_length);
    switch (mem_length){
        case 1: return *(uint8_t  *)mem_addr;
        case 2: return *(uint16_t *)mem_addr;
        case 4: return *(uint32_t *)mem_addr;
        case 8: return *(uint64_t *)mem_addr;
        default: printf("[memory] invalid read length\n"); assert(0); return 0;
    }
}

static void mem_host_write(void *mem_addr, int mem_length, uint64_t mem_data){
    switch (mem_length){
        case 1: *(uint8_t  *)mem_addr = mem_data; return;
        case 2: *(uint16_t *)mem_addr = mem_data; return;
        case 4: *(uint32_t *)mem_addr = mem_data; return;
        case 8: *(uint64_t *)mem_addr = mem_data; return;
        default: printf("[memory] invalid write length\n"); assert(0); return;
    }
}

bool mem_addr_in_bound(uint32_t mem_addr){
    // For a 64-bit system, the mem address space should be uint64_t, however, if we use uint64_t here, when we scan memory, there will be segmentation error, so we set to uint32_t first
    // but need later refinements!
    if(mem_addr - mem_start_addr > mem_size || mem_addr < mem_start_addr)
        {/*printf("[memory] address 0x%x out of bound [0x%x,0x%x]\n", mem_addr, mem_start_addr, mem_end_addr);*/ /*assert(0);*/ return false; }
    return true;
}

void mem_init_mem(){
    mem_pmem = (uint8_t *)malloc(mem_size);
    //printf("[memory] host memory starts at %p\n", mem_pmem);
    assert(mem_pmem);
    printf("[memory] physical memory area [0x%x,0x%x] size 0x%x\n", mem_start_addr, mem_end_addr, mem_size);
}

uint8_t* mem_guest_to_host(uint64_t paddr) { uint64_t temp = paddr - mem_start_addr; /*printf("[memory] temp is 0x%0x mem_pmem is %p, guest to host addr %p, paddr 0x%x, memstart addr 0x%x\n", temp, mem_pmem, mem_pmem + paddr, paddr, mem_start_addr);*/ return mem_pmem + (paddr - mem_start_addr); }
uint64_t mem_host_to_guest(uint8_t *haddr) { return haddr - mem_pmem + mem_start_addr; }

uint64_t mem_pmem_read(uint64_t mem_addr, int mem_length){
    //printf("[memory] mem_pmem_read: mem_guest_to_host(mem_addr) = %p\n", mem_guest_to_host(mem_addr));

    //printf("addr = 0x%lx\n", mem_addr);

    //if(device_have_rtc && mem_addr == DEVICE_RTC_ADDR_LO){/*printf("[device] access rtc_lo\n");*/ return device_timer_write_time_to_sim(0);}
    //if(device_have_rtc && mem_addr == DEVICE_RTC_ADDR_HI){/*printf("[device] access rtc_hi\n");*/ return device_timer_write_time_to_sim(1);}

    if(mem_addr_in_bound(mem_addr)){
        uint64_t ret = mem_host_read(mem_guest_to_host(mem_addr), mem_length);

        #ifdef trace_enable_mtrace
        trace_mtrace_write(mem_addr, false, ret, mem_length);
        #endif

        return ret;
    }else{
        // Address is not Physical Memory, implement device
        //printf("addr = 0x%lx\n", mem_addr);
        uint64_t ret = device_mmio_read(mem_addr, mem_length);

        #ifdef trace_enable_dtrace 
        trace_dtrace_write((char *)device_mmio_fetch_mmio_map(mem_addr) -> name, mem_addr, mem_length, false, ret);
        #endif

        return ret;
    }
}
void mem_pmem_write(uint64_t mem_addr, int mem_length, uint64_t mem_data){

    //if(device_have_serial && mem_addr == DEVICE_SERIAL_ADDR){device_serial_putchar(mem_data); return;}

    if(mem_addr_in_bound(mem_addr)){
        //printf("[memory] normal write, addr = 0x%lx, len = %d, data = 0x%lx\n", mem_addr, mem_length, mem_data);
        mem_host_write(mem_guest_to_host(mem_addr), mem_length, mem_data);

        #ifdef trace_enable_mtrace 
        trace_mtrace_write(mem_addr, true, mem_data, mem_length);
        #endif

        return;
    }else{
        //printf("[memory] device write, addr = 0x%lx, len = %d, data = 0x%lx\n", mem_addr, mem_length, mem_data);
        device_mmio_write(mem_addr, mem_length, mem_data);

        #ifdef trace_enable_dtrace 
        trace_dtrace_write((char *)device_mmio_fetch_mmio_map(mem_addr) -> name, mem_addr, mem_length, false, mem_data);
        #endif

        return;
    }

    // should not reach here!
    printf("[mem-pmem-write] error: address not in physical memory bound and failed with device mem write\n");
    assert(0);
    return;
    
}

uint64_t mem_paddr_read(uint64_t mem_addr, int mem_length){
    //printf("[memory] mem_paddr_read: mem_addr = 0x%x\n", mem_addr);
    if(mem_addr_in_bound(mem_addr))
        return mem_pmem_read(mem_addr, mem_length);
    // should not reach here!
    assert(0);
    return 0;
}

void mem_paddr_write(uint64_t mem_addr, int mem_length, uint64_t mem_data){
    if(mem_addr_in_bound(mem_addr))
        mem_pmem_write(mem_addr, mem_length, mem_data);
    // should not reach here!
    assert(0);
    return;
}

uint64_t mem_vaddr_ifetch(uint64_t mem_addr, int mem_length){
    return mem_paddr_read(mem_addr, mem_length);
}

uint64_t mem_vaddr_read(uint64_t mem_addr, int mem_length){
    return mem_paddr_read(mem_addr, mem_length);
}

void mem_vaddr_write(uint64_t mem_addr, int mem_length, uint64_t mem_data){
    mem_paddr_write(mem_addr, mem_length, mem_data);
}