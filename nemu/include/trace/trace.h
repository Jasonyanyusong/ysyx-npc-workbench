#include<stdbool.h>
#include<stdio.h>
#include<string.h>
#include<elf.h>
#include<stdlib.h>

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
word_t ftrace_exec_count = 0;
int ftrace_nr_function = 0;

void ftrace_process_efl(char* elf_addr){
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

    // Display: informations about ELF header
    printf("trace: ftrace ELF header: e_type (Object file type) is ");
    switch(ftrace_efl_header.e_type){
        default:      printf("Unknown type\n")                    ; break;
        case 0:       printf("No file type\n")                    ; break;
        case 1:       printf("Relocatable file\n")                ; break;
        case 2:       printf("Executable file\n")                 ; break;
        case 3:       printf("Shared object file\n")              ; break;
        case 4:       printf("Core file\n")                       ; break;
        case 5:       printf("Number of defined types\n")         ; break;
        case 0xfe00:  printf("OS-specific range start\n")         ; break;
        case 0xfeff:  printf("OS-specific range end\n")           ; break;
        case 0xff00:  printf("Processor-specific range start\n")  ; break;
        case 0xffff:  printf("Processor-specific range end\n")    ; break;
    }

    printf("trace: ftrace ELF header: e_machine (Architecture) is ");
    switch(ftrace_efl_header.e_machine){
        default:      printf("Unknown/Other machine\n")           ; break;
        case 243:     printf("RISC-V\n")                          ; break;
    }

    printf("trace: ftrace ELF header: e_version (Object file version) is ");
    switch(ftrace_efl_header.e_version){
        default:      printf("Unknown version\n")                 ; break;
        case 0:       printf("Invalid ELF version\n")             ; break;
        case 1:       printf("Current version\n")                 ; break;
        case 2:       printf("Number of versions\n")              ; break;
    }

    printf("trace: ftrace ELF header: e_entry (Entry point virtual address) is 0x%lx\n", ftrace_efl_header.e_entry);
    printf("trace: ftrace ELF header: e_phoff (Program header table file offset) is 0x%lx\n", ftrace_efl_header.e_phoff);
    printf("trace: ftrace ELF header: e_shoff (Section header table file offset) is 0x%lx\n", ftrace_efl_header.e_shoff);

    printf("trace: ftrace ELF header: e_flags (Processor-specific flags) is ");
    switch(ftrace_efl_header.e_flags){
        default:      printf("Unknown flag\n")                    ; break;
        case 0x1f:    printf("EF_SH_MACH_MASK\n")                 ; break;
        case 0x0:     printf("EF_SH_UNKNOWN\n")                   ; break;
        case 0x1:     printf("EF_SH1\n")                          ; break;
        case 0x2:     printf("EF_SH2\n")                          ; break;
        case 0x3:     printf("EF_SH3\n")                          ; break;
        case 0x4:     printf("EF_SH_DSP\n")                       ; break;
        case 0x5:     printf("EF_SH3_DSP\n")                      ; break;
        case 0x6:     printf("EF_SH4AL_DSP\n")                    ; break;
        case 0x8:     printf("EF_SH3E\n")                         ; break;
        case 0x9:     printf("EF_SH4\n")                          ; break;
        case 0xb:     printf("EF_SH2E\n")                         ; break;
        case 0xc:     printf("EF_SH4A\n")                         ; break;
        case 0xd:     printf("EF_SH2A\n")                         ; break;
        case 0x10:    printf("EF_SH4_NOFPU\n")                    ; break;
        case 0x11:    printf("EF_SH4A_NOFPU\n")                   ; break;
        case 0x12:    printf("EF_SH4_NOMMU_NOFPU\n")              ; break;
        case 0x13:    printf("EF_SH2A_NOFPU\n")                   ; break;
        case 0x14:    printf("EF_SH3_NOMMU\n")                    ; break;
        case 0x15:    printf("EF_SH2A_SH4_NOFPU\n")               ; break;
        case 0x16:    printf("EF_SH2A_SH3_NOFPU\n")               ; break;
        case 0x17:    printf("EF_SH2A_SH4\n")                     ; break;
        case 0x18:    printf("EF_SH2A_SH3E\n")                    ; break;
    }

    printf("trace: ftrace ELF header: e_ehsize (ELF header size in bytes) is 0x%x\n", ftrace_efl_header.e_ehsize);
    printf("trace: ftrace ELF header: e_phentsize (Program header table entry size) is 0x%x\n", ftrace_efl_header.e_phentsize);
    printf("trace: ftrace ELF header: e_phnum (Program header table entry count)is 0x%x\n", ftrace_efl_header.e_phnum);
    printf("trace: ftrace ELF header: e_shentsize (Section header table entry size) is 0x%x\n", ftrace_efl_header.e_shentsize);
    printf("trace: ftrace ELF header: e_shnum (Section header table entry count) is 0x%x\n", ftrace_efl_header.e_shnum);
    printf("trace: ftrace ELF header: e_shstrndx (Section header string table index) is 0x%x\n", ftrace_efl_header.e_shstrndx);

    //Elf64_

    return;
}

void ftrace_init(char* ftrace_elf, char* ftrace_das){
    printf("trace: ftrace enabled\n");
    printf("trace: ftrace ELF file is \"%s\"\n", ftrace_elf);
    printf("trace: ftrace TXT file is \"%s\"\n", ftrace_das);
    if(remove("ftrace.txt")==0){
        printf("NEMU removed previous ftrace records.\n");
    } // So previous traces will not be recorded
    // TODO: elf and disassembly file location is parsed here, read ELF and diasm to get location of each function
    ftrace_process_efl(ftrace_elf);
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
    return;
}