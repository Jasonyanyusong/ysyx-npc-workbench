#include <proc.h>
#include <elf.h>
#include <fs.h>

//#define LOADER_LOG

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
  // In PA3.3 need to add fs_open to get the file number and call fs_read to read from ramdisk

  // open file
  int fileNo = fs_open(filename);
  assert(fileNo >= 0);

  // Read ELF Header using fs_read()
  Elf_Ehdr Ehdr;
  int Ehdr_readSize = -1;
  //Ehdr_readSize = ramdisk_read(&Ehdr, 0, sizeof(Elf_Ehdr));
  Ehdr_readSize = fs_read(fileNo, &Ehdr, sizeof(Elf_Ehdr));
  #ifdef LOADER_LOG
  Log("Read ELF Header with size %d", Ehdr_readSize);
  #endif
  assert(Ehdr_readSize == sizeof(Elf_Ehdr));

  // Check ELF file
  assert(Ehdr.e_ident[EI_MAG0] == ELFMAG0);
  assert(Ehdr.e_ident[EI_MAG1] == ELFMAG1);
  assert(Ehdr.e_ident[EI_MAG2] == ELFMAG2);
  assert(Ehdr.e_ident[EI_MAG3] == ELFMAG3);

  assert(Ehdr.e_machine == EXPECT_TYPE);

  #ifdef LOADER_LOG
  Log("Program Header offset is 0x%x", Ehdr.e_phoff);
  Log("Program Header number is %d", Ehdr.e_phnum);
  #endif

  Elf_Phdr Phdr[Ehdr.e_phnum];
  //int Phdr_readSize = ramdisk_read(&Phdr, Ehdr.e_phoff, Ehdr.e_phnum * sizeof(Elf_Phdr));
  fs_lseek(fileNo, Ehdr.e_phoff, SEEK_SET);
  int Phdr_readSize = fs_read(fileNo, Phdr, Ehdr.e_phnum * sizeof(Elf_Phdr));
  #ifdef LOADER_LOG
  Log("Read ELF Function data to Phdr with size %d", Phdr_readSize);
  #endif

  for(int i = 0; i < Ehdr.e_phnum; i = i + 1){
    if(Phdr[i].p_type == PT_LOAD){
      // This is a LOAD segment, so we need to load to memory
      #ifdef LOADER_LOG
      Log("Phdr[%d] holds a loadable segment", i);
      Log("\tFile Size = %d", Phdr[i].p_filesz);
      Log("\tMem  Size = %d", Phdr[i].p_memsz);
      Log("\tPhys Addr = 0x%x", Phdr[i].p_paddr);
      Log("\tVirt Size = 0x%x", Phdr[i].p_vaddr);
      #endif

      //ramdisk_read((void *)Phdr[i].p_vaddr, Phdr[i].p_offset, Phdr[i].p_memsz);
      fs_lseek(fileNo, Phdr[i].p_offset, SEEK_SET);
      fs_read(fileNo, (void *)Phdr[i].p_vaddr, Phdr[i].p_memsz);
      #ifdef LOADER_LOG
      Log("\tFinished: Write [VirtAddr, VirtAddr + MemSiz) To Memory");
      #endif

      memset((void *)(Phdr[i].p_vaddr + Phdr[i].p_filesz), 0, Phdr[i].p_memsz - Phdr[i].p_filesz);
      #ifdef LOADER_LOG
      Log("\tFinished: Clear [VirtAddr + FileSiz, VirtAddr + MemSiz) To 0");
      #endif
    }
  }
  
  return Ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

