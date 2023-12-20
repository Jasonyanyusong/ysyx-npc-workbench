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
#include <device.h>

#define CH_OFFSET 0

static uint8_t *serial_base = NULL;

static void serial_putc(char ch) {
  putc(ch, stderr);
}

static void serial_io_handler(uint32_t offset, int len, bool is_write) {
  assert(len == 1);
  switch (offset) {
    /* We bind the serial port with the host stderr in NPC. */
    case CH_OFFSET:
      if (is_write) {serial_putc(serial_base[0]);}
      else {printf("do not support read\n"); assert(0);}
      break;
    default: printf("do not support offset = %d\n", offset); assert(0);
  }
}

void init_serial() {
  serial_base = new_space(8);
  add_mmio_map("serial", CONFIG_SERIAL_MMIO, serial_base, 8, serial_io_handler);
}
