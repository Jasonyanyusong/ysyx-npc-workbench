#include<stdbool.h>
#include<stdio.h>

void itrace_init(){
    if(remove("$NEMU_HOME/itrace.txt")==0){
        printf("NEMU removed previous itrace records.\n");
    } // So previous traces will not be recorded
    return;
}

void iringbuf_init(){
    if(remove("$NEMU_HOME/iringbuf.txt")==0){
        printf("NEMU removed previous iringbuf records.\n");
    } // So previous traces will not be recorded
    return;
}

void mtrace_init(){
    if(remove("$NEMU_HOME/mtrace.txt")==0){
        printf("NEMU removed previous mtrace records.\n");
    } // So previous traces will not be recorded
    return;
}

void ftrace_init(){
    if(remove("$NEMU_HOME/ftrace.txt")==0){
        printf("NEMU removed previous ftrace records.\n");
    } // So previous traces will not be recorded
    return;
}

void trace_init(bool enable_itrace, bool enable_iringbuf, bool enable_mtrace, bool enable_ftrace){
    printf("Initialized (Enabled) tracer(s): ");
    if(enable_itrace){
        printf(" itrace ");
        itrace_init();
    }
    if(enable_iringbuf){
        printf(" iringbuf ");
        iringbuf_init();
    }
    if(enable_mtrace){
        printf(" mtrace ");
        mtrace_init();
    }
    if(enable_ftrace){
        printf(" ftrace ");
        ftrace_init();
    }
    printf("\n");
    return;
}