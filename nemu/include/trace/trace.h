#include<stdbool.h>
#include<stdio.h>
#include<string.h>
#include<elf.h>
#include<stdlib.h>
#include<stdint.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<common.h>
#include<fcntl.h>

#include "isa.h"

#define iringbuf_size 64
//#define ftrace_elf_nr_Phdr 1024
#define ftrace_elf_nr_Shdr 1024
#define ftrace_nr_stirng_table 1024

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

void etrace_init(){
    printf("trace: etrace enabled\n");
    if(remove("etrace.txt")==0){
        printf("NEMU removed previous etrace records.\n");
    } // So previous traces will not be recorded
    return;
}

void etrace_write(uint64_t mstatus, uint64_t mtvec, uint64_t mepc, uint64_t mcause){
    FILE *etrace_file = fopen("etrace.txt", "a+");
    assert(etrace_file != NULL);
    char etrace_msg[128];
    sprintf(etrace_msg, "pc: 0x%lx, mstatus: 0x%lx, mtvec: 0x%lx, mepc: 0x%lx, mcause: 0x%lx\n\0", cpu.pc, mstatus, mtvec, mepc, mcause);
    printf("trace-etrace: %s", etrace_msg);
    fputs(etrace_msg, etrace_file);
    fclose(etrace_file);
    return;
}

void dtrace_init(){
    printf("trace: dtrace enabled\n");
    if(remove("dtrace.txt")==0){
        printf("NEMU removed previous dtrace records.\n");
    } // So previous traces will not be recorded
    return;
}

void dtrace_write(uint64_t dtrace_pc, char* dtrace_device_name, uint64_t dtrace_addr, int dtrace_len, bool dtrace_is_write, uint64_t dtrace_data){
    FILE *dtrace_file = fopen("dtrace.txt", "a+");
    assert(dtrace_file != NULL);

    char mtrace_to_be_written[128];
    if(dtrace_is_write == true){
        sprintf(mtrace_to_be_written, "pc: 0x%lx, mmio_W, name = \"%s\", addr = 0x%lx, len = %d, data = 0x%lx\n", cpu.pc, dtrace_device_name, dtrace_addr, dtrace_len, dtrace_data);
    }else{
        sprintf(mtrace_to_be_written, "pc: 0x%lx, mmio_R, name = \"%s\", addr = 0x%lx, len = %d, data = 0x%lx\n", cpu.pc, dtrace_device_name, dtrace_addr, dtrace_len, dtrace_data);
    }

    printf("trace-dtrace: %s", mtrace_to_be_written);
    fputs(mtrace_to_be_written, dtrace_file);
    fclose(dtrace_file);
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

void mtrace_write(bool mem_RW, word_t addr, int mem_len, word_t mem_data){
    char written_to_mtrace[128];
    if(mem_RW){
        sprintf(written_to_mtrace, "pc: 0x%lx  mem_w  addr: 0x%lx  len: %d  data: 0x%lx\n", cpu.pc, addr, mem_len, mem_data);
        printf("trace-mtrace: %s", written_to_mtrace);
        FILE *mtrace_file = fopen("mtrace.txt", "a+");
        assert(mtrace_file != NULL);
        fputs(written_to_mtrace, mtrace_file);
        fclose(mtrace_file);
    }
    else{
        sprintf(written_to_mtrace, "pc: 0x%lx  mem_r  addr: 0x%lx  len: %d  data: 0x%lx\n", cpu.pc, addr, mem_len, mem_data);
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
    sprintf(rtrace_pc_msg, "At pc = 0x%lx, \0", cpu.pc);
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

#define nr_ftrace_func 1024
struct ftrace_function
{
    char func_name[256];
    uint64_t func_startAddr;
    uint64_t func_endAddr;
}ftrace_functions[nr_ftrace_func];

int nr_ftrace_function = 0;

void ftrace_init(char* ftrace_elf, char* ftrace_das){
    printf("trace: ftrace enabled\n");
    printf("trace: ftrace ELF file is \"%s\"\n", ftrace_elf);
    if(remove("ftrace.txt")==0){
        printf("NEMU removed previous ftrace records.\n");
    } // So previous traces will not be recorded

    assert(ftrace_elf != NULL);

    uint8_t *mem;
    Elf64_Ehdr *Ehdr; // ELF file header
    Elf64_Shdr *Shdr; // ELF Section(s)
    Elf64_Sym  *Sym;  // ELF Symbol(s)
    char *strtab = NULL;

    int openfile;
    openfile = open(ftrace_elf, O_RDONLY);

    struct stat st;
    fstat(openfile, &st);

    mem = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, openfile, 0);

    Ehdr = (Elf64_Ehdr *)mem;
    Shdr = (Elf64_Shdr *)&mem[Ehdr -> e_shoff];
    int nr_sectionHeader = Ehdr -> e_shnum;

    uint64_t Sym_total  = -1;
    uint64_t Sym_single = -1;

    for(int i = 0; i < nr_sectionHeader; i = i + 1){
        if(Shdr[i].sh_type == SHT_SYMTAB){
            printf("trace: ftrace find Symbole Table at index %d\n", i);
            Sym = (Elf64_Sym *)&mem[Shdr[i].sh_offset];
            Sym_total = Shdr[i].sh_size;
            assert(Sym_total > 0);
            Sym_single = Shdr[i].sh_entsize;
            assert(Sym_single > 0);
            strtab = (char *)&mem[Shdr[Shdr[i].sh_link].sh_offset];
            assert(strtab != NULL);
        }
    }

    int nr_Sym = -1;
    nr_Sym = Sym_total / Sym_single;
    assert(nr_Sym > 0);
    printf("trace: ftrace get Symbol number is %d\n", nr_Sym);

    for(int i = 0; i < nr_Sym; i = i + 1){
        if(ELF64_ST_TYPE(Sym[i].st_info) == STT_FUNC){
            printf("trace: ftrace catch a function at Sym[%d]\n", i);
            if(i >= nr_ftrace_func){
                printf("trace: ftrace reach max amount of function record, change in trace.h\n");
                assert(0);
            }
            strcpy(ftrace_functions[nr_ftrace_function].func_name, &strtab[Sym[i].st_name]);
            ftrace_functions[nr_ftrace_function].func_startAddr = Sym[i].st_value;
            ftrace_functions[nr_ftrace_function].func_endAddr = Sym[i].st_value + Sym[i].st_size;
            printf("trace: ftrace find function \"%s\"@[0x%lx, 0x%lx]\n",ftrace_functions[nr_ftrace_function].func_name, ftrace_functions[nr_ftrace_function].func_startAddr, ftrace_functions[nr_ftrace_function].func_endAddr);
            nr_ftrace_function = nr_ftrace_function + 1;
        }
    }

    return;
}

void ftrace_write(uint32_t ftrace_inst, uint64_t ftrace_pc, uint64_t ftrace_dnpc){
    // JAL: function call
    // JALR: function return
    return;
}

void trace_init(char* elf, char* diasm){
    IFDEF(CONFIG_InstructionTrace, itrace_init());
    IFDEF(CONFIG_InstructionRingBuffer, iringbuf_init());
    IFDEF(CONFIG_MemoryTrace, mtrace_init());
    IFDEF(CONFIG_FunctionTrace, ftrace_init(elf, diasm));
    IFDEF(CONFIG_RegisterTrace, rtrace_init());
    IFDEF(CONFIG_DeviceTrace, dtrace_init());
    return;
}