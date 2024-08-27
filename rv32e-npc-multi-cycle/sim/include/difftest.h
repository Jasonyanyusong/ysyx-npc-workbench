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
//#include <dlfcn.h>

//#define CONFIG_DIFFTEST

enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };

void difftest_init(char* ref_so_file, word_t img_size);
void difftest_one_exec();
bool difftest_check_reg();
void difftest_skip_ref();

extern void (*ref_difftest_memcpy)(word_t addr, void *buf, word_t n, bool direction);
extern void (*ref_difftest_regcpy)(void *dut, bool direction);
extern void (*ref_difftest_exec)(word_t n);
extern void (*ref_difftest_raise_intr)(word_t NO);