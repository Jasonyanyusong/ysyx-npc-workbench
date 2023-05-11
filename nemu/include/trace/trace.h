#include<stdbool.h>
#include<stdio.h>
#include<string.h>

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
    //char *message;
    bool used;
    word_t recordPC;
    word_t pc;
    word_t snpc;
    word_t dnpc;
    word_t inst;
    char *iringbuf_diasm;
}iringbuf_store[64];

void iringbuf_init(){
    printf("trace: iringbuf enabled, ring size is %d\n", iringbuf_size);
    for(int i = 0; i < 64; i = i + 1){
        iringbuf_store[i].used = false;
        iringbuf_store[i].recordPC = 0; // Set PC to 0 at initialization process
        //memset(iringbuf_store[i].iringbuf_diasm,0,128);
    }
    if(remove("iringbuf.txt")==0){
        printf("NEMU removed previous iringbuf records.\n");
    } // So previous traces will not be recorded
    return;
}

void iringbuf_write(char* write_msg, word_t pc, word_t snpc, word_t dnpc, word_t inst, word_t diasm){
    int number_used = 0;
    for(int i = 0; i < iringbuf_size; i = i + 1){
        if(iringbuf_store[i].used == true){
            number_used = number_used + 1;
        }
    }

    printf("%d of iringbuf have used\n", number_used);
    if(number_used >= iringbuf_size){
        int lowest_index = 0;
        for(int i = 0; i < iringbuf_size; i = i + 1){
            if(iringbuf_store[i].recordPC >= iringbuf_store[lowest_index].recordPC){
                lowest_index = i;
            }
        }
        printf("lowest_index is %d\n", lowest_index);
        iringbuf_store[lowest_index].used = true;
        iringbuf_store[lowest_index].recordPC = pc;
        iringbuf_store[lowest_index].pc = pc;
        iringbuf_store[lowest_index].snpc = snpc;
        iringbuf_store[lowest_index].dnpc = dnpc;
        iringbuf_store[lowest_index].inst = inst;
        //iringbuf_store[lowest_index].iringbuf_diasm = diasm;
    }
    else{
        printf("have spare space, number used is %d\n", number_used);
        iringbuf_store[number_used].used = true;
        iringbuf_store[number_used].recordPC = pc;
        iringbuf_store[number_used].pc = pc;
        iringbuf_store[number_used].snpc = snpc;
        iringbuf_store[number_used].dnpc = dnpc;
        iringbuf_store[number_used].inst = inst;
        //iringbuf_store[number_used].iringbuf_diasm = diasm;
        strcpy(iringbuf_store[number_used].iringbuf_diasm, diasm);
    }

    FILE *iringbuf_file = fopen("iringbuf.txt", "w+");
    assert(iringbuf_file != NULL);
    for(int i = 0; i < 64; i = i + 1){
        printf("i = %d, used = %d\n", i, iringbuf_store[i].used);
        printf("pc: 0x%lx  snpc: 0x%lx  inst: 0x%8x  dnpc: 0x%lx  %s\n", iringbuf_store[number_used].pc, iringbuf_store[number_used].snpc, iringbuf_store[number_used].inst, iringbuf_store[number_used].dnpc = dnpc, iringbuf_store[number_used].iringbuf_diasm);
        //fputs(iringbuf_store[i].message, iringbuf_file);
    }
    //
    fclose(iringbuf_file);
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