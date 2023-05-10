#include <stdio.h>
#include "trace.h"

void itrace_init(){
    FILE *itrace_file = fopen("$NEMU_HOME/trace/itrace.txt", "w+");
    return;
}