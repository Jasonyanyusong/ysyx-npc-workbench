#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV32__) || defined(__ISA_RISCV64__)
# define EXPECT_TYPE EM_RISCV
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  // In PA3.2 just treat pcb and filename as NULL

  // Read ELF Header
  Elf_Ehdr Ehdr;
  int Ehdr_readSize = -1;
  Ehdr_readSize = ramdisk_read(&Ehdr, 0, sizeof(Elf_Ehdr));
  Log("Read ELF Header with size %d", Ehdr_readSize);
  assert(Ehdr_readSize == sizeof(Elf_Ehdr));

  // Check ELF file
  assert(Ehdr.e_ident[EI_MAG0] == ELFMAG0);
  assert(Ehdr.e_ident[EI_MAG1] == ELFMAG1);
  assert(Ehdr.e_ident[EI_MAG2] == ELFMAG2);
  assert(Ehdr.e_ident[EI_MAG3] == ELFMAG3);

  assert(Ehdr.e_machine == EXPECT_TYPE);

  Log("Program Header offset is 0x%x", Ehdr.e_phoff);
  Log("Program Header number is %d", Ehdr.e_phnum);

  Elf_Phdr Phdr[Ehdr.e_phnum];
  int Phdr_readSize = ramdisk_read(&Phdr, Ehdr.e_phoff, Ehdr.e_phnum * sizeof(Elf_Phdr));
  Log("Read ELF Function data to Phdr with size %d", Phdr_readSize);

  for(int i = 0; i < Ehdr.e_phnum; i = i + 1){
    if(Phdr[i].p_type == PT_LOAD){
      // This is a LOAD segment, so we need to load to memory
      Log("Phdr[%d] holds a loadable segment", i);
      Log("\tFile Size = %d", Phdr[i].p_filesz);
      Log("\tMem  Size = %d", Phdr[i].p_memsz);
      Log("\tPhys Addr = 0x%x", Phdr[i].p_paddr);
      Log("\tVirt Size = 0x%x", Phdr[i].p_vaddr);

      ramdisk_read((void *)Phdr[i].p_vaddr, Phdr[i].p_offset, Phdr[i].p_memsz);
      Log("\tFinished: Write [VirtAddr, VirtAddr + MemSiz) To Memory");

      memset((void *)(Phdr[i].p_vaddr + Phdr[i].p_filesz), 0, Phdr[i].p_memsz - Phdr[i].p_filesz);
      Log("\tFinished: Clear [VirtAddr + FileSiz, VirtAddr + MemSiz) To 0");
    }
  }
  
  return Ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

