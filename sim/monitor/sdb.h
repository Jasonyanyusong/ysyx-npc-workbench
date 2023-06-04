#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char *e, bool *success);
void rtl_gpr_display();
void rtl_fpr_display();
void rtl_csr_display();

void record_WP(char* expression);
void delete_WP(int WP_number);
void print_WP();
void check_WP();
#endif
