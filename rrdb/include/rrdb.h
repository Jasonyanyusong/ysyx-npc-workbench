#include <stdint.h>
#include <stdio.h>
#define mem_start 0x90000000
#define mem_end   0x9fffffff
#define mem_size mem_end - mem_start

// functions in expr.c
uint64_t expr_eval(char* expr_str);

// functions in watchpoint.c
void wp_add();
void wp_delete();

// functions in init.c
void init_img();
void init_trace();

#include "mem.h"
// functions in mem.cpp
void mem_init();
bool mem_in_bound(uint64_t addr);
uint64_t mem_read(uint64_t addr, int len);
uint64_t mem_write(uint64_t addr, int len, uint64_t val);

// functions in rtl.cpp
void rtl_exec_once();
void rtl_get_gpr(int idx);