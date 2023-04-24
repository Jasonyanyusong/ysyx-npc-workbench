/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
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

#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char *e, bool *success);

// Functions in sdb.c
void set_sdb_print_instruction(bool target_sdb_print_instruction);
void set_sdb_print_debug(bool target_sdb_print_debug);
void set_sdb_print_checkpoint(bool target_sdb_print_checkpoint);
void set_sdb_print_assertpoint(bool target_sdb_print_assertpoint);
bool get_sdb_print_instruction();
bool get_sdb_print_debug();
bool get_sdb_print_checkpoint();
bool get_sdb_print_assertpoint();

// Functions in expr.c
void set_expr_print_instruction(bool target_expr_print_instruction);
void set_expr_print_debug(bool target_expr_print_debug);
void set_expr_print_checkpoint(bool target_expr_print_checkpoint);
void set_expr_print_assertpoint(bool target_expr_print_assertpoint);
bool get_expr_print_instruction();
bool get_expr_print_debug();
bool get_expr_print_checkpoint();
bool get_expr_print_assertpoint();

// Functions in watchpoint.c
void set_watchpoint_print_instruction(bool target_watchpoint_print_instruction);
void set_watchpoint_print_debug(bool target_watchpoint_print_debug);
void set_watchpoint_print_checkpoint(bool target_watchpoint_print_checkpoint);
void set_watchpoint_print_assertpoint(bool target_watchpoint_print_assertpoint);
bool get_watchpoint_print_instruction();
bool get_watchpoint_print_debug();
bool get_watchpoint_print_checkpoint();
bool get_watchpoint_print_assertpoint();

// Functions in watchpoint.c
void record_WP(char* expression);
void delete_WP(int WP_number);
void print_WP();
void check_WP();

// Functions in reglog.c
void init_reglog();
void reglog_print(int register_number);
void reglog_print_all();
void reglog_record(word_t* register_values, word_t pc);

#endif
