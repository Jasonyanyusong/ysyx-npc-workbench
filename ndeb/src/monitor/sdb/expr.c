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
#include "sdb.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <string.h>
#include <stdlib.h>

// Tesk Token #1: p 0x8000 + 123 - 6 * (5 + 4)
// Test Token #2: p 123 + 456 + 789
// Test Token #3: p 123+456+789
// Test Token #4: p 0x8000 + 123
// Test Token #5: p 0x8fff + 0x1234

enum {
 TK_NOTYPE = 256,
 TK_EQ = 255,
 TK_NEQ = 254,
 TK_AND = 253,
 TK_OR = 252,
 TK_NEGATIVESIGN = 251,
 TK_NOT = 250,
 TK_POINTER = 249,
 TK_NUMBER = 248,
 TK_HEXNUMBER = 247,
 TK_REGISTER = 246,
 TK_MARK = 245,
 TK_EXOR = 244,
 TK_POSITIVESIGN = 243,
 TK_PLUS = 242,
 TK_MINUS = 241,
 TK_MULTIPLY = 240,
 TK_DIVIDE = 239,
 TK_LEFT_PARENTHESES = 238,
 TK_RIGHT_PARENTHESES = 237,
 TK_MOD = 236, 
};

bool check_parentheses_balance(); // Used in expr()
bool check_parentheses_valid(); // Used in give_priority_no_parentheses()
bool check_left_token_is_number_or_bool(int check_index);
bool check_right_token_is_number_or_bool(int check_index);
void process_operator_token();
void give_priority();
void give_priority_no_parentheses();
void give_sub_priority();
int bool_to_int(bool bool_value);
bool valid_call;

bool expr_print_instruction = false;
bool expr_print_debug = false;
bool expr_print_checkpoint = false;
bool expr_print_assertpoint = false;
void set_expr_print_instruction(bool target_expr_print_instruction);
void set_expr_print_debug(bool target_expr_print_debug);
void set_expr_print_checkpoint(bool target_expr_print_checkpoint);
void set_expr_print_assertpoint(bool target_expr_print_assertpoint);

int process_add(int add_operator_index);
int process_minus(int minus_operator_index);
int process_multiply(int multiply_operator_index);
int process_devide(int devide_operator_index);
int process_equal(int equal_operator_index);
int process_not_equal(int not_equal_operator_index);
int process_and(int and_operator_index);
int process_or(int or_operator_index);
int process_exor(int exor_operator_index);
int process_not(int not_operator_index);
int process_mod(int mod_operator_index);
int process_negative_sign(int negative_sign_operator_index);
int process_positive_sign(int positive_sign_operator_index);
int process_pointer_dereference(int pointer_dereference_index);

void expr_init();
void init_tokens();
void init_operator_tokens();
void init_operator_tokens_no_parentheses();
void init_execute_history();

char* calculate_one_round(bool success_calculate_one_round_call);

char* expr_main_loop(char* token_input, bool *success_main_loop, bool *finished);

struct OperatorToken
{
  const char *regex;
  int token_type;
  int priority;
  int position;
} operator_tokens[32];

struct OperatorTokenNoParentheses
{
  const char *regex;
  int token_type;
  int priority_level;
  int position;
  int sub_priority_level;
} operator_tokens_no_parentheses[32];

struct ExecuteHistory
{
  const char *result_token_history;
  bool success_history;
  bool finished_history;
  int that_round_operator_token_no_narentheses_index;
  int that_round_token_index;
} execution_histories[32];

int nr_operator_tokens_no_parentheses = 0;
int nr_operator_token = 0;
int nr_execution_histories = 0;

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {
  {" +", TK_NOTYPE}, // Spaces
  {"0x[0-9,a-f]+", TK_HEXNUMBER}, // Hex Numbers
  {"[0-9]+", TK_NUMBER}, // Dec Numbers
  {"\\$[a-z0-9]{1,31}", TK_REGISTER}, // Register Names
  {"\\(", TK_LEFT_PARENTHESES}, // Left Parenthesis IS_OPERATOR_TOKEN
  {"\\)", TK_RIGHT_PARENTHESES}, // Right Parenthesis IS_OPERATOR_TOKEN
  {"\\*", TK_MULTIPLY}, // Multiply IS_OPERATOR_TOKEN
  {"\\/", TK_DIVIDE}, // Devide IS_OPERATOR_TOKEN
  {"\\+", TK_PLUS}, // Plus IS_OPERATOR_TOKEN
  {"\\-", TK_MINUS}, // Minus IS_OPERATOR_TOKEN
  {"\\//", TK_MOD}, // Mod IS_OPERATOR_TOKEN
  {"==", TK_EQ}, // Equal IS_OPERATOR_TOKEN
  {"!=", TK_NEQ}, // Not Equal IS_OPERATOR_TOKEN
  {"&&", TK_AND}, // And IS_OPERATOR_TOKEN
  {"\\|\\|", TK_OR}, // Or IS_OPERATOR_TOKEN
  {"!", TK_NOT}, // Not IS_OPERATOR_TOKEN
  {"\\^", TK_EXOR}, // Exor IS_OPERATOR_TOKEN
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

void set_expr_print_instruction(bool target_expr_print_instruction)
{
  expr_print_instruction = target_expr_print_instruction;
  if(expr_print_instruction)
  {
    printf("expr_print_instruction: ON\n");
  }
  else
  {
    printf("expr_print_instruction: OFF\n");
  }
  return;
}
void set_expr_print_debug(bool target_expr_print_debug)
{
  expr_print_debug = target_expr_print_debug;
  if(expr_print_debug)
  {
    printf("expr_print_debug: ON\n");
  }
  else
  {
    printf("expr_print_debug: OFF\n");
  }
  return;
}
void set_expr_print_checkpoint(bool target_expr_print_checkpoint)
{
  expr_print_checkpoint = target_expr_print_checkpoint;
  if(expr_print_checkpoint)
  {
    printf("expr_print_checkpoint: ON\n");
  }
  else
  {
    printf("expr_print_checkpoint: OFF\n");
  }
  return;
}
void set_expr_print_assertpoint(bool target_expr_print_assertpoint)
{
  expr_print_assertpoint = target_expr_print_assertpoint;
  if(expr_print_assertpoint)
  {
    printf("expr_print_assertpoint: ON\n");
  }
  else
  {
    printf("expr_print_assertpoint: OFF\n");
  }
  return;
}
bool get_expr_print_instruction()
{
  return expr_print_instruction;
}
bool get_expr_print_debug()
{
  return expr_print_debug;
}
bool get_expr_print_checkpoint()
{
  return expr_print_checkpoint;
}
bool get_expr_print_assertpoint()
{
  return expr_print_assertpoint;
}

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

void expr_init()
{
  init_tokens();
  init_operator_tokens();
  init_operator_tokens_no_parentheses();
  return;
}

void init_tokens()
{
  nr_token = 0;
  for(int init_tokens_index = 0; init_tokens_index < 32; init_tokens_index = init_tokens_index + 1)
  {
    memset(tokens[init_tokens_index].str,0,sizeof(tokens[init_tokens_index].str));
    tokens[init_tokens_index].type = -1;
  }
  return;
}

void init_operator_tokens()
{
  nr_operator_token = 0;
  for(int init_operator_tokens_index = 0; init_operator_tokens_index < 32; init_operator_tokens_index = init_operator_tokens_index + 1)
  {
    operator_tokens[init_operator_tokens_index].regex = NULL;
    operator_tokens[init_operator_tokens_index].position = -1;
    operator_tokens[init_operator_tokens_index].priority = -1;
    operator_tokens[init_operator_tokens_index].token_type = -1;
  }
  return;
}

void init_operator_tokens_no_parentheses()
{
  nr_operator_tokens_no_parentheses = 0;
  for(int init_operator_tokens_no_parentheses_index = 0; init_operator_tokens_no_parentheses_index < 32; init_operator_tokens_no_parentheses_index = init_operator_tokens_no_parentheses_index + 1)
  {
    operator_tokens_no_parentheses[init_operator_tokens_no_parentheses_index].regex = NULL;
    operator_tokens_no_parentheses[init_operator_tokens_no_parentheses_index].position = -1;
    operator_tokens_no_parentheses[init_operator_tokens_no_parentheses_index].priority_level = -1;
    operator_tokens_no_parentheses[init_operator_tokens_no_parentheses_index].token_type = -1;
    operator_tokens_no_parentheses[init_operator_tokens_no_parentheses_index].sub_priority_level = -1;
  }
  return;
}

void init_execute_history()
{
  nr_execution_histories = 0;
  for(int init_execute_history_index = 0; init_execute_history_index < 32; init_execute_history_index = init_execute_history_index + 1)
  {
    execution_histories[init_execute_history_index].result_token_history = NULL;
    execution_histories[init_execute_history_index].success_history = false;
    execution_histories[init_execute_history_index].finished_history = false;
    execution_histories[init_execute_history_index].that_round_operator_token_no_narentheses_index = -1;
    execution_histories[init_execute_history_index].that_round_token_index = -1;
  }
}

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    if(expr_print_checkpoint)
    {
      printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #01\n");
    }
    for (i = 0; i < NR_REGEX; i ++) {
      if(expr_print_checkpoint)
      {
        printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #02\n");
      }
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        if(expr_print_checkpoint)
        {
          printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #03\n");
        }
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        switch (rules[i].token_type) 
        {
          default:
          {
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #04: switch (rules[i].token_type) to DEFAULT\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Use Default\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_NOTYPE:
          {
            // Case No.1
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #05\n");
            }
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_NOTYPE TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] TK_NOTYPE will be thrown away directly\n");
            }
            break;
          }
          case TK_EQ:
          {
            // Case No.2
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #06\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_EQ;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_EQ TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] This is a OPERATOR_TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_NEQ:
          {
            // Case No.3
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #07\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_NEQ;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_NEQ TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] This is a OPERATOR_TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_NOT:
          {
            // Case No.4
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #08\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_NOT;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_NOT TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] This is a OPERATOR_TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_AND:
          {
            // Case No.5
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #09\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_AND;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_AND TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] This is a OPERATOR_TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_OR:
          {
            // Case No.6
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #10\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_OR;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_OR TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] This is a OPERATOR_TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_POINTER:
          {
            // Case No.7
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #11\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_POINTER;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_POINTER TOKEN\n");

              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_NUMBER:
          {
            // Case No.10
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #14\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_NUMBER;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_NUMBER TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_HEXNUMBER:
          {
            // Case No.11
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #15\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_HEXNUMBER;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_HEXNUMBER TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            // We convert TK_HEXNUMBER to TK_NUMBER for easy of calculation
            int convert_to_int;
            sscanf(tokens[nr_token].str, "%x", &convert_to_int);
            sprintf(tokens[nr_token].str, "%d", convert_to_int);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] convert_to_int is %d\n", convert_to_int);
            }
            tokens[nr_token].type = TK_NUMBER;
            nr_token = nr_token + 1;
            break;
          }
          case TK_REGISTER: // !!! Special Case
          {
            // Case No.12
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #16\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_REGISTER;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_REGISTER TOKEN\n");

              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_MARK:
          {
            // Case No.13
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #17\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_MARK;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_MARK TOKEN\n");

              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_LEFT_PARENTHESES:
          {
            // Case No.14
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #18\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_LEFT_PARENTHESES;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_LEFT_PARENTHESES TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] This is a OPERATOR_TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_RIGHT_PARENTHESES:
          {
            // Case No.15
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #19\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_RIGHT_PARENTHESES;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_RIGHT_PARENTHESES TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] This is a OPERATOR_TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_MULTIPLY:
          {
            // Case No.16
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #20\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_MULTIPLY;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_MULTIPLY TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] This is a OPERATOR_TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            if((tokens[nr_token - 1].type != TK_NUMBER && tokens[nr_token - 1].type != TK_HEXNUMBER) || nr_token == 0)
            {
              if(expr_print_debug)
              {
                printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] By Double Check, this is actually a TK_POINTER TOKEN\n");
              }
              tokens[nr_token].type = TK_POINTER;
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_DIVIDE:
          {
            // Case No.17
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #21\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_DIVIDE;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_DIVIDE TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] This is a OPERATOR_TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_PLUS:
          {
            // Case No.18
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #22\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_PLUS;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_PLUS TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] This is a OPERATOR_TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            if(tokens[nr_token - 1].type != TK_NUMBER && tokens[nr_token - 1].type != TK_HEXNUMBER)
            {
              if(expr_print_debug)
              {
                printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] By Double Check, this is actually a TK_POSITIVESIGN TOKEN\n");
              }
              tokens[nr_token].type = TK_POSITIVESIGN;
            }
            nr_token = nr_token + 1;
            break;
          }
          case TK_MINUS:
          {
            // Case No.19
            if(expr_print_checkpoint)
            {
              printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #23\n");
            }
            memset(tokens[nr_token].str,0,sizeof(tokens[nr_token].str));
            tokens[nr_token].type = TK_MINUS;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            if(expr_print_debug)
            {
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_start is \"%s\"\n", substr_start);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] substr_len is \"%d\"\n", substr_len);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Found a TK_MINUS TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] This is a OPERATOR_TOKEN\n");
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].type is: %d\n", tokens[nr_token].type);
              printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] tokens[nr_token].str is: \"%s\"\n", tokens[nr_token].str);
            }
            if(tokens[nr_token - 1].type != TK_NUMBER && tokens[nr_token - 1].type != TK_HEXNUMBER)
            {
              if(expr_print_debug)
              {
                printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] By Double Check, this is actually a TK_NEGATIVESIGN TOKEN\n");
              }
              tokens[nr_token].type = TK_NEGATIVESIGN;
            }
            nr_token = nr_token + 1;
            break;
          }
        }
        if(expr_print_checkpoint)
        {
          printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #24\n");
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      if(expr_print_checkpoint)
      {
        printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #25\n");
      }
      Log("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  // Debug Point: Print Tokens
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: static bool make_token(char *e)] CKPT #26\n");
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Token Numbers (Decimal): %d\n", nr_token);
  }
  for(int display_index = 0; display_index < nr_token; display_index = display_index + 1)
  {
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: static bool make_token(char *e)] Token Number: %4d, Token Type (Decimal ID): %4d, Token String: \"%s\"\n", display_index, tokens[display_index].type, tokens[display_index].str);
    }
  }
  return true;
}

