#include<stdbool.h>
#include<stdio.h>

void itrace_init(){
    printf("trace: itrace enabled\n");
    if(remove("itrace.txt")==0){
        printf("NEMU removed previous itrace records.\n");
    } // So previous traces will not be recorded
    return;
}

void iringbuf_init(){
    printf("trace: iringbuf enabled\n");
    if(remove("iringbuf.txt")==0){
        printf("NEMU removed previous iringbuf records.\n");
    } // So previous traces will not be recorded
    return;
}

void mtrace_init(){
    printf("trace: mtrace enabled\n");
    if(remove("$mtrace.txt")==0){
        printf("NEMU removed previous mtrace records.\n");
    } // So previous traces will not be recorded
    return;
}

void ftrace_init(){
    printf("trace: ftrace enabled\n");
    if(remove("ftrace.txt")==0){
        printf("NEMU removed previous ftrace records.\n");
    } // So previous traces will not be recorded
    return;
}

void trace_init(){
    IFDEF(CONFIG_InstructionTrace, itrace_init());
    IFDEF(CONFIG_InstructionRingBuffer, iringbuf_init());
    IFDEF(CONFIG_MemoryTrace, mtrace_init());
    IFDEF(CONFIG_FunctionTrace, ftrace_init());
    return;
}