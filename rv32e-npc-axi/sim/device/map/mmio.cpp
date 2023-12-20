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

#define NR_MAP 16

static IOMap maps[NR_MAP] = {};
static int nr_map = 0;

static IOMap* fetch_mmio_map(word_t addr) {
  //printf("[fetch_mmio_map] paddr is 0x%8x\n", addr);
  int mapid = find_mapid_by_addr(maps, nr_map, addr);
  return (mapid == -1 ? NULL : &maps[mapid]);
}

static void report_mmio_overlap(const char *name1, word_t l1, word_t r1, const char *name2, word_t l2, word_t r2) {
    printf("[device-mmio], MMIO region %s@[0x%x, 0x%x] is overlapped with %s@[0x%x, 0x%x]\n", name1, l1, r1, name2, l2, r2);
    assert(0);
}

/* device interface */
void add_mmio_map(const char *name, word_t addr, void *space, uint32_t len, io_callback_t callback) {
  assert(nr_map < NR_MAP);
  word_t left = addr, right = addr + len - 1;
  if (in_pmem(left) || in_pmem(right)) {
    report_mmio_overlap(name, left, right, "pmem", MEM_START, MEM_END);
  }
  for (int i = 0; i < nr_map; i++) {
    if (left <= maps[i].high && right >= maps[i].low) {
      report_mmio_overlap(name, left, right, maps[i].name, maps[i].low, maps[i].high);
    }
  }

  maps[nr_map] = (IOMap){ .name = name, .low = addr, .high = addr + len - 1, .space = space, .callback = callback };
  printf("Add mmio map %s@[0x%x, 0x%x]\n", maps[nr_map].name, maps[nr_map].low, maps[nr_map].high);

  nr_map ++;
}

/* bus interface */
word_t mmio_read(word_t addr, int len) {
  //printf("[mmio_read] paddr is 0x%8x\n", addr);
  return map_read(addr, len, fetch_mmio_map(addr));
}

void mmio_write(word_t addr, int len, word_t data) {
  //printf("[mmio_read] paddr is 0x%8x\n", addr);
  map_write(addr, len, data, fetch_mmio_map(addr));
}