void give_priority()
{
  // Give priority ignoring the parentheses
  // Previous (1st version) Precidency Level: (No Parentheses Considered)
  // 1) Or 2) AND 3) EQ/NEQ 4) PLUS/MINUS 5) MULTIPLY/DEVIDE 6) NOT
  // Current (2nd version) Precidency Level: (No Parentheses Considered)
  // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: void give_priority()] CKPT #01: Enter function\n");
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void give_priority()] nr_operator_token = %d\n", nr_operator_token);
  }
  for(int give_priority_index = 0; give_priority_index < nr_operator_token; give_priority_index = give_priority_index + 1)
  {
    if(expr_print_checkpoint)
    {
      printf("[NEMU_EXPR_CHECKPOINT: void give_priority()] CKPT #02: Enter give priority loop at give_priority_index = %d\n", give_priority_index);
    }
    switch(operator_tokens[give_priority_index].token_type)
    {
      case TK_OR:
      {
        // Current (2nd version) Precidency Level: (No Parentheses Considered)
        // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
        // Precidency Level: 1
        operator_tokens[give_priority_index].priority = 1;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_priority()] found a TK_OR operator, set priority to 1\n");
          printf("[NEMU_EXPR_DEBUG: void give_priority()] give_priority_index = %d\n", give_priority_index);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].position = %d\n", operator_tokens[give_priority_index].position);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].priority = %d\n", operator_tokens[give_priority_index].priority);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].regex = \"%s\"\n", operator_tokens[give_priority_index].regex);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].token_type = %d\n", operator_tokens[give_priority_index].token_type);
        }
        break;
      }
      case TK_EXOR:
      {
        // Current (2nd version) Precidency Level: (No Parentheses Considered)
        // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
        // Precidency Level: 2
        operator_tokens[give_priority_index].priority = 2;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_priority()] found a TK_EXOR operator, set priority to 2\n");
          printf("[NEMU_EXPR_DEBUG: void give_priority()] give_priority_index = %d\n", give_priority_index);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].position = %d\n", operator_tokens[give_priority_index].position);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].priority = %d\n", operator_tokens[give_priority_index].priority);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].regex = \"%s\"\n", operator_tokens[give_priority_index].regex);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].token_type = %d\n", operator_tokens[give_priority_index].token_type);
        }
        break;
      }
      case TK_AND:
      {
        // Current (2nd version) Precidency Level: (No Parentheses Considered)
        // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
        // Precidency Level: 3
        operator_tokens[give_priority_index].priority = 3;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_priority()] found a TK_AND operator, set priority to 3\n");
          printf("[NEMU_EXPR_DEBUG: void give_priority()] give_priority_index = %d\n", give_priority_index);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].position = %d\n", operator_tokens[give_priority_index].position);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].priority = %d\n", operator_tokens[give_priority_index].priority);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].regex = \"%s\"\n", operator_tokens[give_priority_index].regex);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].token_type = %d\n", operator_tokens[give_priority_index].token_type);
        }
        break;
      }
      case TK_EQ:
      {
        // Current (2nd version) Precidency Level: (No Parentheses Considered)
        // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
        // Precidency Level: 4
        operator_tokens[give_priority_index].priority = 4;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_priority()] found a TK_EQ operator, set priority to 4\n");
          printf("[NEMU_EXPR_DEBUG: void give_priority()] give_priority_index = %d\n", give_priority_index);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].position = %d\n", operator_tokens[give_priority_index].position);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].priority = %d\n", operator_tokens[give_priority_index].priority);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].regex = \"%s\"\n", operator_tokens[give_priority_index].regex);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].token_type = %d\n", operator_tokens[give_priority_index].token_type);
        }
        break;
      }
      case TK_NEQ:
      {
        // Current (2nd version) Precidency Level: (No Parentheses Considered)
        // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
        // Precidency Level: 4
        operator_tokens[give_priority_index].priority = 4;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_priority()] found a TK_NEQ operator, set priority to 4\n");
          printf("[NEMU_EXPR_DEBUG: void give_priority()] give_priority_index = %d\n", give_priority_index);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].position = %d\n", operator_tokens[give_priority_index].position);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].priority = %d\n", operator_tokens[give_priority_index].priority);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].regex = \"%s\"\n", operator_tokens[give_priority_index].regex);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].token_type = %d\n", operator_tokens[give_priority_index].token_type);
        }
        break;
      }
      case TK_PLUS:
      {
        // Current (2nd version) Precidency Level: (No Parentheses Considered)
        // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
        // Precidency Level: 5
        operator_tokens[give_priority_index].priority = 5;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_priority()] found a TK_PLUS operator, set priority to 5\n");
          printf("[NEMU_EXPR_DEBUG: void give_priority()] give_priority_index = %d\n", give_priority_index);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].position = %d\n", operator_tokens[give_priority_index].position);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].priority = %d\n", operator_tokens[give_priority_index].priority);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].regex = \"%s\"\n", operator_tokens[give_priority_index].regex);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].token_type = %d\n", operator_tokens[give_priority_index].token_type);
        }
        break;
      }
      case TK_MINUS:
      {
        // Current (2nd version) Precidency Level: (No Parentheses Considered)
        // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
        // Precidency Level: 5
        operator_tokens[give_priority_index].priority = 5;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_priority()] found a TK_MINUS operator, set priority to 5\n");
          printf("[NEMU_EXPR_DEBUG: void give_priority()] give_priority_index = %d\n", give_priority_index);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].position = %d\n", operator_tokens[give_priority_index].position);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].priority = %d\n", operator_tokens[give_priority_index].priority);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].regex = \"%s\"\n", operator_tokens[give_priority_index].regex);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].token_type = %d\n", operator_tokens[give_priority_index].token_type);
        }
        break;
      }
      case TK_MULTIPLY:
      {
        // Current (2nd version) Precidency Level: (No Parentheses Considered)
        // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
        // Precidency Level: 6
        operator_tokens[give_priority_index].priority = 6;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_priority()] found a TK_MULTIPLY operator, set priority to 6\n");
          printf("[NEMU_EXPR_DEBUG: void give_priority()] give_priority_index = %d\n", give_priority_index);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].position = %d\n", operator_tokens[give_priority_index].position);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].priority = %d\n", operator_tokens[give_priority_index].priority);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].regex = \"%s\"\n", operator_tokens[give_priority_index].regex);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].token_type = %d\n", operator_tokens[give_priority_index].token_type);
        }
        break;
      }
      case TK_DIVIDE:
      {
        // Current (2nd version) Precidency Level: (No Parentheses Considered)
        // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
        // Precidency Level: 6
        operator_tokens[give_priority_index].priority = 6;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_priority()] found a TK_DIVIDE operator, set priority to 6\n");
          printf("[NEMU_EXPR_DEBUG: void give_priority()] give_priority_index = %d\n", give_priority_index);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].position = %d\n", operator_tokens[give_priority_index].position);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].priority = %d\n", operator_tokens[give_priority_index].priority);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].regex = \"%s\"\n", operator_tokens[give_priority_index].regex);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].token_type = %d\n", operator_tokens[give_priority_index].token_type);
        }
        break;
      }
      case TK_MOD:
      {
        // Current (2nd version) Precidency Level: (No Parentheses Considered)
        // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
        // Precidency Level: 6
        operator_tokens[give_priority_index].priority = 6;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_priority()] found a TK_MOD operator, set priority to 6\n");
          printf("[NEMU_EXPR_DEBUG: void give_priority()] give_priority_index = %d\n", give_priority_index);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].position = %d\n", operator_tokens[give_priority_index].position);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].priority = %d\n", operator_tokens[give_priority_index].priority);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].regex = \"%s\"\n", operator_tokens[give_priority_index].regex);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].token_type = %d\n", operator_tokens[give_priority_index].token_type);
        }
        break;
      }
      case TK_NOT:
      {
        // Current (2nd version) Precidency Level: (No Parentheses Considered)
        // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
        // Precidency Level: 7
        operator_tokens[give_priority_index].priority = 7;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_priority()] found a TK_NOT operator, set priority to 7\n");
          printf("[NEMU_EXPR_DEBUG: void give_priority()] give_priority_index = %d\n", give_priority_index);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].position = %d\n", operator_tokens[give_priority_index].position);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].priority = %d\n", operator_tokens[give_priority_index].priority);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].regex = \"%s\"\n", operator_tokens[give_priority_index].regex);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].token_type = %d\n", operator_tokens[give_priority_index].token_type);
        }
        break;
      }
      case TK_POSITIVESIGN:
      {
        // Current (2nd version) Precidency Level: (No Parentheses Considered)
        // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
        // Precidency Level: 7
        operator_tokens[give_priority_index].priority = 7;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_priority()] found a TK_POSITIVESIGN operator, set priority to 7\n");
          printf("[NEMU_EXPR_DEBUG: void give_priority()] give_priority_index = %d\n", give_priority_index);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].position = %d\n", operator_tokens[give_priority_index].position);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].priority = %d\n", operator_tokens[give_priority_index].priority);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].regex = \"%s\"\n", operator_tokens[give_priority_index].regex);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].token_type = %d\n", operator_tokens[give_priority_index].token_type);
        }
        break;
      }
      case TK_NEGATIVESIGN:
      {
        // Current (2nd version) Precidency Level: (No Parentheses Considered)
        // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
        // Precidency Level: 7
        operator_tokens[give_priority_index].priority = 7;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_priority()] found a TK_NEGATIVESIGN operator, set priority to 7\n");
          printf("[NEMU_EXPR_DEBUG: void give_priority()] give_priority_index = %d\n", give_priority_index);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].position = %d\n", operator_tokens[give_priority_index].position);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].priority = %d\n", operator_tokens[give_priority_index].priority);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].regex = \"%s\"\n", operator_tokens[give_priority_index].regex);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].token_type = %d\n", operator_tokens[give_priority_index].token_type);
        }
        break;
      }
      case TK_POINTER:
      {
        // Current (2nd version) Precidency Level: (No Parentheses Considered)
        // 1) OR 2) EXOR 3) AND 4) EQ/NEQ 5) PLUS/MINUS 6) MULTIPLY/DEVIDE/MOD 7) NOT/POSITIVESIGN/NEGATIVESIGN 8) POINTER
        // Precidency Level: 7
        operator_tokens[give_priority_index].priority = 8;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_priority()] found a TK_POINTER operator, set priority to 8\n");
          printf("[NEMU_EXPR_DEBUG: void give_priority()] give_priority_index = %d\n", give_priority_index);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].position = %d\n", operator_tokens[give_priority_index].position);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].priority = %d\n", operator_tokens[give_priority_index].priority);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].regex = \"%s\"\n", operator_tokens[give_priority_index].regex);
          printf("[NEMU_EXPR_DEBUG: void give_priority()] operator_tokens[give_priority_index].token_type = %d\n", operator_tokens[give_priority_index].token_type);
        }
        break;
      }
    }
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void give_priority()] nr_operator_token = %d\n", nr_operator_token);
  }
  for(int operator_token_print_index = 0; operator_token_print_index < nr_operator_token; operator_token_print_index = operator_token_print_index + 1)
  {
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: void give_priority()] Operator Token Number: %4d, Position: %4d, Token Type: %4d, Token String: \"%s\", Priority: %4d\n", operator_token_print_index, operator_tokens[operator_token_print_index].position, operator_tokens[operator_token_print_index].token_type, operator_tokens[operator_token_print_index].regex, operator_tokens[operator_token_print_index].priority);
    }
  }
  return;
}

