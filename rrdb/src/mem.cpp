#include "rrdb.h"
void mem_init(){
    return;
}

bool mem_in_bound(uint64_t addr){
    return addr - mem_start < mem_size;
}

uint64_t mem_read(uint64_t addr, int len){
    return 0;
}
uint64_t mem_write(uint64_t addr, int len, uint64_t val){
    return 0;
}