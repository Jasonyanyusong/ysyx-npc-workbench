/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <stdio.h>
#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include "math.h"
// #include "sdb.h"
// #include <stdlib.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

u_int32_t instruction_value = 0;

enum {
  TYPE_R, TYPE_I, TYPE_S, TYPE_B, TYPE_U, TYPE_J, TYPE_R4, 
  TYPE_N, // none
};

bool riscv64_instC_Print_Debug = true;
bool riscv64_instC_Print_ChecKPoinT = true;
bool riscv64_instC_Print_Instruction = true;

#define src1R() do { *src1 = R(rs1); } while (0) // Skeleton Code
#define src2R() do { *src2 = R(rs2); } while (0) // Skeleton Code
//#define src3R() do { *src3 = R(rs3); } while (0) // Referenced from src1R and src2R
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0) // Skeleton Code
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0) // Skeleton Code
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0) // Skeleton Code
// My codes (Have confusions about SEXT and BITS)
#define immB() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 12) | BITS(i, 7, 7) << 11 | BITS(i, 30, 25) << 5 | BITS(i, 11, 8) << 1;} while(0)
#define immJ() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 20) | BITS(i, 19, 12) << 12 | BITS(i, 20, 20) << 11 | BITS(i, 30, 21) << 1;} while(0)

static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  // Below is rs3's implementation, rs3 appears in RV32(64)F, RV32(64)D, RV32(64)Q and RV32(64)Zfh 
  //int rs3 = BITS(i, 31, 27);
  *rd     = BITS(i, 11, 7);
  switch (type) {
    case TYPE_R : src1R(); src2R();         break;
    case TYPE_R4: /*TODO*/                  break;
    case TYPE_I : src1R();          immI(); break;
    case TYPE_S : src1R(); src2R(); immS(); break;
    case TYPE_B : src1R(); src2R(); immB(); break;
    case TYPE_U :                   immU(); break;
    case TYPE_J :                   immJ(); break;
  }
  //printf("rs1 = 0x%x, rs2 = 0x%x\n", rs1, rs2);
}

