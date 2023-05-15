#include<stdbool.h>
#include<stdio.h>
#include<string.h>

#define iringbuf_size 64

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
    printf("trace-itrace: %s", messageWrite);
    fputs(messageWrite, itrace_file);
    fclose(itrace_file);
    return;
}

char iringbuf_records[iringbuf_size][128]={0};
int iringbuf_count = 0;

void iringbuf_init(){
    printf("trace: iringbuf enabled, ring size is %d\n", iringbuf_size);
    if(remove("iringbuf.txt")==0){
        printf("NEMU removed previous iringbuf records.\n");
    } // So previous traces will not be recorded
    return;
}

void iringbuf_write(word_t pc, word_t snpc, word_t dnpc, word_t inst, char* diasm){
    int iringbuf_opindex = iringbuf_count % iringbuf_size;
    sprintf(iringbuf_records[iringbuf_opindex], "pc: 0x%lx  snpc: 0x%lx  inst: 0x%8lx  dnpc: 0x%lx  %s\n", pc, snpc, inst, dnpc, diasm);
    iringbuf_count = iringbuf_count + 1;
    FILE *iringbuf_file = fopen("iringbuf.txt", "w+");
    assert(iringbuf_file != NULL);
    for(int i = 0; i < iringbuf_size; i = i + 1){
        printf("trace-iringbuf: %s", iringbuf_records[i]);
        fputs(iringbuf_records[i], iringbuf_file);
    }
    fclose(iringbuf_file);
    return;
}

word_t mtrace_pc = 0;
void mtrace_updatePC(word_t new_pc){
    mtrace_pc = new_pc;
    printf("trace-mtrace: current pc is 0x%lx\n", mtrace_pc);
    return;
}

void mtrace_write(bool mem_RW, word_t addr, int mem_len, word_t mem_data){
    char written_to_mtrace[128];
    if(mem_RW){
        sprintf(written_to_mtrace, "pc: 0x%lx  mem_w  addr: 0x%lx  len: %d  data: 0x%lx\n", mtrace_pc, addr, mem_len, mem_data);
        printf("trace-mtrace: %s", written_to_mtrace);
        FILE *mtrace_file = fopen("mtrace.txt", "a+");
        assert(mtrace_file != NULL);
        fputs(written_to_mtrace, mtrace_file);
        fclose(mtrace_file);
    }
    else{
        sprintf(written_to_mtrace, "pc: 0x%lx  mem_r  addr: 0x%lx  len: %d  data: 0x%lx\n", mtrace_pc, addr, mem_len, mem_data);
        printf("trace-mtrace: %s", written_to_mtrace);
        FILE *mtrace_file = fopen("mtrace.txt", "a+");
        assert(mtrace_file != NULL);
        fputs(written_to_mtrace, mtrace_file);
        fclose(mtrace_file);
    }
}

void mtrace_init(){
    printf("trace: mtrace enabled\n");
    if(remove("mtrace.txt")==0){
        printf("NEMU removed previous mtrace records.\n");
    } // So previous traces will not be recorded
    return;
}

word_t rtrace_pc = 0;
void rtrace_updatePC(word_t new_pc){
    rtrace_pc = new_pc;
    printf("trace-rtrace: current pc is 0x%lx\n", rtrace_pc);
    return;
}

void rtrace_init(){
    printf("trace: rtrace enabled\n");
    if(remove("rtrace.txt")==0){
        printf("NEMU removed previous mtrace records.\n");
    } // So previous traces will not be recorded
    return;
}

void rtrace_write(){
    char written_to_rtrace[1024] = {0};
    char rtrace_pc_msg[64];
    sprintf(rtrace_pc_msg, "At pc = 0x%lx, \0", rtrace_pc);
    strcat(written_to_rtrace, rtrace_pc_msg);
    for(int i = 0; i < 32; i = i + 1){
        char rtrace_gpr_msg[64];
        sprintf(rtrace_gpr_msg, "x%d = 0x%16lx ", i, cpu.gpr[i]);
        strcat(written_to_rtrace, rtrace_gpr_msg);
    }
    strcat(written_to_rtrace, "\n\0");
    printf("\n\n%s\n\n", written_to_rtrace);
    FILE *rtrace_file = fopen("rtrace.txt", "a+");
    assert(rtrace_file != NULL);
    fputs(written_to_rtrace, rtrace_file);
    fclose(rtrace_file);
    return;
}

struct ftrace_function{
    char * ftrace_name;
    word_t ftrace_start_addr;
    word_t ftrace_end_addr;
}ftrace_functions[1024];

word_t ftrace_exec_depth = 0;
int ftrace_nr_function = 0;

void ftrace_init(char* ftrace_elf, char* ftrace_das){
    printf("trace: ftrace enabled\n");
    printf("trace: ftrace ELF file is \"%s\"\n", ftrace_elf);
    printf("trace: ftrace TXT file is \"%s\"\n", ftrace_das);
    if(remove("ftrace.txt")==0){
        printf("NEMU removed previous ftrace records.\n");
    } // So previous traces will not be recorded
    return;
}

void ftrace_write(){
    return;
}

void trace_init(char* elf, char* diasm){
    IFDEF(CONFIG_InstructionTrace, itrace_init());
    IFDEF(CONFIG_InstructionRingBuffer, iringbuf_init());
    IFDEF(CONFIG_MemoryTrace, mtrace_init());
    IFDEF(CONFIG_FunctionTrace, ftrace_init(elf, diasm));
    IFDEF(CONFIG_RegisterTrace, rtrace_init());
    return;
}