void give_priority_no_parentheses()
{
  // In function give_priority() we just give pripority ignoring the parentheses, now we need to add them back
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: void give_priority_no_parentheses()] CKPT #01: Enter function\n");
  }
  int local_highest_priority = 0;
  for(int current_scan_local_highest_priority_index = 1; current_scan_local_highest_priority_index < nr_operator_token - 1; current_scan_local_highest_priority_index = current_scan_local_highest_priority_index + 1)
  {
    if(expr_print_checkpoint)
    {
      printf("[NEMU_EXPR_CHECKPOINT: void give_priority_no_parentheses()] CKPT #02: Enter scan local highest priority loop\n");
    }
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] enter loop, current_scan_local_highest_priority_index = %d\n", current_scan_local_highest_priority_index);
      printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] enter loop, current local_highest_priority = %d\n", local_highest_priority);
    }
    if(operator_tokens[current_scan_local_highest_priority_index].priority > local_highest_priority)
    {
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] at index %4d, find priority %4d, higher than local_highest_priority %4d\n", current_scan_local_highest_priority_index, operator_tokens[current_scan_local_highest_priority_index].priority , local_highest_priority);
      }
      local_highest_priority = operator_tokens[current_scan_local_highest_priority_index].priority;
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] set local_highest_priority to %4d\n", local_highest_priority);
      }
    }
  }
  if(local_highest_priority < 0)
  {
    // Invalid
    if(expr_print_assertpoint)
    {
      printf("[NEMU_EXPR_ASSERTPOINT: void give_priority_no_parentheses()] ASPT #01: local_highest_priority < 0\n");
    }
    assert(0);
  }
  if(!check_parentheses_valid())
  {
    // Invalid
    if(expr_print_assertpoint)
    {
      printf("[NEMU_EXPR_ASSERTPOINT: void give_priority_no_parentheses()] ASPT #02: !check_parentheses_valid()\n");
    }
    assert(0);
  }
  int give_priority_no_parentheses_parentheses_level = 0;
  for(int give_priority_no_parentheses_increase_priority_index = 0; give_priority_no_parentheses_increase_priority_index < nr_operator_token; give_priority_no_parentheses_increase_priority_index = give_priority_no_parentheses_increase_priority_index + 1)
  {
    if(expr_print_checkpoint)
    {
      printf("[NEMU_EXPR_CHECKPOINT: void give_priority_no_parentheses()] CKPT #03: Enter increase priority loop\n");
    }
    if(operator_tokens[give_priority_no_parentheses_increase_priority_index].token_type == TK_LEFT_PARENTHESES)
    {
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] Find a TK_LEFT_PARENTHESES at index %d\n", give_priority_no_parentheses_increase_priority_index);
        printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] Previous give_priority_no_parentheses_parentheses_level = %d, Current give_priority_no_parentheses_parentheses_level = %d\n", give_priority_no_parentheses_parentheses_level, give_priority_no_parentheses_parentheses_level + 1);
      }
      give_priority_no_parentheses_parentheses_level = give_priority_no_parentheses_parentheses_level + 1;
    }
    if(operator_tokens[give_priority_no_parentheses_increase_priority_index].token_type == TK_RIGHT_PARENTHESES)
    {
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] Find a TK_RIGHT_PARENTHESES at index %d\n", give_priority_no_parentheses_increase_priority_index);
        printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] Previous give_priority_no_parentheses_parentheses_level = %d, Current give_priority_no_parentheses_parentheses_level = %d\n", give_priority_no_parentheses_parentheses_level, give_priority_no_parentheses_parentheses_level - 1);
      }
      give_priority_no_parentheses_parentheses_level = give_priority_no_parentheses_parentheses_level - 1;
    }
    if(operator_tokens[give_priority_no_parentheses_increase_priority_index].token_type != TK_LEFT_PARENTHESES && operator_tokens[give_priority_no_parentheses_increase_priority_index].token_type != TK_RIGHT_PARENTHESES)
    {
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] Find a NON_PARENTHESES_TOKEN at index %d\n", give_priority_no_parentheses_increase_priority_index);
        printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] Original priority: %d, new priority: %d\n", operator_tokens[give_priority_no_parentheses_increase_priority_index].priority, operator_tokens[give_priority_no_parentheses_increase_priority_index].priority + give_priority_no_parentheses_parentheses_level * local_highest_priority);
      }
      operator_tokens[give_priority_no_parentheses_increase_priority_index].priority = operator_tokens[give_priority_no_parentheses_increase_priority_index].priority + give_priority_no_parentheses_parentheses_level * local_highest_priority;
      if(expr_print_checkpoint)
      {
        printf("[NEMU_EXPR_CHECKPOINT: void give_priority_no_parentheses()] CKPT #04: Copy information about non_parentheses tokens to operator_tokens_no_parentheses[32]\n");
      }
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] operator_tokens_no_parentheses[%d].position = %d\n", nr_operator_tokens_no_parentheses, operator_tokens[give_priority_no_parentheses_increase_priority_index].position);
        printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] operator_tokens_no_parentheses[%d].priority_level = %d\n", nr_operator_tokens_no_parentheses, operator_tokens[give_priority_no_parentheses_increase_priority_index].priority);
        printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] operator_tokens_no_parentheses[%d].regex = \"%s\"\n", nr_operator_tokens_no_parentheses, operator_tokens[give_priority_no_parentheses_increase_priority_index].regex);
        printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] operator_tokens_no_parentheses[%d].token_type = %d\n", nr_operator_tokens_no_parentheses, operator_tokens[give_priority_no_parentheses_increase_priority_index].token_type);
        printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] nr_operator_tokens_no_parentheses = %d\n", nr_operator_tokens_no_parentheses + 1);
      }
      operator_tokens_no_parentheses[nr_operator_tokens_no_parentheses].position = operator_tokens[give_priority_no_parentheses_increase_priority_index].position;
      operator_tokens_no_parentheses[nr_operator_tokens_no_parentheses].priority_level = operator_tokens[give_priority_no_parentheses_increase_priority_index].priority;
      operator_tokens_no_parentheses[nr_operator_tokens_no_parentheses].regex = operator_tokens[give_priority_no_parentheses_increase_priority_index].regex;
      operator_tokens_no_parentheses[nr_operator_tokens_no_parentheses].token_type = operator_tokens[give_priority_no_parentheses_increase_priority_index].token_type;
      nr_operator_tokens_no_parentheses = nr_operator_tokens_no_parentheses + 1;
    }
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] nr_operator_token = %d\n", nr_operator_token);
    for(int print_operator_token_index = 0; print_operator_token_index < nr_operator_token; print_operator_token_index = print_operator_token_index + 1)
    {
      printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] Token Number: %2d, Position: %2d, Priority: %2d, Token String: \"%s\", Token Type: %d\n", print_operator_token_index, operator_tokens[print_operator_token_index].position, operator_tokens[print_operator_token_index].priority, operator_tokens[print_operator_token_index].regex, operator_tokens[print_operator_token_index].token_type);
    }
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] nr_operator_tokens_no_parentheses = %d\n", nr_operator_tokens_no_parentheses);
    for(int print_operator_token_no_parentheses_index = 0; print_operator_token_no_parentheses_index < nr_operator_tokens_no_parentheses; print_operator_token_no_parentheses_index = print_operator_token_no_parentheses_index + 1)
    {
      printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] Token Number: %2d, Position: %2d, Priority: %2d, Token String: \"%s\", Token Type: %d\n", print_operator_token_no_parentheses_index, operator_tokens_no_parentheses[print_operator_token_no_parentheses_index].position, operator_tokens_no_parentheses[print_operator_token_no_parentheses_index].priority_level, operator_tokens_no_parentheses[print_operator_token_no_parentheses_index].regex, operator_tokens_no_parentheses[print_operator_token_no_parentheses_index].token_type);
    }
  }
  return;
}

