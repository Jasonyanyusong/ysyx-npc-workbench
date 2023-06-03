#ifndef __MEMORY_HOST_H__
#define __MEMORY_HOST_H__

#include <stdint.h>
#include <assert.h>
#include <stdio.h>

uint64_t host_read(void *addr, int len){
    switch (len){
        case 1: return *(uint8_t  *)addr;
        case 2: return *(uint16_t *)addr;
        case 4: return *(uint32_t *)addr;
        case 8: return *(uint64_t *)addr;
        default: printf("Invalid len"); assert(0); break;
    }
    return 0;
}

void host_write(void *addr, int len, uint64_t data){
    switch (len){
        case 1: *(uint8_t  *)addr = data; return; break;
        case 2: *(uint16_t *)addr = data; return; break;
        case 4: *(uint32_t *)addr = data; return; break;
        case 8: *(uint64_t *)addr = data; return; break;
        default: printf("Invalid len"); assert(0); return;
    }
    return;
}

#endif