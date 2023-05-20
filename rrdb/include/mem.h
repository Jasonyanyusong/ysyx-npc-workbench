#include <stdint.h>
#include <assert.h>
#include <stdio.h>

uint64_t paddr_read(uint64_t addr, int len);
void paddr_write(uint64_t addr, int len, uint64_t data);

uint8_t* guest_to_host(uint64_t paddr);
uint64_t host_to_guest(uint8_t *haddr);

static inline uint64_t host_read(void *addr, int len) {
  switch (len) {
    case 1: return *(uint8_t  *)addr;
    case 2: return *(uint16_t *)addr;
    case 4: return *(uint32_t *)addr;
    case 8: return *(uint64_t *)addr;
    default: printf("Host Read: Invalid length\n"); assert(0);
  }
}

static inline void host_write(void *addr, int len, uint64_t data) {
  switch (len) {
    case 1: *(uint8_t  *)addr = data; return;
    case 2: *(uint16_t *)addr = data; return;
    case 4: *(uint32_t *)addr = data; return;
    case 8: *(uint64_t *)addr = data; return;
    default: printf("Host Write: Invalid length\n"); assert(0);
  }
}