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
#include <verilator-sim.h>

#define PAGE_SHIFT        12
#define PAGE_SIZE         (1ul << PAGE_SHIFT)
#define PAGE_MASK         (PAGE_SIZE - 1)

#define IO_SPACE_MAX (2 * 1024 * 1024)

static uint8_t *io_space = NULL;
static uint8_t *p_space = NULL;

uint8_t* new_space(int size) {
  //printf("[device-map] adding new space with size %d\n", size);
  assert(p_space);
  uint8_t *p = p_space;
  assert(p);
  // page aligned;
  size = (size + (PAGE_SIZE - 1)) & ~PAGE_MASK;
  p_space += size;
  assert(p_space - io_space < IO_SPACE_MAX);
  return p;
}

static void invoke_callback(io_callback_t c, word_t offset, int len, bool is_write) {
  if (c != NULL) { c(offset, len, is_write); }
}

void init_map() {
  io_space = (uint8_t *)malloc(IO_SPACE_MAX);
  assert(io_space);
  p_space = io_space;
  assert(p_space);
}

static bool check_bound(IOMap *map, word_t addr) {
  if (map == NULL) {
    printf("[device] address = 0x%x is out of bound\n", addr);
    npc_state.state = NPC_ABORT;
    return false;
  } else {
    if(addr <= map->high && addr >= map->low){
        return true;
    }else{
        printf("[device] address = 0x%x is out of bound %s@[0x%x, 0x%x]\n", addr, map -> name, map -> low, map -> high);
        npc_state.state = NPC_ABORT;
        return false;
    }
  }
}

word_t map_read(word_t addr, int len, IOMap *map) {
  assert(len >= 1 && len <= 8);

  if(!check_bound(map, addr)){
    return -1;
  }

  check_bound(map, addr);
  word_t offset = addr - map->low;
  invoke_callback(map->callback, offset, len, false); // prepare data to read
  word_t ret = host_read(map->space + offset, len);
  return ret;
}

void map_write(word_t addr, int len, word_t data, IOMap *map) {
  assert(len >= 1 && len <= 8);

  if(!check_bound(map, addr)){
    return;
  }

  check_bound(map, addr);
  word_t offset = addr - map->low;
  host_write(map->space + offset, len, data);
  invoke_callback(map->callback, offset, len, true);
}
