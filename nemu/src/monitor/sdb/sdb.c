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

static int is_batch_mode = false;

bool sdb_print_instruction = false;
bool sdb_print_debug = false;
bool sdb_print_checkpoint = false;
bool sdb_print_assertpoint = false;


void init_regex();
void init_wp_pool();

void set_sdb_print_instruction(bool target_sdb_print_instruction)
{
  sdb_print_instruction = target_sdb_print_instruction;
  if(sdb_print_instruction)
  {
    printf("sdb_print_instruction: ON\n");
  }
  else
  {
    printf("sdb_print_instruction: OFF\n");
  }
  return;
}

void set_sdb_print_debug(bool target_sdb_print_debug)
{
  sdb_print_debug = target_sdb_print_debug;
  if(sdb_print_debug)
  {
    printf("sdb_print_debug: ON\n");
  }
  else
  {
    printf("sdb_print_debug: OFF\n");
  }
  return;
}

void set_sdb_print_checkpoint(bool target_sdb_print_checkpoint)
{
  sdb_print_checkpoint = target_sdb_print_checkpoint;
  if(sdb_print_checkpoint)
  {
    printf("sdb_print_checkpoint: ON\n");
  }
  else
  {
    printf("sdb_print_checkpoint: OFF\n");
  }
  return;
}

void set_sdb_print_assertpoint(bool target_sdb_print_assertpoint)
{
  sdb_print_assertpoint = target_sdb_print_assertpoint;
  if(sdb_print_assertpoint)
  {
    printf("sdb_print_assertpoint: ON\n");
  }
  else
  {
    printf("sdb_print_assertpoint: OFF\n");
  }
  return;
}

bool get_sdb_print_instruction()
{
  return sdb_print_instruction;
}
bool get_sdb_print_debug()
{
  return sdb_print_debug;
}
bool get_sdb_print_checkpoint()
{
  return sdb_print_checkpoint;
}
bool get_sdb_print_assertpoint()
{
  return sdb_print_assertpoint;
}

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
  if (sdb_print_instruction)
  {
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_c(char *args)] cmd_c command\n");
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_c(char *args)] Continue the execution of the program\n");
  }
  cpu_exec(-1);
  return 0;
}

static int cmd_si(char *args){
  if (sdb_print_instruction)
  {
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_si(char *args)] cmd_si command\n");
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_si(char *args)] Run the program for N steps and then suspend, if N is not given, defalt is 1\n");
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_si(char *args)] Subcommnd Received: \"%s\"\n", args);
  }
  if (args == NULL){
    if (sdb_print_debug)
    {
      printf("[NEMU_SDB_DEBUG: static int cmd_si(char *args)] No Subcommand received, default 1\n");
    }
    cpu_exec(1);
  }
  else{
    int cmd_si_n;
    cmd_si_n = atoi(args);
    if(cmd_si_n < -1)
    {
      printf("Invalid input\n");
      return 0;
    }
    if (sdb_print_debug)
    {
      printf("[NEMU_SDB_DEBUG: static int cmd_si(char *args)] Will execute cpu_exec %d times\n", cmd_si_n);
    }
    cpu_exec(cmd_si_n);
  }
  if (sdb_print_debug)
  {
    printf("[NEMU_SDB_DEBUG: static int cmd_si(char *args)] Execution finished\n");
  }
  return 0;
}

static int cmd_reglog(char *args){
  if (sdb_print_instruction)
  {
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_reglog(char *args)] cmd_reglog command\n");
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_reglog(char *args)] Show the log of registers, default 1, can add a number between 0 and 31 to show specified register\n");
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_reglog(char *args)] Subcommnd Received: \"%s\"\n", args);
  }
  if (args == NULL){
    if (sdb_print_debug)
    {
      printf("[NEMU_SDB_DEBUG: static int cmd_reglog(char *args)] No Subcommand received, default all\n");
    }
    reglog_print_all();
  }
  else{
    int cmd_reglog_n;
    cmd_reglog_n = atoi(args);
    if(cmd_reglog_n < 0 || cmd_reglog_n > 31)
    {
      printf("Invalid input\n");
      return 0;
    }
    if (sdb_print_debug)
    {
      printf("[NEMU_SDB_DEBUG: static int cmd_reglog(char *args)] Will show reglog of register No.%d\n", cmd_reglog_n);
    }
    reglog_print(cmd_reglog_n);
  }
  if (sdb_print_debug)
  {
    printf("[NEMU_SDB_DEBUG: static int cmd_reglog(char *args)] Execution finished\n");
  }
  return 0;
}

