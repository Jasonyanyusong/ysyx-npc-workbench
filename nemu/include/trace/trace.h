#include<stdbool.h>
#include<stdio.h>

void itrace_init(){
    remove("$NEMU_HOME/itrace.txt"); // So previous traces will not be recorded
    FILE *itrace_file = fopen("$NEMU_HOME/itrace.txt", "w+");
    fputs("NEMU itrace records\n", itrace_file);
    //fwrite("NEMU itrace records", strlen("NEMU itrace records"), 1, itrace_file);
    //fwrite("This file contains the pc, inst and diassembly of each instruction exected by NEMU", strlen("This file contains the pc, inst and diassembly of each instruction exected by NEMU"), 1, itrace_file);
    return;
}

void trace_init(bool enable_itrace, bool enable_iringbuf, bool enable_mtrace, bool enable_ftrace){
    if(enable_itrace){
        itrace_init();
    }
    if(enable_iringbuf){
        //init_iringbuf(64);
    }
    if(enable_mtrace){
        //init_mtrace();
    }
    if(enable_ftrace){
        //init_ftrace();
    }
    return;
}