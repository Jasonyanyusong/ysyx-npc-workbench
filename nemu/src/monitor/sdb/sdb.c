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

#include <isa.h>
#include <cpu/cpu.h>
#include <stdlib.h>
#include <math.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include <memory/host.h>
#include "sdb.h"
#include "trace/trace.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_si(char *args){
  if (args == NULL){
    cpu_exec(1);
  }
  else{
    int cmd_si_n;
    cmd_si_n = atoi(args);
    if(cmd_si_n < -1){
      printf("Invalid input\n");
      return 0;
    }
    cpu_exec(cmd_si_n);
  }
  return 0;
}

static int cmd_info(char *args){
  if (args == NULL){
    printf("No Subcommand\n");
    return 0;
  }
  else{
    if (strcmp(args, "r") == 0){
      isa_gpr_display();
    }
  else if (strcmp(args, "w") == 0){
      print_WP();
    }
  else{
      printf("Subcommand Not Defined\n");
    }
  }
  return 0;
}

void print_memory_1(int allisa_start_memory_address, int steps){
  printf("******************************************************************************\n");
  printf("|  Address   | 1b Phys (Hex) | 1b Virt (Hex) | 1b Phys (Dec) | 1b Virt (Dec) |\n");
  for (int i = allisa_start_memory_address; i < allisa_start_memory_address + steps; i = i + 1){
    printf("| 0x%x | 0x   %8lx | 0x   %8lx | 0x %10ld | 0x %10ld |\n", i, paddr_read(i, 1), vaddr_read(i, 1), paddr_read(i, 1), vaddr_read(i, 1));
  }
  printf("******************************************************************************\n\n");
}

void print_memory_2(int allisa_start_memory_address, int steps){
  printf("******************************************************************************\n");
  printf("|  Address   | 2b Phys (Hex) | 2b Virt (Hex) | 2b Phys (Dec) | 2b Virt (Dec) |\n");
  for (int i = allisa_start_memory_address; i < allisa_start_memory_address + steps; i = i + 2){
    printf("| 0x%x | 0x   %8lx | 0x   %8lx | 0x %10ld | 0x %10ld |\n", i, paddr_read(i, 2), vaddr_read(i, 2), paddr_read(i, 2), vaddr_read(i, 2));
  }
  printf("******************************************************************************\n\n");
}

void print_memory_4(int allisa_start_memory_address, int steps){
  printf("******************************************************************************\n");
  printf("|  Address   | 4b Phys (Hex) | 4b Virt (Hex) | 4b Phys (Dec) | 4b Virt (Dec) |\n");
  for (int i = allisa_start_memory_address; i < allisa_start_memory_address + steps; i = i + 4){
    printf("| 0x%x | 0x   %8lx | 0x   %8lx | 0x %10ld | 0x %10ld |\n", i, paddr_read(i, 4), vaddr_read(i, 4), paddr_read(i, 4), vaddr_read(i, 4));
  }
  printf("******************************************************************************\n\n");
}

static int cmd_x(char *args){
  int print_length;
  int start_memory_address;
  char *last_part_of_args;
  char *string_token_first = strtok_r(args, " ", &last_part_of_args);
  print_length = atoi(string_token_first);
  sscanf(last_part_of_args, "%x", &start_memory_address);
  print_memory_1(start_memory_address, print_length);
  print_memory_2(start_memory_address, print_length);
  print_memory_4(start_memory_address, print_length);
  return 0;
}

static int cmd_p(char *args){
  bool expression_success;
  expression_success = false;
  u_int64_t cmd_p_result = 0;
  cmd_p_result = expr(args, &expression_success);
  printf("%ld\t", cmd_p_result);
  printf("0x%lx\n", cmd_p_result);
  return 0;
}

static int cmd_w(char *args){
  if(args == NULL){
    return -1;
  }
  else{
    record_WP(args);
  }
  return 0;
}

static int cmd_d(char *args){
  if(args == NULL){
    return -1;
  }
  else{
    delete_WP(atoi(args));
  }
  return 0;
}

static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  // Refined the function for quiting NEMU, so the system will not report bug.
  // Principle: this is the function that calls the quit of NEMU, bu defalt, the function will not change the NEMU state when quiting.
  // If we add "nemu_state.state = NEMU_QUIT;" the system will know that NEMU quit with status "NEMU_QUIT", there will no bug generated.
  return -1;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Run the program for N steps and then suspend, if N is not given, defalt is 1", cmd_si},
  { "info", "info r: Print the state of register, info w: Print the information of watch point(s)", cmd_info},
  { "x", "Solve the value of EXPR, set the result of the start of memory address, using hexadecimal as output, print N continue memory", cmd_x},
  { "p", "Solve the expression EXPR", cmd_p},
  { "w", "When the value of EXPR changes, suspend the program", cmd_w},
  { "d", "Delete the watch point with number N", cmd_d}
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();

  /* Initialize tracers*/
  trace_init();
}