static int decode_exec(Decode *s) {
  int rd = 0;
  word_t src1 = 0, src2 = 0, imm = 0;
  s->dnpc = s->snpc;

#ifdef CONFIG_ShowInstInfo
  char instruction_bin_string[33] = {0};
  u_int32_t get_instruction = s->isa.inst.val;
  for(int i = 0; i <= 31; i = i + 1)
  {
    if(get_instruction >= pow(2, 31 - i))
    {
      instruction_bin_string[i] = '1';
      get_instruction = get_instruction - pow(2, 31 - i);
    }
    else
    {
      instruction_bin_string[i] = '0';
    }
  }
  instruction_bin_string[32] = '\0';
#endif

#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

#ifdef CONFIG_ShowInstInfo
  int rs1 = BITS(s->isa.inst.val, 19, 15);
  int rs2 = BITS(s->isa.inst.val, 24, 20);

  printf("Inst: %s (0x%8x)\n", instruction_bin_string, s->isa.inst.val);
  printf("rs1 = 0x%x (%d), rs2 = 0x%x (%d), rd = 0x%x (%d)\n", rs1, rs1, rs2, rs2, rd, rd);
  printf("src1 = 0x%8lx (%ld), src2 = 0x%8lx (%ld), R(rd) = 0x%8lx (%ld), imm = 0x%8lx (%ld)\n", src1, src1, src2, src2, R(rd), R(rd), imm, imm);
  printf("pc = 0x%lx, dnpc = 0x%lx, snpc = 0x%lx\n", s -> pc, s -> dnpc, s -> snpc);
#endif
  // R(10) is $a0

  INSTPAT_START();
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(rd) = imm);
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(rd) = s -> pc + imm);
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, R(rd) = s -> pc + 4, s -> dnpc = s -> pc + imm);
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, s -> dnpc = (src1 + imm) & ~ 1, R(rd) = s -> pc + 4);
  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, s -> dnpc = src1 == src2 ? s -> pc + imm : s -> pc + 4);
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, s -> dnpc = src1 != src2 ? s -> pc + imm : s -> pc + 4);
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, s -> dnpc = (signed)src1 <  (signed)src2 ? s -> pc + imm : s -> pc + 4);
  INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, s -> dnpc = (signed)src1 >= (signed)src2 ? s -> pc + imm : s -> pc + 4);
  INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, s -> dnpc = (unsigned)src1 <  (unsigned)src2 ? s -> pc + imm : s -> pc + 4);
  INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, s -> dnpc = (unsigned)src1 >= (unsigned)src2 ? s -> pc + imm : s -> pc + 4);
  INSTPAT("??????? ????? ????? 000 ????? 00000 11", lb     , I, R(rd) = SEXT(Mr(src1 + imm, 1), 8));
  INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh     , I, R(rd) = SEXT(Mr(src1 + imm, 2), 16));
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(rd) = SEXT(Mr(src1 + imm, 4), 32));
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(rd) = Mr(src1 + imm, 1) & 0xFFFFFFFF);
  INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu    , I, R(rd) = Mr(src1 + imm, 2) & 0xFFFFFFFF);
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));
  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, src2));
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2));
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(rd) = src1 + imm);
  INSTPAT("??????? ????? ????? 010 ????? 00100 11", slti   , I, R(rd) = (signed)src1 < (signed)imm);
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, R(rd) = (unsigned)src1 < (unsigned)imm);
  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(rd) = src1 ^ imm);
  INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori    , I, R(rd) = src1 | imm);
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(rd) = src1 & imm);
  INSTPAT("000000? ????? ????? 001 ????? 00100 11", slli   , I, R(rd) = src1 << BITS(s->isa.inst.val, 25, 20));
  INSTPAT("000000? ????? ????? 101 ????? 00100 11", srli   , I, R(rd) = src1 >> BITS(s->isa.inst.val, 25, 20));
  INSTPAT("010000? ????? ????? 101 ????? 00100 11", srai   , I, R(rd) = (signed)src1 >> BITS(s->isa.inst.val, 25, 20));
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(rd) = src1 + src2);
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(rd) = src1 - src2);
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R, R(rd) = (unsigned)src1 << (src2 & 0b111111));
  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, R(rd) = (signed)src1 < (signed)src2);
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, R(rd) = (unsigned)src1 < (unsigned)src2);
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, R(rd) = src1 ^ src2);
  INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl    , R, R(rd) = (unsigned)src1 >> (src2 & 0b111111));
  INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra    , R, R(rd) = (signed)src1 >> src2& 0b111111);
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(rd) = src1 | src2);
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(rd) = src1 & src2);
  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10)));
  INSTPAT("??????? ????? ????? 110 ????? 00000 11", lwu    , I, R(rd) = Mr(src1 + imm, 4) & 0xFFFFFFFF);
  INSTPAT("??????? ????? ????? 011 ????? 00000 11", ld     , I, R(rd) = Mr(src1 + imm, 8));
  INSTPAT("??????? ????? ????? 011 ????? 01000 11", sd     , S, Mw(src1 + imm, 8, src2));
  INSTPAT("??????? ????? ????? 000 ????? 00110 11", addiw  , I, R(rd) = SEXT(BITS((signed)(src1 + imm), 31, 0), 64));
  INSTPAT("0000000 ????? ????? 001 ????? 00110 11", slliw  , R, R(rd) = SEXT((unsigned)BITS(src1, 31, 0) << BITS(s->isa.inst.val, 24, 20), 64));
  INSTPAT("0000000 ????? ????? 101 ????? 00110 11", srliw  , R, R(rd) = SEXT((unsigned)BITS(src1, 31, 0) >> BITS(s->isa.inst.val, 24, 20), 64));
  INSTPAT("0100000 ????? ????? 101 ????? 00110 11", sraiw  , R, R(rd) = SEXT((signed)BITS(src1, 31, 0) >> BITS(s->isa.inst.val, 24, 20), 64));
  INSTPAT("0000000 ????? ????? 000 ????? 01110 11", addw   , R, R(rd) = SEXT(BITS((signed)(src1 + src2), 31, 0), 64));
  INSTPAT("0100000 ????? ????? 000 ????? 01110 11", subw   , R, R(rd) = SEXT(BITS((signed)(src1 - src2), 31, 0), 64));
  INSTPAT("0000000 ????? ????? 001 ????? 01110 11", sllw   , R, R(rd) = SEXT(BITS((unsigned)BITS(src1, 31, 0) << src2, 31, 0), 64));
  INSTPAT("0000000 ????? ????? 101 ????? 01110 11", srlw   , R, R(rd) = SEXT((unsigned)BITS(src1, 31, 0) >> src2, 64));
  INSTPAT("0100000 ????? ????? 101 ????? 01110 11", sraw   , R, R(rd) = SEXT((signed)BITS(src1, 31, 0) >> src2, 64));
  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, R(rd) = BITS(src1 * src2, 63, 0));
  // INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh   , R, R(rd) = BITS((signed)src1 * (signed)src2, 127, 64));
  // INSTPAT("0000001 ????? ????? 010 ????? 01100 11", mulhsu , R, R(rd) = BITS((signed)src1 * (unsigned)src2, 127, 64));
  // INSTPAT("0000001 ????? ????? 011 ????? 01100 11", mulhu  , R, R(rd) = BITS((unsigned)src1 * (unsigned)src2, 127, 64));
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div    , R, R(rd) = (signed)src1 / (signed)src2);
  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu   , R, R(rd) = (unsigned)src1 / (unsigned)src2);
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem    , R, R(rd) = (signed)src1 % (signed)src2);
  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu   , R, R(rd) = (unsigned)src1 % (unsigned)src2);
  INSTPAT("0000001 ????? ????? 000 ????? 01110 11", mulw   , R, R(rd) = SEXT(BITS(BITS(src1, 31, 0) * BITS(src2, 31, 0), 31, 0), 64));
  INSTPAT("0000001 ????? ????? 100 ????? 01110 11", divw   , R, R(rd) = SEXT((signed)BITS(src1, 31, 0) / (signed)BITS(src2, 31, 0), 64));
  INSTPAT("0000001 ????? ????? 101 ????? 01110 11", divuw  , R, R(rd) = SEXT((unsigned)BITS(src1, 31, 0) / (unsigned)BITS(src2, 31, 0), 64));
  INSTPAT("0000001 ????? ????? 110 ????? 01110 11", remw   , R, R(rd) = SEXT((signed)BITS(src1, 31, 0) % (signed)BITS(src2, 31, 0), 64));
  INSTPAT("0000001 ????? ????? 111 ????? 01110 11", remuw  , R, R(rd) = SEXT((unsigned)BITS(src1, 31, 0) % (unsigned)BITS(src2, 31, 0), 64));
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

#ifdef CONFIG_ShowInstInfo
  printf("Inst: %s (0x%8x)\n", instruction_bin_string, s->isa.inst.val);
  printf("rs1 = 0x%x (%d), rs2 = 0x%x (%d), rd = 0x%x (%d)\n", rs1, rs1, rs2, rs2, rd, rd);
  printf("src1 = 0x%8lx (%ld), src2 = 0x%8lx (%ld), R(rd) = 0x%8lx (%ld), imm = 0x%8lx (%ld)\n", src1, src1, src2, src2, R(rd), R(rd), imm, imm);
  printf("pc = 0x%lx, dnpc = 0x%lx, snpc = 0x%lx\n", s -> pc, s -> dnpc, s -> snpc);
#endif

  return 0;
}

int isa_exec_once(Decode *s) {
#ifdef CONFIG_ShowInstInfo
  printf("********************************************************************************************************************************\n");
#endif
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}