static int cmd_info(char *args){
  if (sdb_print_instruction)
  {
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_info(char *args)] cmd_info command\n");
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_info(char *args)] info r: Print the state of register\n");
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_info(char *args)] info w: Print the information of watch point(s)\n");
  }

  if (args == NULL)
  {
    printf("No Subcommand\n");
    return 0;
  }
  else
  {
    if (strcmp(args, "r") == 0)
    {
      if (sdb_print_debug)
      {
        printf("[NEMU_SDB_DEBUG: static int cmd_info(char *args)] Received Subcommand “r”: print the state of register\n");
      }
      isa_reg_display();
    }
  else if (strcmp(args, "w") == 0)
    {
      if (sdb_print_debug)
      {
        printf("[NEMU_SDB_DEBUG: static int cmd_info(char *args)] Received Subcommand “w”: print the information of watch point(s)\n");
      }
      print_WP();
    }
  else
    {
      printf("Subcommand Not Defined\n");
    }
  }
  if (sdb_print_debug)
  {
    printf("[NEMU_SDB_DEBUG: static int cmd_info(char *args)] Execution finished\n");
  }
  return 0;
}

void print_memory_allisa(int allisa_start_memory_address, int steps)
{
  printf("******************************************************************************************************************\n");
  printf("|  Address   | 1b Phys | 2b Phys | 4b Phys  |     8b Phys      | 1b Virt | 2b Virt | 4b Virt  |     8b Virt      |\n");
  for (int i = allisa_start_memory_address; i < allisa_start_memory_address + steps; i = i + 1)
  {
    IFDEF(CONFIG_ISA64, printf("| 0x%x | %7lx | %7lx | %8lx | %16lx | %7lx | %7lx | %8lx | %16lx |\n", i, paddr_read(i, 1), paddr_read(i, 2),paddr_read(i, 4), paddr_read(i, 8), vaddr_read(i, 1), vaddr_read(i, 2),vaddr_read(i, 4), vaddr_read(i, 8)););
    IFNDEF(CONFIG_ISA64, printf("| 0x%x | %7x | %7x | %8x |        NA        | %7x | %7x | %8x |        NA        |\n", i, paddr_read(i, 1), paddr_read(i, 2),paddr_read(i, 4), vaddr_read(i, 1), vaddr_read(i, 2),vaddr_read(i, 4)););
  }
  printf("******************************************************************************************************************\n");
}

static int cmd_x(char *args){
  if(sdb_print_instruction)
  {
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_x(char *args)] cmd_x command\n");
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_x(char *args)] x N EXPR: Solve the value of EXPR, set the result of the start of memory address, using hexadecimal as output, print 1/2/4(/8 RISCV64 Only) continue Bit\n");
  }
  int print_length;
  int start_memory_address;
  char *last_part_of_args;
  char *string_token_first = strtok_r(args, " ", &last_part_of_args);
  print_length = atoi(string_token_first);
  sscanf(last_part_of_args, "%x", &start_memory_address);
  if(sdb_print_debug)
  {
    printf("[NEMU_SDB_DEBUG: static int cmd_x(char *args)] string_token_first is : \"%s\"\n", string_token_first);
    printf("[NEMU_SDB_DEBUG: static int cmd_x(char *args)] print_length is : \"%d\"\n", print_length);
    printf("[NEMU_SDB_DEBUG: static int cmd_x(char *args)] last_part_of_args is : \"%s\"\n", last_part_of_args);
    printf("[NEMU_SDB_DEBUG: static int cmd_x(char *args)] start_memory_address (Oct) is: 0o%o\n", start_memory_address);
    printf("[NEMU_SDB_DEBUG: static int cmd_x(char *args)] start_memory_address (Dec) is: 00%d\n", start_memory_address);
    printf("[NEMU_SDB_DEBUG: static int cmd_x(char *args)] start_memory_address (Hex) is: 0x%x\n", start_memory_address);
  }
  print_memory_allisa(start_memory_address, print_length);
  if(sdb_print_debug)
  {
    printf("[NEMU_SDB_DEBUG: static int cmd_x(char *args)] Execution finished\n");
  }
  return 0;
}

static int cmd_p(char *args){
  if(sdb_print_instruction)
  {
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_p(char *args)] cmd_p command\n");
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_p(char *args)] p EXPR: Solve the expression EXPR\n");
  }
  bool expression_success;
  expression_success = false;
  u_int64_t cmd_p_result = 0;
  if(sdb_print_debug)
  {
    printf("[NEMU_SDB_DEBUG: static int cmd_p(char *args)] Received Expression: \"%s\" , evaluating\n", args);
  }
  cmd_p_result = expr(args, &expression_success);
  printf("%ld\t", cmd_p_result);
  printf("0x%lx\n", cmd_p_result);
  return 0;
}

