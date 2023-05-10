#include<stdbool.h>
#include<stdio.h>

int iringbuf_size = 64;

void itrace_init(){
    printf("trace: itrace enabled\n");
    if(remove("itrace.txt")==0){
        printf("NEMU removed previous itrace records.\n");
    } // So previous traces will not be recorded
    return;
}

void itrace_write(char* messageWrite){
    FILE *itrace_file = fopen("itrace.txt", "a+");
    assert(itrace_file != NULL);
    fputs(messageWrite, itrace_file);
    fclose(itrace_file);
    return;
}

struct ringbuf_element{
    char message[128];
    bool used;
    int index;
}iringbuf_store[64];

void iringbuf_init(){
    printf("trace: iringbuf enabled, ring size is %d\n", iringbuf_size);
    for(int i = 0; i < 64; i = i + 1){
        iringbuf_store[i].index = -1;
        iringbuf_store[i].used = false;
    }
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