void give_sub_priority()
{
  // Test Token for this function: p 1 + (2 + (3 + 4) + (5 + 6 + (7 * 8 * 9) + 10) * 11 + 12)
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: void give_sub_priority()] CKPT #01: Enter function\n");
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] nr_operator_token = %d\n", nr_operator_token);
    for(int print_operator_token_index = 0; print_operator_token_index < nr_operator_token; print_operator_token_index = print_operator_token_index + 1)
    {
      printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] Token Number: %2d, Position: %2d, Priority: %2d, Token String: \"%s\", Token Type: %d\n", print_operator_token_index, operator_tokens[print_operator_token_index].position, operator_tokens[print_operator_token_index].priority, operator_tokens[print_operator_token_index].regex, operator_tokens[print_operator_token_index].token_type);
    }
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] nr_operator_tokens_no_parentheses = %d\n", nr_operator_tokens_no_parentheses);
    for(int print_operator_token_no_parentheses_index = 0; print_operator_token_no_parentheses_index < nr_operator_tokens_no_parentheses; print_operator_token_no_parentheses_index = print_operator_token_no_parentheses_index + 1)
    {
      printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] Token Number: %2d, Position: %2d, Priority: %2d, Sub-Priority: %2d, Token String: \"%s\", Token Type: %d\n", print_operator_token_no_parentheses_index, operator_tokens_no_parentheses[print_operator_token_no_parentheses_index].position, operator_tokens_no_parentheses[print_operator_token_no_parentheses_index].priority_level, operator_tokens_no_parentheses[print_operator_token_no_parentheses_index].sub_priority_level, operator_tokens_no_parentheses[print_operator_token_no_parentheses_index].regex, operator_tokens_no_parentheses[print_operator_token_no_parentheses_index].token_type);
    }
  }
  // Now we give sub calculation priority by considering the position of operators
  int global_highest_priority = -1;
  for(int find_global_highest_priority_index = 0; find_global_highest_priority_index < nr_operator_tokens_no_parentheses; find_global_highest_priority_index = find_global_highest_priority_index + 1)
  {
    if(expr_print_checkpoint)
    {
      printf("[NEMU_EXPR_CHECKPOINT: void give_sub_priority()] CKPT #02: find_global_highest_priority loop\n");
    }
    if(operator_tokens_no_parentheses[find_global_highest_priority_index].priority_level > global_highest_priority)
    {
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void give_sub_priority()] At find_global_highest_priority_index = %d, find priority_level = %d, higher than current global_highest_priority = %d\n", find_global_highest_priority_index, operator_tokens_no_parentheses[find_global_highest_priority_index].priority_level, global_highest_priority);
      }
      global_highest_priority = operator_tokens_no_parentheses[find_global_highest_priority_index].priority_level;
    }
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void give_sub_priority()] global_highest_priority = %d\n", global_highest_priority);
  }
  for(int current_processing_global_priority = global_highest_priority; current_processing_global_priority > 0; current_processing_global_priority = current_processing_global_priority - 1)
  {
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: void give_sub_priority()] current_processing_global_priority = %d\n", current_processing_global_priority);
    }
    int current_processing_local_priority = 0;
    for(int current_scanning_operator_tokens_no_parentheses_index = 0; current_scanning_operator_tokens_no_parentheses_index < nr_operator_tokens_no_parentheses; current_scanning_operator_tokens_no_parentheses_index = current_scanning_operator_tokens_no_parentheses_index + 1)
    {
      // First, we need to find out how many operators have the same global priority, by scanning each operator_token_no_parentheses and we count them
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void give_sub_priority()] current_processing_global_priority = %d\n", current_processing_global_priority);
        printf("[NEMU_EXPR_DEBUG: void give_sub_priority()] current_processing_local_priority = %d\n", current_processing_local_priority);
      }
      if(operator_tokens_no_parentheses[current_scanning_operator_tokens_no_parentheses_index].priority_level == current_processing_global_priority)
      {
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_sub_priority()] switch operator_tokens_no_parentheses from %d to %d\n", current_processing_local_priority, current_processing_local_priority + 1);
        }
        current_processing_local_priority = current_processing_local_priority + 1;
      }
    }
    for(int current_scanning_operator_tokens_no_parentheses_index = 0; current_scanning_operator_tokens_no_parentheses_index < nr_operator_tokens_no_parentheses; current_scanning_operator_tokens_no_parentheses_index = current_scanning_operator_tokens_no_parentheses_index + 1)
    {
      // Second, we give the highest local priority to the first operator token with the same global priority, then we minus 1 to current_processing_local_priority
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void give_sub_priority()] current_processing_global_priority = %d\n", current_processing_global_priority);
        printf("[NEMU_EXPR_DEBUG: void give_sub_priority()] current_processing_local_priority = %d\n", current_processing_local_priority);
      }
      if(operator_tokens_no_parentheses[current_scanning_operator_tokens_no_parentheses_index].priority_level == current_processing_global_priority)
      {
        operator_tokens_no_parentheses[current_scanning_operator_tokens_no_parentheses_index].sub_priority_level = current_processing_local_priority;
        current_processing_local_priority = current_processing_local_priority - 1;
        if(expr_print_debug)
        {
          printf("[NEMU_EXPR_DEBUG: void give_sub_priority()] operator_tokens_no_parentheses[%d].sub_priority_level = %d\n", current_scanning_operator_tokens_no_parentheses_index, current_processing_local_priority);
        }
      }
    }
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] nr_operator_tokens_no_parentheses = %d\n", nr_operator_tokens_no_parentheses);
    for(int print_operator_token_no_parentheses_index = 0; print_operator_token_no_parentheses_index < nr_operator_tokens_no_parentheses; print_operator_token_no_parentheses_index = print_operator_token_no_parentheses_index + 1)
    {
      printf("[NEMU_EXPR_DEBUG: void give_priority_no_parentheses()] Token Number: %2d, Position: %2d, Priority: %2d, Sub-Priority: %2d, Token String: \"%s\", Token Type: %d\n", print_operator_token_no_parentheses_index, operator_tokens_no_parentheses[print_operator_token_no_parentheses_index].position, operator_tokens_no_parentheses[print_operator_token_no_parentheses_index].priority_level, operator_tokens_no_parentheses[print_operator_token_no_parentheses_index].sub_priority_level, operator_tokens_no_parentheses[print_operator_token_no_parentheses_index].regex, operator_tokens_no_parentheses[print_operator_token_no_parentheses_index].token_type);
    }
  }
  return;
}

int bool_to_int(bool bool_value)
{
  // We received a bool value, convert it to 0 or 1
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int bool_to_int(bool bool_value)] CKPT #01: Enter function\n");
  }
  if(bool_value)
  {
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: int bool_to_int(bool bool_value)] Get bool_value = true, return 1\n");
    }
    return 1;
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int bool_to_int(bool bool_value)] Get bool_value = false, return 0\n");
  }
  return 0;
}

bool check_left_token_is_number_or_bool(int check_index)
{
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: bool check_left_token_is_number_or_bool(int check_index)] CKPT #01: Enter function\n");
  }
  if(tokens[check_index - 1].type == TK_NUMBER || tokens[check_index - 1].type == TK_HEXNUMBER)
  {
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: bool check_left_token_is_number_or_bool(int check_index)] Token at %d (left) is one of the NUMBER type\n", check_index - 1);
    }
    return true;
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: bool check_left_token_is_number_or_bool(int check_index)] Token at %d (left) is NOT one of the NUMBER type\n", check_index - 1);
  }
  return false;
}

bool check_right_token_is_number_or_bool(int check_index)
{
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: bool check_right_token_is_number_or_bool(int check_index)] CKPT #01: Enter function\n");
  }
  if(tokens[check_index + 1].type == TK_NUMBER || tokens[check_index + 1].type == TK_HEXNUMBER)
  {
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: bool check_right_token_is_number_or_bool(int check_index)] Token at %d (right) is one of the NUMBER type\n", check_index - 1);
    }
    return true;
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: bool check_right_token_is_number_or_bool(int check_index)] Token at %d (right) is NOT one of the NUMBER type\n", check_index - 1);
  }
  return false;
}

int process_add(int add_operator_index)
{
  // We recognized the add_operator, evaluate it
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int process_add(int add_operator_index)] CKPT #01: Enter function\n");
  }
  int process_add_answer = 0;
  int left_token_index = add_operator_index - 1;
  int right_token_index = add_operator_index + 1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_add(int add_operator_index)] add_operator_index = %d\n", add_operator_index);
    printf("[NEMU_EXPR_DEBUG: int process_add(int add_operator_index)] left_token_index = %d\n", left_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_add(int add_operator_index)] left token type : %d\n", tokens[left_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_add(int add_operator_index)] left token string : \"%s\"\n", tokens[left_token_index].str);
    printf("[NEMU_EXPR_DEBUG: int process_add(int add_operator_index)] right_token_index = %d\n", right_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_add(int add_operator_index)] right token type : %d\n", tokens[right_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_add(int add_operator_index)] right token string : \"%s\"\n", tokens[right_token_index].str);
  }
  int left_token_int_value = atoi(tokens[left_token_index].str);
  int right_token_int_value = atoi(tokens[right_token_index].str);
  process_add_answer = left_token_int_value + right_token_int_value;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_add(int add_operator_index)] left_token_int_value = %d\n", left_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_add(int add_operator_index)] right_token_int_value = %d\n", right_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_add(int add_operator_index)] process_add_answer = %d\n", process_add_answer);
  }
  return process_add_answer;
}

