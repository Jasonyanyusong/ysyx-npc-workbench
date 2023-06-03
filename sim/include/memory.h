#ifndef __MEMORY_HOST_H__
#define __MEMORY_HOST_H__

#include <stdint.h>
#include <assert.h>

uint64_t host_read(void *addr, int len){
    // TODO: Copy codes from NEMU.
    assert(0);
    return 0;
}

void host_write(void *addr, int len, uint64_t data){
    // TODO: Copy codes from NEMU.
    assert(0);
    return;
}

#endif