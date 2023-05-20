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

#include "sdb.h"
#include <string.h>

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char* watchpoint_expression;
  u_int64_t watchpoint_last_value;
  u_int64_t watchpoint_current_value;

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

bool watchpoint_print_instruction = false;
bool watchpoint_print_debug = false;
bool watchpoint_print_checkpoint = false;
bool watchpoint_print_assertpoint = false;
void set_watchpoint_print_instruction(bool target_watchpoint_print_instruction);
void set_watchpoint_print_debug(bool target_watchpoint_print_debug);
void set_watchpoint_print_checkpoint(bool target_watchpoint_print_checkpoint);
void set_watchpoint_print_assertpoint(bool target_watchpoint_print_assertpoint);

void set_watchpoint_print_instruction(bool target_watchpoint_print_instruction)
{
  watchpoint_print_instruction = target_watchpoint_print_instruction;
  if(watchpoint_print_instruction)
  {
    printf("watchpoint_print_instruction: ON\n");
  }
  else
  {
    printf("watchpoint_print_instruction: OFF\n");
  }
  return;
}

void set_watchpoint_print_debug(bool target_watchpoint_print_debug)
{
  watchpoint_print_debug = target_watchpoint_print_debug;
  if(watchpoint_print_debug)
  {
    printf("watchpoint_print_debug: ON\n");
  }
  else
  {
    printf("watchpoint_print_debug: OFF\n");
  }
  return;
}

void set_watchpoint_print_checkpoint(bool target_watchpoint_print_checkpoint)
{
  watchpoint_print_checkpoint = target_watchpoint_print_checkpoint;
  if(watchpoint_print_checkpoint)
  {
    printf("watchpoint_print_checkpoint: ON\n");
  }
  else
  {
    printf("watchpoint_print_checkpoint: OFF\n");
  }
  return;
}

void set_watchpoint_print_assertpoint(bool target_watchpoint_print_assertpoint)
{
  watchpoint_print_assertpoint = target_watchpoint_print_assertpoint;
  if(watchpoint_print_assertpoint)
  {
    printf("watchpoint_print_assertpoint: ON\n");
  }
  else
  {
    printf("watchpoint_print_assertpoint: OFF\n");
  }
  return;
}

bool get_watchpoint_print_instruction()
{
  return watchpoint_print_instruction;
}
bool get_watchpoint_print_debug()
{
  return watchpoint_print_debug;
}
bool get_watchpoint_print_checkpoint()
{
  return watchpoint_print_checkpoint;
}
bool get_watchpoint_print_assertpoint()
{
  return watchpoint_print_assertpoint;
}

