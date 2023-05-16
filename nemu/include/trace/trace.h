#include<stdbool.h>
#include<stdio.h>
#include<string.h>
#include<elf.h>
#include<stdlib.h>

#define iringbuf_size 64
#define ftrace_elf_nr_Phdr 1024

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

    // Default set number of Program Header's number at the begining of the file
    Elf64_Phdr ftrace_elf_program_header[ftrace_elf_nr_Phdr];
    fseek(elf_file, ftrace_efl_header.e_phoff, SEEK_SET);
    fread(ftrace_elf_program_header, sizeof(Elf64_Phdr), ftrace_efl_header.e_phnum, elf_file);
    printf("trace: ftrace ELF Program header: number of header(s) is %d, starting at offset 0x%4lx\n", ftrace_efl_header.e_phnum, ftrace_efl_header.e_phoff);
    for(int i = 0; i < ftrace_efl_header.e_phnum; i = i + 1){
        printf("trace: ftrace ELF Program header: program header %d\n\t\t\t\t\tp_type (Segment type) is %d:", i, ftrace_elf_program_header[i].p_type);
        switch(ftrace_elf_program_header[i].p_type){
            default:         printf("Unknown type\n");                                     break;
            case 0:          printf("PT_NULL (Program header table entry unused)\n");      break;
            case 1:          printf("PT_LOAD (Loadable program segment)\n");               break;
            case 2:          printf("PT_DYNAMIC (Dynamic linking information)\n");         break;
            case 3:          printf("PT_INTERP (Program interpreter)\n");                  break;
            case 4:          printf("PT_NOTE (Auxiliary information)\n");                  break;
            case 5:          printf("PT_SHLIB (Reserved)\n");                              break;
            case 6:          printf("PT_PHDR (Entry for header table itself)\n");          break;
            case 7:          printf("PT_TLS (Thread-local storage segment)\n");            break;
            case 8:          printf("PT_NUM (Number of defined types)\n");                 break;
            case 0x60000000: printf("PT_LOOS (Start of OS-specific)\n");                   break;
            case 0x6474e550: printf("PT_GNU_EH_FRAME (GCC .eh_frame_hdr segment)\n");      break;
            case 0x6474e551: printf("PT_GNU_STACK (Indicates stack executability)\n");     break;
            case 0x6474e552: printf("PT_GNU_RELRO (Read-only after relocation)\n");        break;
            case 0x6474e553: printf("PT_GNU_PROPERTY (GNU property)\n");                   break;
            case 0x6ffffffa: printf("PT_LOSUNW or PT_SUNWBSS (Sun Specific segment)\n");   break;
            case 0x6ffffffb: printf("PT_SUNWSTACK (Stack segment)\n");                     break;
            case 0x6fffffff: printf("PT_HISUNW or PT_HIOS (End of OS-specific)\n");        break;
            case 0x70000000: printf("PT_LOPROC (Start of processor-specific)\n");          break;
            case 0x7fffffff: printf("PT_HIPROC (End of processor-specific)\n");            break;
        }
        printf("\t\t\t\t\tp_flags (segment flags) is ");
        switch(ftrace_elf_program_header[i].p_flags){
            default:         printf("Unknown flag\n");                                     break;
            case (1 << 0):   printf("PF_X (Segment is executable)\n");                     break;
            case (1 << 1):   printf("PF_W (Segment is writable)\n");                       break;
            case (1 << 2):   printf("PF_R (Segment is readable)\n");                       break;
            case 0x0ff00000: printf("PF_MASKOS (OS-specific)\n");                          break;
            case 0xf0000000: printf("PF_MASKPROC (Processor-specific)\n");                 break;
        }
        printf("\t\t\t\t\tp_offset (Segment file offset) is 0x%lx\n", ftrace_elf_program_header[i].p_offset);
        printf("\t\t\t\t\tp_vaddr (Segment virtual address) is 0x%lx\n", ftrace_elf_program_header[i].p_vaddr);
        printf("\t\t\t\t\tp_paddr (Segment physical address) is 0x%lx\n", ftrace_elf_program_header[i].p_paddr);
        printf("\t\t\t\t\tp_filesz (Segment size in file) is 0x%lx\n", ftrace_elf_program_header[i].p_filesz);
        printf("\t\t\t\t\tp_memsz (Segment size in memory) is 0x%lx\n", ftrace_elf_program_header[i].p_memsz);
        printf("\t\t\t\t\tp_align (Segment alignment) is 0x%lx\n", ftrace_elf_program_header[i].p_align);
    }

    Elf64_Shdr * ftrace_elf_section_header = (Elf64_Shdr*)malloc(sizeof(Elf64_Shdr) * ftrace_efl_header.e_shnum);
    assert(ftrace_elf_section_header != NULL);
    //ftrace_check_elf = 

    

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
    return;
}