static int cmd_w(char *args){
  if(sdb_print_instruction)
  {
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_w(char *args)] cmd_w command\n");
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_w(char *args)] When the value of EXPR changes, suspend the program\n");
  }
  if(args == NULL)
  {
    if(sdb_print_debug)
    {
      printf("[NEMU_SDB_DEBUG: static int cmd_w(char *args)] No args\n");
    }
    return -1;
  }
  else
  {
    record_WP(args);
  }
  return 0;
}

static int cmd_d(char *args){
  if(sdb_print_instruction)
  {
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_d(char *args)] cmd_d command\n");
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_d(char *args)] Delete the watch point with number N\n");
  }
  if(args == NULL)
  {
    if(sdb_print_debug)
    {
      printf("[NEMU_SDB_DEBUG: static int cmd_w(char *args)] No args\n");
    }
    return -1;
  }
  else
  {
    delete_WP(atoi(args));
  }
  return 0;
}

static int cmd_q(char *args) {
  if(sdb_print_instruction)
  {
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_q(char *args)] cmd_q command\n");
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_q(char *args)] Exit NEMU\n");
  }
  nemu_state.state = NEMU_QUIT;
  // Refined the function for quiting NEMU, so the system will not report bug.
  // Principle: this is the function that calls the quit of NEMU, bu defalt, the function will not change the NEMU state when quiting.
  // If we add "nemu_state.state = NEMU_QUIT;" the system will know that NEMU quit with status "NEMU_QUIT", there will no bug generated.
  return -1;
}

void message_set_instruction()
{
  printf("\n");
  printf("Modify mode of different types(instruction, debug, checkopint, assertpoint) of message output in different areas(sdb, expr, watchpoint)\n");
  printf("1) Type the tool to make modification, in this tool type \"NEMU\" with a \"_\" at the end\n");
  printf("2) Declare the area of changing massage status, type \"SDB\", \"EXPR\" or \"WATCHPOINT\" with a \"_\" at the end\n");
  printf("3) Declare what type of message to modify, use \"INSTRUCTION\", \"DEBUG\", \"CHECKPOINT\" or \"ASSERTPOINT\" with a \"_\" at the end\n");
  printf("4) Declare the status of message, use \"on\" or \"off\" with no space or any other character at the end\n");
  printf("Only inputs that match the 3 rules can be interpreted by cmd_message()\n");
  printf("Inputs are like :\"message AREA TYPE STATUS\"\n");
  return;
}

void print_message_status()
{
  printf("\n");
  printf("1) In Simple Debuger Module:\n");
  if(get_sdb_print_instruction())
  {
    printf("Instruction Message: ON \n");
  }
  else
  {
    printf("Instruction Message: OFF \n");
  }
  if(get_sdb_print_debug())
  {
    printf("Debug Message: ON \n");
  }
  else
  {
    printf("Debug Message: OFF \n");
  }
  if(get_sdb_print_checkpoint())
  {
    printf("Checkpoint Message: ON \n");
  }
  else
  {
    printf("Checkpoint Message: OFF \n");
  }
  if(get_sdb_print_assertpoint())
  {
    printf("Assertpoint Message: ON \n");
  }
  else
  {
    printf("Assertpoint Message: OFF \n");
  }
  printf("\n");

  printf("2) In Expression Evaluation Module:\n");
  if(get_expr_print_instruction())
  {
    printf("Instruction Message: ON \n");
  }
  else
  {
    printf("Instruction Message: OFF \n");
  }
  if(get_expr_print_debug())
  {
    printf("Debug Message: ON \n");
  }
  else
  {
    printf("Debug Message: OFF \n");
  }
  if(get_expr_print_checkpoint())
  {
    printf("Checkpoint Message: ON \n");
  }
  else
  {
    printf("Checkpoint Message: OFF \n");
  }
  if(get_expr_print_assertpoint())
  {
    printf("Assertpoint Message: ON \n");
  }
  else
  {
    printf("Assertpoint Message: OFF \n");
  }
  printf("\n");

  printf("3) In Watch Point Module:\n");
  if(get_watchpoint_print_instruction())
  {
    printf("Instruction Message: ON \n");
  }
  else
  {
    printf("Instruction Message: OFF \n");
  }
  if(get_watchpoint_print_debug())
  {
    printf("Debug Message: ON \n");
  }
  else
  {
    printf("Debug Message: OFF \n");
  }
  if(get_watchpoint_print_checkpoint())
  {
    printf("Checkpoint Message: ON \n");
  }
  else
  {
    printf("Checkpoint Message: OFF \n");
  }
  if(get_watchpoint_print_assertpoint())
  {
    printf("Assertpoint Message: ON \n");
  }
  else
  {
    printf("Assertpoint Message: OFF \n");
  }
  printf("\n");

  return;
}