int process_minus(int minus_operator_index)
{
  // We recognized the minus_operator, evaluate it
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int process_minus(int minus_operator_index)] CKPT #01: Enter function\n");
  }
  int process_minus_answer = 0;
  int left_token_index = minus_operator_index - 1;
  int right_token_index = minus_operator_index + 1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_minus(int minus_operator_index)] minus_operator_index = %d\n", minus_operator_index);
    printf("[NEMU_EXPR_DEBUG: int process_minus(int minus_operator_index)] left_token_index = %d\n", left_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_minus(int minus_operator_index)] left token type : %d\n", tokens[left_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_minus(int minus_operator_index)] left token string : \"%s\"\n", tokens[left_token_index].str);
    printf("[NEMU_EXPR_DEBUG: int process_minus(int minus_operator_index)] right_token_index = %d\n", right_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_minus(int minus_operator_index)] right token type : %d\n", tokens[right_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_minus(int minus_operator_index)] right token string : \"%s\"\n", tokens[right_token_index].str);
  }
  int left_token_int_value = atoi(tokens[left_token_index].str);
  int right_token_int_value = atoi(tokens[right_token_index].str);
  process_minus_answer = left_token_int_value - right_token_int_value;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_minus(int minus_operator_index)] left_token_int_value = %d\n", left_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_minus(int minus_operator_index)] right_token_int_value = %d\n", right_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_minus(int minus_operator_index)] process_minus_answer = %d\n", process_minus_answer);
  }
  return process_minus_answer;
}

int process_multiply(int multiply_operator_index)
{
  // We recognized the multiply_operator, evaluate it
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int process_multiply(int multiply_operator_index)] CKPT #01: Enter function\n");
  }
  int process_multiply_answer = 0;
  int left_token_index = multiply_operator_index - 1;
  int right_token_index = multiply_operator_index + 1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_multiply(int multiply_operator_index)] multiply_operator_index = %d\n", multiply_operator_index);
    printf("[NEMU_EXPR_DEBUG: int process_multiply(int multiply_operator_index)] left_token_index = %d\n", left_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_multiply(int multiply_operator_index)] left token type : %d\n", tokens[left_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_multiply(int multiply_operator_index)] left token string : \"%s\"\n", tokens[left_token_index].str);
    printf("[NEMU_EXPR_DEBUG: int process_multiply(int multiply_operator_index)] right_token_index = %d\n", right_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_multiply(int multiply_operator_index)] right token type : %d\n", tokens[right_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_multiply(int multiply_operator_index)] right token string : \"%s\"\n", tokens[right_token_index].str);
  }
  int left_token_int_value = atoi(tokens[left_token_index].str);
  int right_token_int_value = atoi(tokens[right_token_index].str);
  process_multiply_answer = left_token_int_value * right_token_int_value;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_multiply(int multiply_operator_index)] left_token_int_value = %d\n", left_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_multiply(int multiply_operator_index)] right_token_int_value = %d\n", right_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_multiply(int multiply_operator_index)] process_multiply_answer = %d\n", process_multiply_answer);
  }
  return process_multiply_answer;
}

int process_devide(int devide_operator_index)
{
  // We recognized the devide_operator, evaluate it
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int process_devide(int devide_operator_index)] CKPT #01: Enter function\n");
  }
  int process_devide_answer = 0;
  int left_token_index = devide_operator_index - 1;
  int right_token_index = devide_operator_index + 1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_devide(int devide_operator_index)] devide_operator_index = %d\n", devide_operator_index);
    printf("[NEMU_EXPR_DEBUG: int process_devide(int devide_operator_index)] left_token_index = %d\n", left_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_devide(int devide_operator_index)] left token type : %d\n", tokens[left_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_devide(int devide_operator_index)] left token string : \"%s\"\n", tokens[left_token_index].str);
    printf("[NEMU_EXPR_DEBUG: int process_devide(int devide_operator_index)] right_token_index = %d\n", right_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_devide(int devide_operator_index)] right token type : %d\n", tokens[right_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_devide(int devide_operator_index)] right token string : \"%s\"\n", tokens[right_token_index].str);
  }
  int left_token_int_value = atoi(tokens[left_token_index].str);
  int right_token_int_value = atoi(tokens[right_token_index].str);
  process_devide_answer = left_token_int_value / right_token_int_value;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_devide(int devide_operator_index)] left_token_int_value = %d\n", left_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_devide(int devide_operator_index)] right_token_int_value = %d\n", right_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_devide(int devide_operator_index)] process_devide_answer = %d\n", process_devide_answer);
  }
  return process_devide_answer;
}

int process_mod(int mod_operator_index)
{
  // We recognized the mod_operator, evaluate it
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int process_mod(int mod_operator_index)] CKPT #01: Enter function\n");
  }
  int process_mod_answer = 0;
  int left_token_index = mod_operator_index - 1;
  int right_token_index = mod_operator_index + 1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_mod(int mod_operator_index)] mod_operator_index = %d\n", mod_operator_index);
    printf("[NEMU_EXPR_DEBUG: int process_mod(int mod_operator_index)] left_token_index = %d\n", left_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_mod(int mod_operator_index)] left token type : %d\n", tokens[left_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_mod(int mod_operator_index)] left token string : \"%s\"\n", tokens[left_token_index].str);
    printf("[NEMU_EXPR_DEBUG: int process_mod(int mod_operator_index)] right_token_index = %d\n", right_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_mod(int mod_operator_index)] right token type : %d\n", tokens[right_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_mod(int mod_operator_index)] right token string : \"%s\"\n", tokens[right_token_index].str);
  }
  int left_token_int_value = atoi(tokens[left_token_index].str);
  int right_token_int_value = atoi(tokens[right_token_index].str);
  int integer_devide_answer = left_token_int_value / right_token_int_value;
  process_mod_answer = left_token_int_value - integer_devide_answer * right_token_int_value;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_mod(int mod_operator_index)] left_token_int_value = %d\n", left_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_mod(int mod_operator_index)] right_token_int_value = %d\n", right_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_mod(int mod_operator_index)] integer_devide_answer = %d\n", integer_devide_answer);
    printf("[NEMU_EXPR_DEBUG: int process_mod(int mod_operator_index)] process_devide_answer = %d\n", process_mod_answer);
  }
  return process_mod_answer;
}

int process_equal(int equal_operator_index)
{
  // We recognized the equal_operator, evaluate it
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int process_equal(int equal_operator_index)] CKPT #01: Enter function\n");
  }
  int process_equal_answer = 0;
  int left_token_index = equal_operator_index - 1;
  int right_token_index = equal_operator_index + 1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_equal(int equal_operator_index)] equal_operator_index = %d\n", equal_operator_index);
    printf("[NEMU_EXPR_DEBUG: int process_equal(int equal_operator_index)] left_token_index = %d\n", left_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_equal(int equal_operator_index)] left token type : %d\n", tokens[left_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_equal(int equal_operator_index)] left token string : \"%s\"\n", tokens[left_token_index].str);
    printf("[NEMU_EXPR_DEBUG: int process_equal(int equal_operator_index)] right_token_index = %d\n", right_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_equal(int equal_operator_index)] right token type : %d\n", tokens[right_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_equal(int equal_operator_index)] right token string : \"%s\"\n", tokens[right_token_index].str);
  }
  int left_token_int_value = atoi(tokens[left_token_index].str);
  int right_token_int_value = atoi(tokens[right_token_index].str);
  if(left_token_int_value == right_token_int_value)
  {
    process_equal_answer = 1;
  }
  else
  {
    process_equal_answer = 0;
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_equal(int equal_operator_index)] left_token_int_value = %d\n", left_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_equal(int equal_operator_index)] right_token_int_value = %d\n", right_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_equal(int equal_operator_index)] process_equal_answer = %d\n", process_equal_answer);
  }
  return process_equal_answer;
}

int process_not_equal(int not_equal_operator_index)
{
  // We recognized the not_equal_operator, evaluate it
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int process_not_equal(int not_equal_operator_index)] CKPT #01: Enter function\n");
  }
  int process_not_equal_answer = 0;
  int left_token_index = not_equal_operator_index - 1;
  int right_token_index = not_equal_operator_index + 1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_not_equal(int not_equal_operator_index)] not_operator_index = %d\n", not_equal_operator_index);
    printf("[NEMU_EXPR_DEBUG: int process_not_equal(int not_equal_operator_index)] left_token_index = %d\n", left_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_not_equal(int not_equal_operator_index)] left token type : %d\n", tokens[left_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_not_equal(int not_equal_operator_index)] left token string : \"%s\"\n", tokens[left_token_index].str);
    printf("[NEMU_EXPR_DEBUG: int process_not_equal(int not_equal_operator_index)] right_token_index = %d\n", right_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_not_equal(int not_equal_operator_index)] right token type : %d\n", tokens[right_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_not_equal(int not_equal_operator_index)] right token string : \"%s\"\n", tokens[right_token_index].str);
  }
  int left_token_int_value = atoi(tokens[left_token_index].str);
  int right_token_int_value = atoi(tokens[right_token_index].str);
  if(left_token_int_value != right_token_int_value)
  {
    process_not_equal_answer = 1;
  }
  else
  {
    process_not_equal_answer = 0;
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_not_equal(int not_equal_operator_index)] left_token_int_value = %d\n", left_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_not_equal(int not_equal_operator_index)] right_token_int_value = %d\n", right_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_not_equal(int not_equal_operator_index)] process_not_equal_answer = %d\n", process_not_equal_answer);
  }
  return process_not_equal_answer;
}

int process_and(int and_operator_index)
{
  // We recognized the and_operator, evaluate it
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int process_and(int and_operator_index)] CKPT #01: Enter function\n");
  }
  int process_and_answer = 0;
  int left_token_index = and_operator_index - 1;
  int right_token_index = and_operator_index + 1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_and(int and_operator_index)] and_operator_index = %d\n", and_operator_index);
    printf("[NEMU_EXPR_DEBUG: int process_and(int and_operator_index)] left_token_index = %d\n", left_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_and(int and_operator_index)] left token type : %d\n", tokens[left_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_and(int and_operator_index)] left token string : \"%s\"\n", tokens[left_token_index].str);
    printf("[NEMU_EXPR_DEBUG: int process_and(int and_operator_index)] right_token_index = %d\n", right_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_and(int and_operator_index)] right token type : %d\n", tokens[right_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_and(int and_operator_index)] right token string : \"%s\"\n", tokens[right_token_index].str);
  }
  int left_token_int_value = atoi(tokens[left_token_index].str);
  int right_token_int_value = atoi(tokens[right_token_index].str);
  process_and_answer = left_token_int_value & right_token_int_value;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_and(int and_operator_index)] left_token_int_value = %d\n", left_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_and(int and_operator_index)] right_token_int_value = %d\n", right_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_and(int and_operator_index)] process_and_answer = %d\n", process_and_answer);
  }
  return process_and_answer;
}

