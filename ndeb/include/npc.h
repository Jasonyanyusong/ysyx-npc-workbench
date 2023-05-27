// This file is used for co-simulation, difftest with NEMU, a way to debug NPC
// Prerequsites: A vaild verilator output
#include <stdio.h>
#include "npc/diff.h"
#include "npc/mem.h"
#include "npc/reg.h"
#include "npc/npc.h"

void npc_welcome();
void npc_si(int setps);
void npc_c();
void npc_reg_display();
void npc_mem_display();