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

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
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

WP* new_wp()
{
  if(free_ == NULL)
  {
    assert(0);
    return NULL;
  }
  WP *new_watch_point = free_;
  free_ = free_ -> next;
  return new_watch_point;
}

void free_WP(WP *wp)
{
  wp -> watchpoint_expression = NULL;
  wp -> next = free_;
  free_ = wp;
  return;
}

void record_WP(char* expression)
{
  bool expr_success = false;
  u_int64_t expr_answer = expr(expression, &expr_success);
  if(!expr_success)
  {
    assert(0);
  }
  WP *watchpoint_new = new_wp();
  watchpoint_new -> watchpoint_expression = strndup(expression, strlen(expression));
  watchpoint_new -> watchpoint_last_value = expr_answer;
  watchpoint_new -> next = head;
  head = watchpoint_new;
  return;
}

void delete_WP(int WP_number)
{
  WP *current_watchpoint = NULL;
  WP *previous_watchpoint = NULL;
  for(current_watchpoint = head; current_watchpoint != NULL; previous_watchpoint = current_watchpoint, current_watchpoint = current_watchpoint -> next)
  {
    // If the code enter this loop without finish means that the watchpoint we need to remove is not the first one
    if(current_watchpoint -> NO == WP_number)
    {
      if(previous_watchpoint == NULL)
      {
        // Find the watchpoint that NO is WP_number, with NO previous watchpoint
        head = current_watchpoint -> next;
      }
      else
      {
        // Find the watchpoint that NO is WP_number, with previous watchpoint
        previous_watchpoint -> next = current_watchpoint -> next;
      }
      free_WP(current_watchpoint);
      return;
    }
  }
}

void print_WP()
{
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
  for(WP *current_checking_watchpoint = head; current_checking_watchpoint != NULL; current_checking_watchpoint = current_checking_watchpoint -> next)
  {
    bool expr_success = false;
    current_checking_watchpoint -> watchpoint_current_value = expr(current_checking_watchpoint -> watchpoint_expression, &expr_success);
    if(!expr_success)
    {
      assert(0);
    }
  }
  print_WP();
  for(WP *current_checking_watchpoint = head; current_checking_watchpoint != NULL; current_checking_watchpoint = current_checking_watchpoint -> next)
  {
    if(current_checking_watchpoint -> watchpoint_last_value != current_checking_watchpoint -> watchpoint_current_value)
    {
      nemu_state.state = NEMU_STOP;
    }
  }
  return;
}