int process_or(int or_operator_index)
{
  // We recognized the or_operator, evaluate it
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int process_or(int or_operator_index)] CKPT #01: Enter function\n");
  }
  int process_or_answer = 0;
  int left_token_index = or_operator_index - 1;
  int right_token_index = or_operator_index + 1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_or(int or_operator_index)] or_operator_index = %d\n", or_operator_index);
    printf("[NEMU_EXPR_DEBUG: int process_or(int or_operator_index)] left_token_index = %d\n", left_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_or(int or_operator_index)] left token type : %d\n", tokens[left_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_or(int or_operator_index)] left token string : \"%s\"\n", tokens[left_token_index].str);
    printf("[NEMU_EXPR_DEBUG: int process_or(int or_operator_index)] right_token_index = %d\n", right_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_or(int or_operator_index)] right token type : %d\n", tokens[right_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_or(int or_operator_index)] right token string : \"%s\"\n", tokens[right_token_index].str);
  }
  int left_token_int_value = atoi(tokens[left_token_index].str);
  int right_token_int_value = atoi(tokens[right_token_index].str);
  process_or_answer = left_token_int_value | right_token_int_value;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_or(int or_operator_index)] left_token_int_value = %d\n", left_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_or(int or_operator_index)] right_token_int_value = %d\n", right_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_or(int or_operator_index)] process_or_answer = %d\n", process_or_answer);
  }
  return process_or_answer;
}

int process_exor(int exor_operator_index)
{
  // We recognized the exor_operator, evaluate it
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int process_exor(int exor_operator_index)] CKPT #01: Enter function\n");
  }
  int process_exor_answer = 0;
  int left_token_index = exor_operator_index - 1;
  int right_token_index = exor_operator_index + 1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_exor(int exor_operator_index)] exor_operator_index = %d\n", exor_operator_index);
    printf("[NEMU_EXPR_DEBUG: int process_exor(int exor_operator_index)] left_token_index = %d\n", left_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_exor(int exor_operator_index)] left token type : %d\n", tokens[left_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_exor(int exor_operator_index)] left token string : \"%s\"\n", tokens[left_token_index].str);
    printf("[NEMU_EXPR_DEBUG: int process_exor(int exor_operator_index)] right_token_index = %d\n", right_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_exor(int exor_operator_index)] right token type : %d\n", tokens[right_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_exor(int exor_operator_index)] right token string : \"%s\"\n", tokens[right_token_index].str);
  }
  int left_token_int_value = atoi(tokens[left_token_index].str);
  int right_token_int_value = atoi(tokens[right_token_index].str);
  process_exor_answer = left_token_int_value ^ right_token_int_value;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_exor(int exor_operator_index)] left_token_int_value = %d\n", left_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_exor(int exor_operator_index)] right_token_int_value = %d\n", right_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_exor(int exor_operator_index)] process_exor_answer = %d\n", process_exor_answer);
  }
  return process_exor_answer;
}

int process_not(int not_operator_index)
{
  // We recognized the not_operator, evaluate it
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int process_not(int not_operator_index)] CKPT #01: Enter function\n");
  }
  int process_not_answer = 0;
  int right_token_index = not_operator_index + 1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_not(int not_operator_index)] not_operator_index = %d\n", not_operator_index);
    printf("[NEMU_EXPR_DEBUG: int process_not(int not_operator_index)] right_token_index = %d\n", right_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_not(int not_operator_index)] right token type : %d\n", tokens[right_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_not(int not_operator_index)] right token string : \"%s\"\n", tokens[right_token_index].str);
  }
  int right_token_int_value = atoi(tokens[right_token_index].str);
  process_not_answer = ~ right_token_int_value;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_not(int not_operator_index)] right_token_int_value = %d\n", right_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_not(int not_operator_index)] process_or_answer = %d\n", process_not_answer);
  }
  return process_not_answer;
}

int process_positive_sign(int positive_sign_operator_index)
{
  // We recognized the positive_sign_operator, evaluate it
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int process_positive_sign(int positive_sign_operator_index)] CKPT #01: Enter function\n");
  }
  int process_positive_sign_answer = 0;
  int right_token_index = positive_sign_operator_index + 1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_positive_sign(int positive_sign_operator_index)] positive_sign_operator_index = %d\n", positive_sign_operator_index);
    printf("[NEMU_EXPR_DEBUG: int process_positive_sign(int positive_sign_operator_index)] right_token_index = %d\n", right_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_positive_sign(int positive_sign_operator_index)] right token type : %d\n", tokens[right_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_positive_sign(int positive_sign_operator_index)] right token string : \"%s\"\n", tokens[right_token_index].str);
  }
  int right_token_int_value = atoi(tokens[right_token_index].str);
  process_positive_sign_answer = 1 * right_token_int_value;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_positive_sign(int positive_sign_operator_index)] right_token_int_value = %d\n", right_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_positive_sign(int positive_sign_operator_index)] process_or_answer = %d\n", process_positive_sign_answer);
  }
  return process_positive_sign_answer;
}

int process_negative_sign(int negative_sign_operator_index)
{
  // We recognized the negative_sign_operator, evaluate it
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int process_negative_sign(int negative_sign_operator_index)] CKPT #01: Enter function\n");
  }
  int process_negative_sign_answer = 0;
  int right_token_index = negative_sign_operator_index + 1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_negative_sign(int negative_sign_operator_index)] negative_sign_operator_index = %d\n", negative_sign_operator_index);
    printf("[NEMU_EXPR_DEBUG: int process_negative_sign(int negative_sign_operator_index)] right_token_index = %d\n", right_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_negative_sign(int negative_sign_operator_index)] right token type : %d\n", tokens[right_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_negative_sign(int negative_sign_operator_index)] right token string : \"%s\"\n", tokens[right_token_index].str);
  }
  int right_token_int_value = atoi(tokens[right_token_index].str);
  process_negative_sign_answer = -1 * right_token_int_value;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_negative_sign(int negative_sign_operator_index)] right_token_int_value = %d\n", right_token_int_value);
    printf("[NEMU_EXPR_DEBUG: int process_negative_sign(int negative_sign_operator_index)] process_or_answer = %d\n", process_negative_sign_answer);
  }
  return process_negative_sign_answer;
}

int process_pointer_dereference(int pointer_dereference_index)
{
  // We recognized the pointer_dereference_operator, evaluate it
  // Special Case, will change the code skeleton
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: int process_pointer_dereference(int pointer_dereference_index)] CKPT #01: Enter function\n");
  }
  int process_pointer_dereference_answer = 0;
  int right_token_index = pointer_dereference_index + 1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: int process_pointer_dereference(int pointer_dereference_index)] negative_sign_operator_index = %d\n", pointer_dereference_index);
    printf("[NEMU_EXPR_DEBUG: int process_pointer_dereference(int pointer_dereference_index)] right_token_index = %d\n", right_token_index);
    printf("[NEMU_EXPR_DEBUG: int process_pointer_dereference(int pointer_dereference_index)] right token type : %d\n", tokens[right_token_index].type);
    printf("[NEMU_EXPR_DEBUG: int process_pointer_dereference(int pointer_dereference_index)] right token string : \"%s\"\n", tokens[right_token_index].str);
  }
  bool pointer_dereference_success = false;
  process_pointer_dereference_answer = isa_reg_str2val(tokens[right_token_index].str + 1, &pointer_dereference_success);
  if(pointer_dereference_success)
  {
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: int process_pointer_dereference(int pointer_dereference_index)] Pointer Dereference SUCCESS\n");
      printf("[NEMU_EXPR_DEBUG: int process_pointer_dereference(int pointer_dereference_index)] Pointer Dereference answer = %d\n", process_pointer_dereference_answer);
    }
    return process_pointer_dereference_answer;
  }
  else
  {
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: int process_pointer_dereference(int pointer_dereference_index)] Pointer Dereference FAILED\n");
    }
    return 0;
  }
}