void init_wp_pool() {
  if(watchpoint_print_checkpoint)
  {
    printf("[NEMU_WATCHPOINT_CHECKPOINT: void init_wp_pool()] CKPT #01: Enter function\n");
  }
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    wp_pool[i].watchpoint_expression = NULL;
    wp_pool[i].watchpoint_last_value = 0;
    wp_pool[i].watchpoint_current_value = 0;
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp()
{
  if(free_ == NULL)
  {
    if(watchpoint_print_debug)
    {
      printf("[NEMU_WATCHPOINT_DEBUG: WP* new_wp()] free_ == NULL\n");
    }
    if(watchpoint_print_assertpoint)
    {
      printf("[NEMU_WATCHPOINT_ASSERTPOINT: WP* new_wp()] No free watchpoint space availablen\n");
    }
    assert(0);
    return NULL;
  }
  WP *new_watch_point = free_;
  if(watchpoint_print_checkpoint)
  {
    printf("[NEMU_WATCHPOINT_CHECKPOINT: WP* new_wp()] CKPT #01: set the new watchpoint\n");
  }
  free_ = free_ -> next;
  if(watchpoint_print_checkpoint)
  {
    printf("[NEMU_WATCHPOINT_CHECKPOINT: WP* new_wp()] CKPT #02: move free_ to the next free_\n");
  }
  return new_watch_point;
}

void free_WP(WP *wp)
{
  if(watchpoint_print_checkpoint)
  {
    printf("[NEMU_WATCHPOINT_CHECKPOINT: void free_wp(WP *wp)] CKPT #01: Enter function\n");
  }
  wp -> watchpoint_expression = NULL;
  if(watchpoint_print_checkpoint)
  {
    printf("[NEMU_WATCHPOINT_CHECKPOINT: void free_wp(WP *wp)] CKPT #02: Execute \"wp -> watchpoint_expression = NULL;\" Finished\n");
  }
  wp -> next = free_;
  if(watchpoint_print_checkpoint)
  {
    printf("[NEMU_WATCHPOINT_CHECKPOINT: void free_wp(WP *wp)] CKPT #03: Execute \"wp -> next = free_;\" Finished\n");
  }
  free_ = wp;
  if(watchpoint_print_checkpoint)
  {
    printf("[NEMU_WATCHPOINT_CHECKPOINT: void free_wp(WP *wp)] CKPT #04: Execute \"free_ = wp;\" Finished, End function\n");
  }
  return;
}

void record_WP(char* expression)
{
  if(watchpoint_print_checkpoint)
  {
    printf("[NEMU_WATCHPOINT_CHECKPOINT: void record_WP(char* expression)] CKPT #01: Enter function\n");
  }
  bool expr_success = false;
  u_int64_t expr_answer = expr(expression, &expr_success);
  if(watchpoint_print_debug)
  {
    printf("[NEMU_WATCHPOINT_DEBUG: void record_WP(char* expression)] expr_answer (Dec) = 00%ld\n", expr_answer);
    printf("[NEMU_WATCHPOINT_DEBUG: void record_WP(char* expression)] expr_answer (Hex) = 0x%lx\n", expr_answer);
  }
  if(!expr_success)
  {
    if(watchpoint_print_debug)
    {
      printf("[NEMU_WATCHPOINT_DEBUG: void record_WP(char* expression)] !expr_success\n");
    }
    if(watchpoint_print_assertpoint)
    {
      printf("[NEMU_WATCHPOINT_ASSERTPOINT: void record_WP(char* expression)] expression evaluate failed\n");
    }
    assert(0);
  }
  WP *watchpoint_new = new_wp();
  watchpoint_new -> watchpoint_expression = strndup(expression, strlen(expression));
  watchpoint_new -> watchpoint_last_value = expr_answer;
  if(watchpoint_print_debug)
  {
    printf("[NEMU_WATCHPOINT_DEBUG: void record_WP(char* expression)] Information about new watchpoint\n");
    printf("[NEMU_WATCHPOINT_DEBUG: void record_WP(char* expression)] watchpoint_new -> watchpoint_expression = \"%s\"\n", watchpoint_new -> watchpoint_expression);
    printf("[NEMU_WATCHPOINT_DEBUG: void record_WP(char* expression)] watchpoint_new -> watchpoint_last_value = 00%ld\n", watchpoint_new -> watchpoint_last_value);
    printf("[NEMU_WATCHPOINT_DEBUG: void record_WP(char* expression)] watchpoint_new -> watchpoint_last_value = 0x%lx\n", watchpoint_new -> watchpoint_last_value);
  }
  watchpoint_new -> next = head;
  head = watchpoint_new;
  return;
}

void delete_WP(int WP_number)
{
  if(watchpoint_print_checkpoint)
  {
    printf("[NEMU_WATCHPOINT_CHECKPOINT: void delete_WP(int WP_number)] CKPT #01: Enter function\n");
  }
  WP *current_watchpoint = NULL;
  WP *previous_watchpoint = NULL;
  for(current_watchpoint = head; current_watchpoint != NULL; previous_watchpoint = current_watchpoint, current_watchpoint = current_watchpoint -> next)
  {
    // If the code enter this loop without finish means that the watchpoint we need to remove is not the first one
    if(watchpoint_print_checkpoint)
    {
      printf("[NEMU_WATCHPOINT_CHECKPOINT: void delete_WP(int WP_number)] CKPT #02: Enter loop, current_watchpoint -> NO = %d\n", current_watchpoint -> NO);
    }
    if(current_watchpoint -> NO == WP_number)
    {
      if(previous_watchpoint == NULL)
      {
        // Find the watchpoint that NO is WP_number, with NO previous watchpoint
        if(watchpoint_print_debug)
        {
          printf("[NEMU_WATCHPOINT_DEBUG: void delete_WP(int WP_number)] Find the watchpoint that NO is WP_number, with NO previous watchpoint\n");
        }
        head = current_watchpoint -> next;
      }
      else
      {
        // Find the watchpoint that NO is WP_number, with previous watchpoint
        if(watchpoint_print_debug)
        {
          printf("[NEMU_WATCHPOINT_DEBUG: void delete_WP(int WP_number)] Find the watchpoint that NO is WP_number, with previous watchpoint\n");
        }
        previous_watchpoint -> next = current_watchpoint -> next;
      }
      free_WP(current_watchpoint);
      return;
    }
  }
}

void print_WP()
{
  if(watchpoint_print_checkpoint)
  {
    printf("[NEMU_WATCHPOINT_CHECKPOINT: void print_WP(int WP_number)] CKPT #01: Enter function\n");
  }
  printf("**************************************************************************************(Oct)***************************************************************************************\n");
  printf("| Watchpoint Number |                               Watchpoint Expression                               | Watchpoint Expression Last Value | Watchpoint Expression Current Value |\n");
  for(WP *current_print_watchpoint = head; current_print_watchpoint != NULL; current_print_watchpoint = current_print_watchpoint -> next)
  {
    printf("| %17d | \"%79s\" | %32lo | %35lo |\n", current_print_watchpoint -> NO, current_print_watchpoint -> watchpoint_expression, current_print_watchpoint -> watchpoint_last_value, current_print_watchpoint -> watchpoint_current_value);
  }
  printf("**************************************************************************************(Oct)***************************************************************************************\n");
  printf("\n");
  printf("**************************************************************************************(Dec)***************************************************************************************\n");
  printf("| Watchpoint Number |                               Watchpoint Expression                               | Watchpoint Expression Last Value | Watchpoint Expression Current Value |\n");
  for(WP *current_print_watchpoint = head; current_print_watchpoint != NULL; current_print_watchpoint = current_print_watchpoint -> next)
  {
    printf("| %17d | \"%79s\" | %32ld | %35ld |\n", current_print_watchpoint -> NO, current_print_watchpoint -> watchpoint_expression, current_print_watchpoint -> watchpoint_last_value, current_print_watchpoint -> watchpoint_current_value);
  }
  printf("**************************************************************************************(Dec)***************************************************************************************\n");
  printf("\n");
  printf("**************************************************************************************(Hex)***************************************************************************************\n");
  printf("| Watchpoint Number |                               Watchpoint Expression                               | Watchpoint Expression Last Value | Watchpoint Expression Current Value |\n");
  for(WP *current_print_watchpoint = head; current_print_watchpoint != NULL; current_print_watchpoint = current_print_watchpoint -> next)
  {
    printf("| %17d | \"%79s\" | %32lx | %35lx |\n", current_print_watchpoint -> NO, current_print_watchpoint -> watchpoint_expression, current_print_watchpoint -> watchpoint_last_value, current_print_watchpoint -> watchpoint_current_value);
  }
  printf("**************************************************************************************(Hex)***************************************************************************************\n");
  return;
}

void check_WP()
{
  if(watchpoint_print_checkpoint)
  {
    printf("[NEMU_WATCHPOINT_CHECKPOINT: void check_WP()] CKPT #01: Enter function\n");
  }
  for(WP *current_checking_watchpoint = head; current_checking_watchpoint != NULL; current_checking_watchpoint = current_checking_watchpoint -> next)
  {
    bool expr_success = false;
    current_checking_watchpoint -> watchpoint_current_value = expr(current_checking_watchpoint -> watchpoint_expression, &expr_success);
    if(!expr_success)
    {
      if(watchpoint_print_assertpoint)
      {
        printf("[NEMU_WATCHPOINT_ASSERTPOINT: void check_WP()] Expression evaluation FAILED\n");
      }
      assert(0);
    }
  }
  print_WP();
  for(WP *current_checking_watchpoint = head; current_checking_watchpoint != NULL; current_checking_watchpoint = current_checking_watchpoint -> next)
  {
    if(current_checking_watchpoint -> watchpoint_last_value != current_checking_watchpoint -> watchpoint_current_value)
    {
      if(watchpoint_print_debug)
      {
        printf("[NEMU_WATCHPOINT_DEBUG: void check_WP()] At watchpoint No.%2d, find different in expression answer\n", current_checking_watchpoint -> NO);
        printf("[NEMU_WATCHPOINT_DEBUG: void check_WP()] previous(Oct) = %lo\n", current_checking_watchpoint -> watchpoint_last_value);
        printf("[NEMU_WATCHPOINT_DEBUG: void check_WP()] current (Oct) = %lo\n", current_checking_watchpoint -> watchpoint_current_value);
        printf("[NEMU_WATCHPOINT_DEBUG: void check_WP()] previous(Dec) = %ld\n", current_checking_watchpoint -> watchpoint_last_value);
        printf("[NEMU_WATCHPOINT_DEBUG: void check_WP()] current (Dec) = %ld\n", current_checking_watchpoint -> watchpoint_current_value);
        printf("[NEMU_WATCHPOINT_DEBUG: void check_WP()] previous(Hex) = %lx\n", current_checking_watchpoint -> watchpoint_last_value);
        printf("[NEMU_WATCHPOINT_DEBUG: void check_WP()] current (Hex) = %lx\n", current_checking_watchpoint -> watchpoint_current_value);
      }
      //nemu_state.state = NEMU_STOP;
    }
  }
  return;
}