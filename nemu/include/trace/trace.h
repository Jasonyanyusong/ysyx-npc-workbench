#include<stdbool.h>
#include<stdio.h>
#include<string.h>
#include<elf.h>
#include<stdlib.h>

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

struct ftrace_function{
    char * ftrace_name;
    word_t ftrace_start_addr;
    word_t ftrace_end_addr;
}ftrace_functions[1024];

word_t ftrace_exec_depth = 0;
word_t ftrace_exec_count = 0;
int ftrace_nr_function = 0;
char ftrace_stringtable[ftrace_nr_stirng_table];

void ftrace_process_elf(char* elf_addr){
    FILE * elf_file;

    Elf64_Ehdr ftrace_efl_header;
    elf_file = fopen(elf_addr, "r");
    assert(elf_file != NULL);

    int ftrace_check_elf = 0;
    ftrace_check_elf = fread(&ftrace_efl_header, sizeof(ftrace_efl_header), 1, elf_file);

    // Check: decide if we have processed a vaild ELF file
    assert(ftrace_check_elf != 0);
    assert(ftrace_efl_header.e_ident[0] == 0x7F);
    assert(ftrace_efl_header.e_ident[1] == 'E');
    assert(ftrace_efl_header.e_ident[2] == 'L');
    assert(ftrace_efl_header.e_ident[3] == 'F');
    printf("trace: ftrace get vaild ELF file at \"%s\"\n", elf_addr);

    // Process Section Headers using fseek and fread
    Elf64_Shdr ftrace_elf_section_header[ftrace_elf_nr_Shdr];
    fseek(elf_file, ftrace_efl_header.e_shoff, SEEK_SET);
    fread(&ftrace_elf_section_header, ftrace_efl_header.e_shnum, sizeof(Elf64_Shdr), elf_file);
    fseek(elf_file, ftrace_elf_section_header[ftrace_efl_header.e_shstrndx].sh_size, SEEK_SET);
    fread(&ftrace_stringtable, ftrace_elf_section_header[ftrace_efl_header.e_shstrndx].sh_size, 1, elf_file);

    // Find which Section header is symtap
    printf("trace: ftrace ELF Section header: number of header(s) is %d, starting at offset 0x%lx\n", ftrace_efl_header.e_shnum, ftrace_efl_header.e_shoff);
    int symtap_index = -1;
    for(int i = 0; i < ftrace_efl_header.e_shnum; i = i + 1){
        // elf.h defined that SHT_SYMTAP's sh_type is 2
        if(ftrace_elf_section_header[i].sh_type == 2){
            symtap_index = i;
        }
    }
    assert(symtap_index > 0);
    printf("trace: in this elf file, symbole table's index is %d\n", symtap_index);

    // Find which Section header is strtap
    printf("trace: ftrace ELF Section header: number of header(s) is %d, starting at offset 0x%lx\n", ftrace_efl_header.e_shnum, ftrace_efl_header.e_shoff);
    int strtap_index = -1;
    for(int i = 0; i < ftrace_efl_header.e_shnum; i = i + 1){
        // elf.h defined that SHT_STRTAP's sh_type is 3
        if(ftrace_elf_section_header[i].sh_type == 3){
            strtap_index = i;
        }
    }
    assert(strtap_index > 0);
    printf("trace: in this elf file, string table's index is %d\n", strtap_index);

    // now we know which is the symbole table and the string table, then we need to fetch function name and address
    char string_array[ftrace_elf_section_header[strtap_index].sh_size];
    fseek(elf_file, ftrace_elf_section_header[strtap_index].sh_offset, SEEK_SET);
    fread(&string_array, sizeof(string_array), 1, elf_file);

    Elf64_Xword nr_symbol = ftrace_elf_section_header[symtap_index].sh_size / ftrace_elf_section_header[symtap_index].sh_entsize;
    printf("ftrace: number of symbol is %ld\n", nr_symbol);

    Elf64_Sym symbol_array[nr_symbol];
    fseek(elf_file, ftrace_elf_section_header[strtap_index].sh_offset, SEEK_SET);
    fread(&symbol_array, ftrace_elf_section_header[symtap_index].sh_size, 1, elf_file);

    // All data need for ftrace is initialized, use these datas to init ftrace
    for(int i = 0; i < nr_symbol; i = i + 1){
        uint8_t stinfo = symbol_array[i].st_info;
        printf("trace: at symbol %ld, st_info is 0x%x, process -> 0x%x\n", i, stinfo, ELF64_ST_TYPE(stinfo));
        if(ELF64_ST_TYPE(stinfo) == STT_FUNC){
            printf("trace: symbol_array[%d]'s value hold a function\n", i);
        }
    }
    return;
}

void ftrace_init(char* ftrace_elf, char* ftrace_das){
    printf("trace: ftrace enabled\n");
    printf("trace: ftrace ELF file is \"%s\"\n", ftrace_elf);
    //printf("trace: ftrace TXT file is \"%s\"\n", ftrace_das);
    if(remove("ftrace.txt")==0){
        printf("NEMU removed previous ftrace records.\n");
    } // So previous traces will not be recorded
    // TODO: elf and disassembly file location is parsed here, read ELF and diasm to get location of each function
    ftrace_process_elf(ftrace_elf);
    return;
}

void ftrace_write(uint32_t ftrace_inst, uint64_t ftrace_pc){
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