char* calculate_one_round(bool success_calculate_one_round_call)
{
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: void calculate_one_round(bool success_calculate_one_round_call)] CKPT #01: First, find the highest priority and sub_priority, then get the index in tokens[32]\n");
  }
  // First, find the highest priority and sub_priority, then get the index in tokens[32]
  int calculate_one_round_highest_priority = -1;
  int calculate_one_round_highest_sub_priority = -1;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] Finding calculate_one_round_highest_priority\n");
  }
  for (int calculate_one_round_highest_priority_scanning_index = 0; calculate_one_round_highest_priority_scanning_index < nr_operator_tokens_no_parentheses; calculate_one_round_highest_priority_scanning_index = calculate_one_round_highest_priority_scanning_index + 1)
  {
    if(operator_tokens_no_parentheses[calculate_one_round_highest_priority_scanning_index].priority_level > calculate_one_round_highest_priority)
    {
      calculate_one_round_highest_priority = operator_tokens_no_parentheses[calculate_one_round_highest_priority_scanning_index].priority_level;
    }
    if(expr_print_checkpoint)
    {
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] calculate_one_round_highest_priority = %d, calculate_one_round_highest_sub_priority = %d\n", calculate_one_round_highest_priority, calculate_one_round_highest_sub_priority);
    }
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] Finding calculate_one_round_highest_sub_priority\n");
  }
  for (int calculate_one_round_highest_priority_scanning_index = 0; calculate_one_round_highest_priority_scanning_index < nr_operator_tokens_no_parentheses; calculate_one_round_highest_priority_scanning_index = calculate_one_round_highest_priority_scanning_index + 1)
  {
    if(operator_tokens_no_parentheses[calculate_one_round_highest_priority_scanning_index].priority_level == calculate_one_round_highest_priority)
    {
      if(operator_tokens_no_parentheses[calculate_one_round_highest_priority_scanning_index].sub_priority_level > calculate_one_round_highest_sub_priority)
      {
        calculate_one_round_highest_sub_priority = operator_tokens_no_parentheses[calculate_one_round_highest_priority_scanning_index].sub_priority_level;
      }
    }
    if(expr_print_checkpoint)
    {
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] calculate_one_round_highest_priority = %d, calculate_one_round_highest_sub_priority = %d\n", calculate_one_round_highest_priority, calculate_one_round_highest_sub_priority);
    }
  }
  int this_round_calculation_operator_token_index = -1;
  for(int scan_index = 0; scan_index < nr_operator_tokens_no_parentheses; scan_index = scan_index + 1)
  {
    if(operator_tokens_no_parentheses[scan_index].priority_level == calculate_one_round_highest_priority && operator_tokens_no_parentheses[scan_index].sub_priority_level == calculate_one_round_highest_sub_priority)
    {
      this_round_calculation_operator_token_index = scan_index;
    }
  }
  execution_histories[nr_execution_histories].that_round_operator_token_no_narentheses_index = this_round_calculation_operator_token_index;
  execution_histories[nr_execution_histories].that_round_token_index = operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] this_round_calculation_operator_token_index = %d\n", this_round_calculation_operator_token_index);
  }
  // Scend, check the condition to make a success call, if not success, set success_calculate_one_round_call to false
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: void calculate_one_round(bool success_calculate_one_round_call)] CKPT #02: Scend, check the condition to make a success call, if not success, set success_calculate_one_round_call to false\n");
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] operator_tokens_no_parentheses[%d]: Position = %d, Priority Level = %d, Sub Priority = %d, Token String = \"%s\", Type = %d\n", this_round_calculation_operator_token_index, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].priority_level, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].sub_priority_level, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].regex, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type);
  }
  if(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type == TK_DIVIDE && atoi(tokens[operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position + 1].str) == 0)
  {
    // Error: Divide by 0
    success_calculate_one_round_call = false;
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] ERROR, details in the next two lines\n");
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] operator_tokens_no_parentheses[%d].token_type == TK_DIVIDE\n", this_round_calculation_operator_token_index);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] atoi(tokens[operator_tokens_no_parentheses[%d].position + 1].str) == 0\n", this_round_calculation_operator_token_index);
    }
    return NULL;
  }
  if(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type != TK_NOT && operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type != TK_POINTER && operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type != TK_NEGATIVESIGN && operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type != TK_POSITIVESIGN && tokens[operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position - 1].type != TK_HEXNUMBER && tokens[operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position - 1].type != TK_NUMBER)
  {
    // Error: Left is not a Dec or Hex number
    // This function will be implemented to only double side operator
    // Will ignore TK_NOT, TK_POINTER, TK_NEGATIVESIGN, TK_POSITIVESIGN
    success_calculate_one_round_call = false;
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] ERROR, details in the next line\n");
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] TK_NUMBER's Type ID = %d, TK_HEXNUMBER's Type ID = %d\n", TK_NUMBER, TK_HEXNUMBER);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] this_round_calculation_operator_token_index = %d\n", this_round_calculation_operator_token_index);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] operator_tokens_no_parentheses[%d].position = %d\n",this_round_calculation_operator_token_index, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] operator_tokens_no_parentheses[%d].priority_level = %d\n",this_round_calculation_operator_token_index, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].priority_level);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] operator_tokens_no_parentheses[%d].sub_priority_level = %d\n",this_round_calculation_operator_token_index, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].sub_priority_level);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] operator_tokens_no_parentheses[%d].regex = \"%s\"\n",this_round_calculation_operator_token_index, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].regex);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] operator_tokens_no_parentheses[%d].token_type = %d\n",this_round_calculation_operator_token_index, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] tokens[%d].str =\"%s\"\n", operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position - 1, tokens[operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position - 1].str);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] tokens[%d].type = %d\n", operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position - 1, tokens[operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position - 1].type);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] tokens[operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position - 1].type is NOT (TK_HECNUMBER or TK_NUMBER)\n");
    }
    return NULL;
  }
  if(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type != TK_POINTER && tokens[operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position + 1].type != TK_HEXNUMBER && tokens[operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position + 1].type != TK_NUMBER)
  {
    // Error: Right is not a Dec or Hex number
    // This function will be implemented to double side operator
    // Will ignore TK_POINTER
    success_calculate_one_round_call = false;
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] ERROR, details in the next line\n");
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] TK_NUMBER's Type ID = %d, TK_HEXNUMBER's Type ID = %d\n", TK_NUMBER, TK_HEXNUMBER);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] this_round_calculation_operator_token_index = %d\n", this_round_calculation_operator_token_index);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] operator_tokens_no_parentheses[%d].position = %d\n",this_round_calculation_operator_token_index, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] operator_tokens_no_parentheses[%d].priority_level = %d\n",this_round_calculation_operator_token_index, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].priority_level);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] operator_tokens_no_parentheses[%d].sub_priority_level = %d\n",this_round_calculation_operator_token_index, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].sub_priority_level);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] operator_tokens_no_parentheses[%d].regex = \"%s\"\n",this_round_calculation_operator_token_index, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].regex);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] operator_tokens_no_parentheses[%d].token_type = %d\n",this_round_calculation_operator_token_index, operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] tokens[%d].str =\"%s\"\n", operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position + 1, tokens[operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position + 1].str);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] tokens[%d].type = %d\n", operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position + 1, tokens[operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position + 1].type);
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] tokens[operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position + 1].type is NOT (TK_HECNUMBER or TK_NUMBER)\n");
    }
    return NULL;
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] All tests are passed, continue\n");
  }
  // Third, implement specific calls to evaluate the result
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: void calculate_one_round(bool success_calculate_one_round_call)] CKPT #03: Third, implement specific calls to evaluate the result\n");
  }
  u_int64_t this_round_calculation_answer = 0;
  if(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type == TK_PLUS)
  {
    this_round_calculation_answer = process_add(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position);
  }
  if(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type == TK_MINUS)
  {
    this_round_calculation_answer = process_minus(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position);
  }
  if(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type == TK_MULTIPLY)
  {
    this_round_calculation_answer = process_multiply(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position);
  }
  if(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type == TK_DIVIDE)
  {
    this_round_calculation_answer = process_devide(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position);
  }
  // TODO: Add AND(OK), OR(OK), NOT(OK), EXOR(OK)
  if(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type == TK_AND)
  {
    this_round_calculation_answer = process_and(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position);
  }
  if(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type == TK_OR)
  {
    this_round_calculation_answer = process_or(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position);
  }
  if(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type == TK_NOT)
  {
    this_round_calculation_answer = process_not(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position);
  }
  if(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type == TK_EXOR)
  {
    this_round_calculation_answer = process_exor(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position);
  }
  // TODO: Add Pointer Dereference(OK) and Negative Sign(OK)
  if(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type == TK_POINTER)
  {
    this_round_calculation_answer = process_pointer_dereference(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position);
  }
  if(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type == TK_NEGATIVESIGN)
  {
    this_round_calculation_answer = process_negative_sign(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position);
  }
  if(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].token_type == TK_POSITIVESIGN)
  {
    this_round_calculation_answer = process_positive_sign(operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position);
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] this_round_calculation_answer (Dec) = %ld\n", this_round_calculation_answer);
    printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] this_round_calculation_answer (Hex) = %lx\n", this_round_calculation_answer);
  }
  // Fourth, store the new result to a token, check if the token's left and right is a pair of parentheses, if so, remove it
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: void calculate_one_round(bool success_calculate_one_round_call)] CKPT #04: Fourth, store the new result to a token, check if the token's left and right is a pair of parentheses, if so, remove it\n");
  }
  bool left_and_right_is_paired_parentheses = false;
  char* result_token = malloc(256);
  if(tokens[operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position - 2].type == TK_LEFT_PARENTHESES && tokens[operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position + 2].type == TK_RIGHT_PARENTHESES)
  {
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] left and right is paired parentheses\n");
    }
    left_and_right_is_paired_parentheses = true;
  }
  for(int current_copying_tokens_index = 0; current_copying_tokens_index < nr_token; current_copying_tokens_index = current_copying_tokens_index + 1)
  {
    if(current_copying_tokens_index == operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position - 2 && left_and_right_is_paired_parentheses == true)
    {
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] left and right is paired parentheses, skip copying\n");
      }
      continue;
      // Do not copy
    }
    if(current_copying_tokens_index == operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position - 1)
    {
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] is the left number, skip copying\n");
      }
      continue;
      // Do not copy
    }
    if(current_copying_tokens_index == operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position)
    {
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] is the operator, skip copying\n");
      }
      sprintf(result_token + strlen(result_token), "%ld", this_round_calculation_answer);
      continue;
      // Do not copy
    }
    if(current_copying_tokens_index == operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position + 1)
    {
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] is the right number, skip copying\n");
      }
      continue;
      // Do not copy
    }
    if(current_copying_tokens_index == operator_tokens_no_parentheses[this_round_calculation_operator_token_index].position + 2 && left_and_right_is_paired_parentheses == true)
    {
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] left and right is paired parentheses, skip copying\n");
      }
      continue;
      // Do not copy
    }
    strcat(result_token, tokens[current_copying_tokens_index].str);
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] result_token = \"%s\"\n", result_token);
    }
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: void calculate_one_round(bool success_calculate_one_round_call)] result_token = \"%s\"\n", result_token);
  }
  success_calculate_one_round_call = true;
  return result_token;
}

bool check_parentheses_balance()
{
  // This function is a more convenient version of check_parentheses(int left_index, int right_index)
  // With default left_index = 0 and right_index = nr_token - 1, and tolerence for Type I and Type II parentheses error
  // This can make it more efficient when the cmd_p command execute, for expr with different left parentheses count and right parentheses count, the cmd_p can quit immediately
  int check_parentheses_balance_left_count = 0;
  int check_parentheses_balance_right_count = 0;
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: bool check_parentheses_balance()] CKPT #01\n");
  }

  for(int current_check_index = 0; current_check_index < nr_token; current_check_index = current_check_index + 1)
  {
    if(expr_print_checkpoint)
    {
      printf("[NEMU_EXPR_CHECKPOINT: bool check_parentheses_balance()] CKPT #02\n");
    }
    if(tokens[current_check_index].type == TK_LEFT_PARENTHESES)
    {
      if(expr_print_checkpoint)
      {
        printf("[NEMU_EXPR_CHECKPOINT: bool check_parentheses_balance()] CKPT #03\n");
      }
      check_parentheses_balance_left_count = check_parentheses_balance_left_count + 1;
    }
    if(tokens[current_check_index].type == TK_RIGHT_PARENTHESES)
    {
      if(expr_print_checkpoint)
      {
        printf("[NEMU_EXPR_CHECKPOINT: bool check_parentheses_balance()] CKPT #04\n");
      }
      check_parentheses_balance_right_count = check_parentheses_balance_right_count + 1;
    }
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: bool check_parentheses_balance()] get check_parentheses_balance_left_count = %d\n", check_parentheses_balance_left_count);
    printf("[NEMU_EXPR_DEBUG: bool check_parentheses_balance()] get check_parentheses_balance_right_count = %d\n", check_parentheses_balance_right_count);
  }
  if(check_parentheses_balance_left_count == check_parentheses_balance_right_count)
  {
    if(expr_print_checkpoint)
    {
      printf("[NEMU_EXPR_CHECKPOINT: bool check_parentheses_balance()] CKPT #05\n");
    }
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: bool check_parentheses_balance()] BALANCED\n");
    }
    return true;
  }
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: bool check_parentheses_balance()] NOT BALANCED\n");
  }
  return false;
}

