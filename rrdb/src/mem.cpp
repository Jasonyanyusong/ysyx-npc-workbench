#include "rrdb.h"
#include <malloc.h>
static uint8_t *pmem = NULL;

void mem_init(){
    pmem = (uint8_t *)malloc(mem_size);
    assert(pmem);
    printf("Physical Memory area is [0x%x,0x%x], size is 0x%x\n", mem_start, mem_end, mem_size);
    return;
}

uint8_t* guest_to_host(uint64_t paddr)  {return pmem + paddr - mem_start;}
uint64_t host_to_guest(uint8_t *haddr)  {return haddr - pmem + mem_start;}
bool     mem_in_bound(uint64_t addr)    {return addr - mem_start < mem_size;}

static uint64_t pmem_read(uint64_t addr, int len)                {uint64_t ret = host_read(guest_to_host(addr), len); return ret;}
static void pmem_write(uint64_t addr, int len, uint64_t data)    {host_write(guest_to_host(addr), len, data);}

uint64_t paddr_read(uint64_t addr, int len) {
    if (mem_in_bound(addr)) return pmem_read(addr, len);
    printf("Physical Memory Read : ADDRESS OUT OF BOUND at %lx\n", addr);
    assert(addr - mem_start < mem_size);
    return 0;
}

void paddr_write(uint64_t addr, int len, uint64_t data) {
    if (mem_in_bound(addr)) { pmem_write(addr, len, data); return; }
    printf("Physical Memory Write : ADDRESS OUT OF BOUND at %lx\n", addr);
    assert(addr - mem_start < mem_size);
    return;
}

uint64_t mem_read(uint64_t addr, int len)                   {return paddr_read(addr, len);}
uint64_t mem_write(uint64_t addr, int len, uint64_t val)    {paddr_write(addr, len, val); return val;}