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

//#include <common.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <getopt.h>

void init_monitor(int argc, char* argv[]);
void parse_args(int argc, char* argv[]);
void load_image();

int cmd_c(char* args);
int cmd_q(char* args);
int cmd_m(char* args);
int cmd_r(char* args);
int cmd_s(char* args);
int cmd_h(char* args);

void sdb_main_loop();
void sdb_init_sdb();
void sdb_set_batch_mode();