bool check_parentheses_valid()
{
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: bool check_parentheses_valid()] CKPT #01: Enter function\n");
    printf("[NEMU_EXPR_CHECKPOINT: bool check_parentheses_valid()] CKPT #02: Check parentheses number balance\n");
  }
  if(check_parentheses_balance())
  {
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: bool check_parentheses_valid()] check_parentheses_balance() SUCCESS\n");
    }
  }
  else
  {
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: bool check_parentheses_valid()] check_parentheses_balance() FAILED\n");
    }
    return false;
  }
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: bool check_parentheses_valid()] CKPT #03: Check inside balance\n");
  }
  int check_parentheses_valid_left_parentheses_count = 0;
  int check_parentheses_valid_right_parentheses_count = 0;
  for(int check_inside_parentheses_valid_index = 0; check_inside_parentheses_valid_index < nr_token; check_inside_parentheses_valid_index = check_inside_parentheses_valid_index + 1)
  {
    if(expr_print_checkpoint)
    {
      printf("[NEMU_EXPR_CHECKPOINT: bool check_parentheses_valid()] CKPT #04: In for loop for check inside balance\n");
    }
    if(tokens[check_inside_parentheses_valid_index].type == TK_LEFT_PARENTHESES)
    {
      check_parentheses_valid_left_parentheses_count = check_parentheses_valid_left_parentheses_count + 1;
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: bool check_parentheses_valid()] found a TK_RIGHT_PARENTHESES at index %2d, check_parentheses_valid_right_parentheses_count = %2d\n", check_inside_parentheses_valid_index, check_parentheses_valid_right_parentheses_count);
      }
    }
    if(tokens[check_inside_parentheses_valid_index].type == TK_RIGHT_PARENTHESES)
    {
      check_parentheses_valid_right_parentheses_count = check_parentheses_valid_right_parentheses_count + 1;
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: bool check_parentheses_valid()] found a TK_LEFT_PARENTHESES at index %2d, check_parentheses_valid_left_parentheses_count = %2d\n", check_inside_parentheses_valid_index, check_parentheses_valid_left_parentheses_count);
      }
    }
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: bool check_parentheses_valid()] check_parentheses_valid_left_parentheses_count - check_parentheses_valid_right_parentheses_count = %d\n", check_parentheses_valid_left_parentheses_count - check_parentheses_valid_right_parentheses_count);
    }
    if(check_parentheses_valid_left_parentheses_count - check_parentheses_valid_right_parentheses_count < 0)
    {
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: bool check_parentheses_valid()] check inside balance FAILED\n");
      }
      return false;
    }
    else
    {
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: bool check_parentheses_valid()] check inside balance SUCCESS\n");
      }
      return true;
    }
  }
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: bool check_parentheses_valid()] CKPT #04: End of function\n");
  }
  return true;
}

void process_operator_token()
{
  // Test Token: p 11 + 2 * 3 - (4 + 1) * 10
  // We should do more things here 20230303
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: void process_operator_token()] CKPT #01: Enter function\n");
  }
  int current_index_of_operator_tokens = 0;
  nr_operator_token = 0;
  for(int current_scanning_index = 0; current_scanning_index < nr_token; current_scanning_index = current_scanning_index + 1)
  {
    if(expr_print_checkpoint)
    {
      printf("[NEMU_EXPR_CHECKPOINT: void process_operator_token()] CKPT #02: In scanning tokens loop\n");
    }
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: void process_operator_token()] current_scanning_index = %d\n", current_scanning_index);
      printf("[NEMU_EXPR_DEBUG: void process_operator_token()] current_index_of_operator_tokens = %d\n", current_index_of_operator_tokens);
      printf("[NEMU_EXPR_DEBUG: void process_operator_token()] tokens[%d].type = %d\n", current_scanning_index, tokens[current_scanning_index].type);
      printf("[NEMU_EXPR_DEBUG: void process_operator_token()] tokens[%d].str = \"%s\"\n", current_scanning_index, tokens[current_scanning_index].str);
    }
    if(tokens[current_scanning_index].type != TK_NUMBER && tokens[current_scanning_index].type != TK_HEXNUMBER)
    {
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void process_operator_token()] find an operator token at nr_token index %d\n", current_scanning_index);
      }
      operator_tokens[current_index_of_operator_tokens].position = current_scanning_index;
      operator_tokens[current_index_of_operator_tokens].token_type = tokens[current_scanning_index].type;
      operator_tokens[current_index_of_operator_tokens].regex = tokens[current_scanning_index].str;
      operator_tokens[current_index_of_operator_tokens].priority = 0; // We set all Operator tokens to 0 first, and then we will change them in another function
      current_index_of_operator_tokens = current_index_of_operator_tokens + 1;
      nr_operator_token = nr_operator_token + 1;
      // Now the token is a operator token, implement codes to register them to operator_tokens
    }
    else
    {
      if(expr_print_debug)
      {
        printf("[NEMU_EXPR_DEBUG: void process_operator_token()] not an operator token at nr_token index %d, ignore in this scope\n", current_scanning_index);
      }
    }
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: void process_operator_token()] nr_operator_token = %d\n", nr_operator_token);
    }
  }
  // To make debug easier, we print all operator tokens if expr_print_debug is true
  for(int operator_token_print_index = 0; operator_token_print_index < nr_operator_token; operator_token_print_index = operator_token_print_index + 1)
  {
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: void process_operator_token()] Operator Token Number: %4d, Position: %4d, Token Type: %4d, Token String: \"%s\", Priority: %4d\n", operator_token_print_index, operator_tokens[operator_token_print_index].position, operator_tokens[operator_token_print_index].token_type, operator_tokens[operator_token_print_index].regex, operator_tokens[operator_token_print_index].priority);
    }
  }
  return;
}

char* expr_main_loop(char* token_input, bool *success_main_loop, bool *finished)
{
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: void expr_main_loop(char* token_input, bool *success_main_loop, bool *finished)] CKPT #01: Enter function\n");
  }
  expr_init();
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: void expr_main_loop(char* token_input, bool *success_main_loop, bool *finished)] CKPT #02: expr_init() finished\n");
  }
  if(!check_parentheses_valid())
  {
    // This function have both check inside valid and macro-valid
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: void expr_main_loop(char* token_input, bool *success_main_loop, bool *finished)] check_parentheses_valid FAILED\n");
    }
    *success_main_loop = false;
    *finished = false;
    return NULL;
  }
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: void expr_main_loop(char* token_input, bool *success_main_loop, bool *finished)] CKPT #04: check_parentheses_valid() finished\n");
  }
  if(!make_token(token_input))
  {
    *success_main_loop = false;
    *finished = false;
    return NULL;
  }
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: void expr_main_loop(char* token_input, bool *success_main_loop, bool *finished)] CKPT #04: make_token() finished\n");
  }
  if(nr_token == 1)
  {
    // The number of token is 1 means the calculation have finished, so just return result
    *success_main_loop = true;
    *finished = true;
    printf("Evaluate Success, Ans (Hex): %x, Ans (Dec): %d\n", atoi(tokens[0].str), atoi(tokens[0].str));
    return NULL;
  }
  if((nr_token == 2 && tokens[0].type == TK_NEGATIVESIGN) || (nr_token == 2 && tokens[0].type == TK_POSITIVESIGN))
  {
    // Treat as nr_token = 1 and finished token
    *success_main_loop = true;
    *finished = true;
    if(tokens[0].type == TK_NEGATIVESIGN)
    {
      printf("Evaluate Success, Ans (Hex): %x, Ans (Dec): %d, Ans (Oct): %o\n", -1 * atoi(tokens[1].str), -1 * atoi(tokens[1].str), -1 * atoi(tokens[1].str));
      return NULL;
    }
    if(tokens[0].type == TK_POSITIVESIGN)
    {
      printf("Evaluate Success, Ans (Hex): %x, Ans (Dec): %d, Ans (Oct): %o\n", atoi(tokens[1].str), atoi(tokens[1].str), atoi(tokens[1].str));
      return NULL;
    }
    *success_main_loop = false;
    printf("Evaluate Failed\n");
    return NULL;
  }
  else
  {
    process_operator_token();
    give_priority();
    give_priority_no_parentheses();
    give_sub_priority();
    *success_main_loop = true;
    execution_histories[nr_execution_histories].result_token_history = strndup(calculate_one_round(*success_main_loop), strlen(calculate_one_round(*success_main_loop)));
    return calculate_one_round(*success_main_loop);
    // TODO
  }
  if(expr_print_checkpoint)
  {
    printf("[NEMU_EXPR_CHECKPOINT: void expr_main_loop(char* token_input, bool *success_main_loop, bool *finished)] CKPT #05: End of function\n");
  }
}

word_t expr(char *e, bool *success) {
  init_execute_history();
  execution_histories[nr_execution_histories].result_token_history = strndup(e, strlen(e));
  nr_execution_histories = nr_execution_histories + 1;
  bool success_expr = true;
  bool finished_expr = false;
  int expr_main_loop_execution_count = 0;
  u_int64_t expr_answer = 0;
  while(success_expr && !finished_expr)
  {
    e = expr_main_loop(e, &success_expr, &finished_expr);
    if(expr_print_debug)
    {
      printf("[NEMU_EXPR_DEBUG: word_t expr(char *e, bool *success)] expr_main_loop_execution_count = %d\n", expr_main_loop_execution_count);
      printf("[NEMU_EXPR_DEBUG: word_t expr(char *e, bool *success)] e = \"%s\"\n", e);
      printf("[NEMU_EXPR_DEBUG: word_t expr(char *e, bool *success)] success_expr = %d\n", success_expr);
      printf("[NEMU_EXPR_DEBUG: word_t expr(char *e, bool *success)] finished_expr = %d\n", finished_expr);
    }
    execution_histories[nr_execution_histories].finished_history = finished_expr;
    execution_histories[nr_execution_histories].success_history = success_expr;
    nr_execution_histories = nr_execution_histories + 1;
    expr_main_loop_execution_count = expr_main_loop_execution_count + 1;
  }
  *success = success_expr;
  if(expr_print_debug)
  {
    printf("[NEMU_EXPR_DEBUG: word_t expr(char *e, bool *success)] nr_execution_histories = %d\n", nr_execution_histories);
    for(int current_print_history_index = 0; current_print_history_index < nr_execution_histories; current_print_history_index = current_print_history_index + 1)
    {
      printf("[NEMU_EXPR_DEBUG: word_t expr(char *e, bool *success)] Record No.%2d, Token No.%2d, Operator Token (No Parentheses) No.%2d, Finish: %d, Success: %d, Result Token: \"%s\"\n", current_print_history_index, execution_histories[current_print_history_index].that_round_token_index, execution_histories[current_print_history_index].that_round_operator_token_no_narentheses_index, execution_histories[current_print_history_index].finished_history, execution_histories[current_print_history_index].success_history, execution_histories[current_print_history_index].result_token_history);
    }
  }
  expr_answer = atoi(execution_histories[nr_execution_histories - 2].result_token_history);
  return expr_answer;
}