static int cmd_message(char *args)
{
  if(sdb_print_instruction)
  {
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_message(char *args)] cmd_message command\n");
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_message(char *args)] Modify mode of different types(instruction, debug, checkopint, assertpoint) of message output in different areas(sdb, expr, watchpoint)\n");
  }

  if(args != NULL)
  {
    if(strcmp(args, "NEMU_SDB_INSTRUCTION on") == 0)
    {
      set_sdb_print_instruction(true);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_SDB_INSTRUCTION off") == 0)
    {
      set_sdb_print_instruction(false);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_SDB_DEBUG on") == 0)
    {
      set_sdb_print_debug(true);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_SDB_DEBUG off") == 0)
    {
      set_sdb_print_debug(false);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_SDB_CHECKPOINT on") == 0)
    {
      set_sdb_print_checkpoint(true);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_SDB_CHECKPOINT off") == 0)
    {
      set_sdb_print_checkpoint(false);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_SDB_ASSERTPOINT on") == 0)
    {
      set_sdb_print_assertpoint(true);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_SDB_ASSERTPOINT off") == 0)
    {
      set_sdb_print_assertpoint(false);
      print_message_status();
      return 0;
    }

    if(strcmp(args, "NEMU_EXPR_INSTRUCTION on") == 0)
    {
      set_expr_print_instruction(true);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_EXPR_INSTRUCTION off") == 0)
    {
      set_expr_print_instruction(false);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_EXPR_DEBUG on") == 0)
    {
      set_expr_print_debug(true);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_EXPR_DEBUG off") == 0)
    {
      set_expr_print_debug(false);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_EXPR_CHECKPOINT on") == 0)
    {
      set_expr_print_checkpoint(true);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_EXPR_CHECKPOINT off") == 0)
    {
      set_expr_print_checkpoint(false);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_EXPR_ASSERTPOINT on") == 0)
    {
      set_expr_print_assertpoint(true);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_EXPR_ASSERTPOINT off") == 0)
    {
      set_expr_print_assertpoint(false);
      print_message_status();
      return 0;
    }

    if(strcmp(args, "NEMU_WATCHPOINT_INSTRUCTION on") == 0)
    {
      set_watchpoint_print_instruction(true);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_WATCHPOINT_INSTRUCTION off") == 0)
    {
      set_watchpoint_print_instruction(false);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_WATCHPOINT_DEBUG on") == 0)
    {
      set_watchpoint_print_debug(true);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_WATCHPOINT_DEBUG off") == 0)
    {
      set_watchpoint_print_debug(false);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_WATCHPOINT_CHECKPOINT on") == 0)
    {
      set_watchpoint_print_checkpoint(true);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_WATCHPOINT_CHECKPOINT off") == 0)
    {
      set_watchpoint_print_checkpoint(false);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_WATCHPOINT_ASSERTPOINT on") == 0)
    {
      set_watchpoint_print_assertpoint(true);
      print_message_status();
      return 0;
    }
    if(strcmp(args, "NEMU_WATCHPOINT_ASSERTPOINT off") == 0)
    {
      set_watchpoint_print_assertpoint(false);
      print_message_status();
      return 0;
    }

    if(strcmp(args, "help") == 0)
    {
      message_set_instruction();
      return 0;
    }
  }
  printf("Usage Error!\n");
  message_set_instruction();
  return 0;
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
  { "d", "Delete the watch point with number N", cmd_d},
  { "message", "Modify mode of different types(instruction, debug, checkopint, assertpoint) of message output in different areas(sdb, expr, watchpoint)", cmd_message},
  { "reglog", "Show the log of registers, default 1, can add a number between 0 and 31 to show specified register", cmd_reglog}
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  if(sdb_print_instruction)
  {
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_help(char *args)] cmd_help command\n");
    printf("[NEMU_SDB_INSTRUCTION: static int cmd_help(char *args)] Display information about all supported commands\n");
  }
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
  /* Initialize the register log*/
  IFDEF(CONFIG_REGLOG, init_reglog());
  IFNDEF(CONFIG_REGLOG, Log("reglog is not enabled, to enable it, run \"make menuconfig\"!"));

  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}