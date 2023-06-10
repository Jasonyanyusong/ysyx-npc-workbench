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
#define CSR(i) csr(i)
#define Mr vaddr_read
#define Mw vaddr_write

enum {
  TYPE_R, TYPE_I, TYPE_S, TYPE_B, TYPE_U, TYPE_J, TYPE_R4, 
  TYPE_N, // none
};

#define src1R() do { *src1 = R(rs1); } while (0) // Skeleton Code
#define src2R() do { *src2 = R(rs2); } while (0) // Skeleton Code
//#define src3R() do { *src3 = R(rs3); } while (0) // Referenced from src1R and src2R
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0) // Skeleton Code
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0) // Skeleton Code
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0) // Skeleton Code
#define immB() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 12) | BITS(i, 7, 7) << 11 | BITS(i, 30, 25) << 5 | BITS(i, 11, 8) << 1;} while(0)
#define immJ() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 20) | BITS(i, 19, 12) << 12 | BITS(i, 20, 20) << 11 | BITS(i, 30, 21) << 1;} while(0)

static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
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
}

static int decode_exec(Decode *s) {
#ifdef CONFIG_DIFFTEST_OBSERVE_MODE
  usleep(1000000);
#endif
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

  int rs1 = BITS(s->isa.inst.val, 19, 15);
  int rs2 = BITS(s->isa.inst.val, 24, 20);

#ifdef CONFIG_RV64A
  bool aq = BITS(s->isa.inst.val, 26, 26);
  bool rl = BITS(s->isa.inst.val, 25, 25);
  printf("aq = %d, rl = %d\n", aq, rl);
#endif
  // R(10) is $a0

  INSTPAT_START();
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, IFDEF(CONFIG_ShowInstName, printf("LUI\n"));    R(rd) = imm);
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, IFDEF(CONFIG_ShowInstName, printf("AUIPC\n"));  R(rd) = s -> pc + imm);
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, IFDEF(CONFIG_ShowInstName, printf("JAL\n"));    R(rd) = s -> pc + 4; s -> dnpc = s -> pc + imm);
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, IFDEF(CONFIG_ShowInstName, printf("JALR\n"));   R(rd) = s -> pc + 4; s -> dnpc = (src1 + imm) & ~ 1);
  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, IFDEF(CONFIG_ShowInstName, printf("BEQ\n"));    s -> dnpc = src1 == src2 ? s -> pc + imm : s -> pc + 4);
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, IFDEF(CONFIG_ShowInstName, printf("BNE\n"));    s -> dnpc = src1 != src2 ? s -> pc + imm : s -> pc + 4);
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, IFDEF(CONFIG_ShowInstName, printf("BLT\n"));    s -> dnpc = (signed)src1 <  (signed)src2 ? s -> pc + imm : s -> pc + 4);
  INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, IFDEF(CONFIG_ShowInstName, printf("BGE\n"));    s -> dnpc = (signed)src1 >= (signed)src2 ? s -> pc + imm : s -> pc + 4);
  INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, IFDEF(CONFIG_ShowInstName, printf("BLTU\n"));   s -> dnpc = (unsigned)src1 <  (unsigned)src2 ? s -> pc + imm : s -> pc + 4);
  INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, IFDEF(CONFIG_ShowInstName, printf("BGEU\n"));   s -> dnpc = (unsigned)src1 >= (unsigned)src2 ? s -> pc + imm : s -> pc + 4);
  INSTPAT("??????? ????? ????? 000 ????? 00000 11", lb     , I, IFDEF(CONFIG_ShowInstName, printf("LB\n"));     R(rd) = SEXT(Mr(src1 + imm, 1), 8));
  INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh     , I, IFDEF(CONFIG_ShowInstName, printf("LH\n"));     R(rd) = SEXT(Mr(src1 + imm, 2), 16));
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, IFDEF(CONFIG_ShowInstName, printf("LW\n"));     R(rd) = SEXT(Mr(src1 + imm, 4), 32));
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, IFDEF(CONFIG_ShowInstName, printf("LBU\n"));    R(rd) = Mr(src1 + imm, 1));
  INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu    , I, IFDEF(CONFIG_ShowInstName, printf("LHU\n"));    R(rd) = Mr(src1 + imm, 2));
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, IFDEF(CONFIG_ShowInstName, printf("SB\n"));     Mw(src1 + imm, 1, src2));
  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, IFDEF(CONFIG_ShowInstName, printf("SH\n"));     Mw(src1 + imm, 2, src2));
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, IFDEF(CONFIG_ShowInstName, printf("SW\n"));     Mw(src1 + imm, 4, src2));
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, IFDEF(CONFIG_ShowInstName, printf("ADDI\n"));   R(rd) = src1 + imm);
  INSTPAT("??????? ????? ????? 010 ????? 00100 11", slti   , I, IFDEF(CONFIG_ShowInstName, printf("SLTI\n"));   R(rd) = (signed)src1 < (signed)imm);
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, IFDEF(CONFIG_ShowInstName, printf("SLTIU\n"));  R(rd) = (unsigned)src1 < (unsigned)imm);
  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, IFDEF(CONFIG_ShowInstName, printf("XORI\n"));   R(rd) = src1 ^ imm);
  INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori    , I, IFDEF(CONFIG_ShowInstName, printf("ORI\n"));    R(rd) = src1 | imm);
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, IFDEF(CONFIG_ShowInstName, printf("ANDI\n"));   R(rd) = src1 & imm);
  INSTPAT("000000? ????? ????? 001 ????? 00100 11", slli   , I, IFDEF(CONFIG_ShowInstName, printf("SLLI\n"));   R(rd) = src1 << (imm & 0b111111));
  INSTPAT("000000? ????? ????? 101 ????? 00100 11", srli   , I, IFDEF(CONFIG_ShowInstName, printf("SRLI\n"));   R(rd) = src1 >> (imm & 0b111111));
  INSTPAT("010000? ????? ????? 101 ????? 00100 11", srai   , I, IFDEF(CONFIG_ShowInstName, printf("SRAI\n"));   R(rd) = (int64_t)src1 >> BITS(s->isa.inst.val, 25, 20));
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, IFDEF(CONFIG_ShowInstName, printf("ADD\n"));    R(rd) = src1 + src2);
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, IFDEF(CONFIG_ShowInstName, printf("SUB\n"));    R(rd) = src1 - src2);
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R, IFDEF(CONFIG_ShowInstName, printf("SLL\n"));    R(rd) = src1 << (src2 & 0b111111));
  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, IFDEF(CONFIG_ShowInstName, printf("SLT\n"));    R(rd) = (int64_t)src1 < (int64_t)src2);
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, IFDEF(CONFIG_ShowInstName, printf("SLTU\n"));   R(rd) = (uint64_t)src1 < (uint64_t)src2);
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, IFDEF(CONFIG_ShowInstName, printf("XOR\n"));    R(rd) = src1 ^ src2);
  INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl    , R, IFDEF(CONFIG_ShowInstName, printf("SRL\n"));    R(rd) = (uint64_t)src1 >> (src2 & 0b111111));
  INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra    , R, IFDEF(CONFIG_ShowInstName, printf("SRA\n"));    R(rd) = (int64_t)src1 >> (src2 & 0b111111));
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, IFDEF(CONFIG_ShowInstName, printf("OR\n"));     R(rd) = src1 | src2);
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, IFDEF(CONFIG_ShowInstName, printf("AND\n"));    R(rd) = src1 & src2);
  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, IFDEF(CONFIG_ShowInstName, printf("EBREAK\n")); NEMUTRAP(s->pc, R(10)));
  INSTPAT("0000000 00000 00000 000 00000 11100 11", ecall  , N, IFDEF(CONFIG_ShowInstName, printf("ECALL\n"));  s -> dnpc = isa_raise_intr(R(17), s -> pc));

  INSTPAT("??????? ????? ????? 110 ????? 00000 11", lwu    , I, IFDEF(CONFIG_ShowInstName, printf("LWU\n"));    R(rd) = Mr(src1 + imm, 4));
  INSTPAT("??????? ????? ????? 011 ????? 00000 11", ld     , I, IFDEF(CONFIG_ShowInstName, printf("LD\n"));     R(rd) = Mr(src1 + imm, 8));
  INSTPAT("??????? ????? ????? 011 ????? 01000 11", sd     , S, IFDEF(CONFIG_ShowInstName, printf("SD\n"));     Mw(src1 + imm, 8, src2));
  INSTPAT("??????? ????? ????? 000 ????? 00110 11", addiw  , I, IFDEF(CONFIG_ShowInstName, printf("ADDIW\n"));  R(rd) = SEXT(BITS((signed)(src1 + imm), 31, 0), 32));
  INSTPAT("0000000 ????? ????? 001 ????? 00110 11", slliw  , R, IFDEF(CONFIG_ShowInstName, printf("SLLIW\n"));  R(rd) = SEXT((unsigned)BITS(src1, 31, 0) << BITS(s->isa.inst.val, 24, 20), 32));
  INSTPAT("0000000 ????? ????? 101 ????? 00110 11", srliw  , R, IFDEF(CONFIG_ShowInstName, printf("SRLIW\n"));  R(rd) = SEXT((unsigned)BITS(src1, 31, 0) >> BITS(s->isa.inst.val, 24, 20), 32));
  INSTPAT("0100000 ????? ????? 101 ????? 00110 11", sraiw  , R, IFDEF(CONFIG_ShowInstName, printf("SRAIW\n"));  R(rd) = SEXT((signed)BITS(src1, 31, 0) >> BITS(s->isa.inst.val, 24, 20), 32));
  INSTPAT("0000000 ????? ????? 000 ????? 01110 11", addw   , R, IFDEF(CONFIG_ShowInstName, printf("ADDW\n"));   R(rd) = SEXT(BITS((signed)(src1 + src2), 31, 0), 32));
  INSTPAT("0100000 ????? ????? 000 ????? 01110 11", subw   , R, IFDEF(CONFIG_ShowInstName, printf("SUBW\n"));   R(rd) = SEXT(BITS((signed)(src1 - src2), 31, 0), 32));
  INSTPAT("0000000 ????? ????? 001 ????? 01110 11", sllw   , R, IFDEF(CONFIG_ShowInstName, printf("SLLW\n"));   R(rd) = SEXT(BITS((unsigned)BITS(src1, 31, 0) << src2, 31, 0), 32));
  INSTPAT("0000000 ????? ????? 101 ????? 01110 11", srlw   , R, IFDEF(CONFIG_ShowInstName, printf("SRLW\n"));   R(rd) = SEXT((unsigned)BITS(src1, 31, 0) >> src2, 32));
  INSTPAT("0100000 ????? ????? 101 ????? 01110 11", sraw   , R, IFDEF(CONFIG_ShowInstName, printf("SRAW\n"));   R(rd) = SEXT((signed)BITS(src1, 31, 0) >> src2, 32));

  INSTPAT("??????? ????? ????? 001 ????? 11100 11", csrrw  , I, IFDEF(CONFIG_ShowInstName, printf("CSRRW\n"));  uint64_t oldCSR = CSR(imm); CSR(imm) = src1; R(rd) = oldCSR);
  INSTPAT("??????? ????? ????? 010 ????? 11100 11", csrrs  , I, IFDEF(CONFIG_ShowInstName, printf("CSRRS\n"));  uint64_t oldCSR = CSR(imm); CSR(imm) = src1 | oldCSR; R(rd) = oldCSR);
  INSTPAT("??????? ????? ????? 011 ????? 11100 11", csrrc  , I, IFDEF(CONFIG_ShowInstName, printf("CSRRC\n"));  uint64_t oldCSR = CSR(imm); CSR(imm) = src1 & oldCSR; R(rd) = oldCSR);
  INSTPAT("??????? ????? ????? 101 ????? 11100 11", csrrwi , I, IFDEF(CONFIG_ShowInstName, printf("CSRRWI\n")); uint64_t oldCSR = CSR(imm); CSR(imm) = rs1; R(rd) = oldCSR);
  INSTPAT("??????? ????? ????? 110 ????? 11100 11", csrrsi , I, IFDEF(CONFIG_ShowInstName, printf("CSRRSI\n")); uint64_t oldCSR = CSR(imm); CSR(imm) = rs1 | oldCSR; R(rd) = oldCSR);
  INSTPAT("??????? ????? ????? 111 ????? 11100 11", csrrci , I, IFDEF(CONFIG_ShowInstName, printf("CSRRCI\n")); uint64_t oldCSR = CSR(imm); CSR(imm) = rs1 & oldCSR; R(rd) = oldCSR);

  // RV64M
  #ifdef CONFIG_RV64M
  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, IFDEF(CONFIG_ShowInstName, printf("MUL\n"));    R(rd) = BITS(src1 * src2, 63, 0));
  INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh   , R, IFDEF(CONFIG_ShowInstName, printf("MULH\n"));   R(rd) = BITS((signed)src1 * (signed)src2, 127, 64));
  INSTPAT("0000001 ????? ????? 010 ????? 01100 11", mulhsu , R, IFDEF(CONFIG_ShowInstName, printf("MULHSU\n")); R(rd) = BITS((signed)src1 * (unsigned)src2, 127, 64));
  INSTPAT("0000001 ????? ????? 011 ????? 01100 11", mulhu  , R, IFDEF(CONFIG_ShowInstName, printf("MULHU\n"));  R(rd) = BITS((unsigned)src1 * (unsigned)src2, 127, 64));
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div    , R, IFDEF(CONFIG_ShowInstName, printf("DIV\n"));    R(rd) = (int64_t)src2 != 0 ? (int64_t)src1 / (int64_t)src2 : -1);
  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu   , R, IFDEF(CONFIG_ShowInstName, printf("DIVU\n"));   R(rd) = (uint64_t)src2 != 0 ? (uint64_t)src1 / (uint64_t)src2 : -1);
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem    , R, IFDEF(CONFIG_ShowInstName, printf("REM\n"));    R(rd) = (int64_t)src2 != 0 ? (int64_t)src1 % (int64_t)src2 : (int64_t)src1);
  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu   , R, IFDEF(CONFIG_ShowInstName, printf("REMU\n"));   R(rd) = (uint64_t)src2 != 0 ? (uint64_t)src1 % (uint64_t)src2 : (uint64_t)src1);
  INSTPAT("0000001 ????? ????? 000 ????? 01110 11", mulw   , R, IFDEF(CONFIG_ShowInstName, printf("MULW\n"));   R(rd) = SEXT(BITS(BITS(src1, 31, 0) * BITS(src2, 31, 0), 31, 0), 32));
  INSTPAT("0000001 ????? ????? 100 ????? 01110 11", divw   , R, IFDEF(CONFIG_ShowInstName, printf("DIVW\n"));   R(rd) = (signed)BITS(src2, 31, 0) != 0 ? SEXT((signed)BITS(src1, 31, 0) / (signed)BITS(src2, 31, 0), 32) : -1);
  INSTPAT("0000001 ????? ????? 101 ????? 01110 11", divuw  , R, IFDEF(CONFIG_ShowInstName, printf("DIVUW\n"));  R(rd) = (unsigned)BITS(src2, 31, 0) != 0 ? SEXT((unsigned)BITS(src1, 31, 0) / (unsigned)BITS(src2, 31, 0), 32) : -1);
  INSTPAT("0000001 ????? ????? 110 ????? 01110 11", remw   , R, IFDEF(CONFIG_ShowInstName, printf("REMW\n"));   R(rd) = (signed)BITS(src2, 31, 0) != 0 ? SEXT((signed)BITS(src1, 31, 0) % (signed)BITS(src2, 31, 0), 32) : SEXT((signed)BITS(src1, 31, 0), 32));
  INSTPAT("0000001 ????? ????? 111 ????? 01110 11", remuw  , R, IFDEF(CONFIG_ShowInstName, printf("REMUW\n"));  R(rd) = (unsigned)BITS(src2, 31, 0) != 0 ? SEXT((unsigned)BITS(src1, 31, 0) % (unsigned)BITS(src2, 31, 0), 32) : SEXT((unsigned)BITS(src1, 31, 0), 32));
  #endif

  // RV64A
  #ifdef CONFIG_RV64A
  INSTPAT("00010?? 00000 ????? 010 ????? 01011 11", lr_w     , R, IFDEF(CONFIG_ShowInstName, printf("LR.W\n"));        assert(0));
  INSTPAT("00011?? ????? ????? 010 ????? 01011 11", sc_w     , R, IFDEF(CONFIG_ShowInstName, printf("SC.W\n"));        assert(0));
  INSTPAT("00001?? ????? ????? 010 ????? 01011 11", amoswap_w, R, IFDEF(CONFIG_ShowInstName, printf("AMOSWAP.W\n"));   assert(0));
  INSTPAT("00000?? ????? ????? 010 ????? 01011 11", amoadd_w , R, IFDEF(CONFIG_ShowInstName, printf("AMOADD.W\n"));    assert(0));
  INSTPAT("00100?? ????? ????? 010 ????? 01011 11", amoxor_w , R, IFDEF(CONFIG_ShowInstName, printf("AMOXOR.W\n"));    assert(0));
  INSTPAT("01100?? ????? ????? 010 ????? 01011 11", amoand_w , R, IFDEF(CONFIG_ShowInstName, printf("AMOAND.W\n"));    assert(0));
  INSTPAT("01000?? ????? ????? 010 ????? 01011 11", amoor_w  , R, IFDEF(CONFIG_ShowInstName, printf("AMOOR.W\n"));     assert(0));
  INSTPAT("10000?? ????? ????? 010 ????? 01011 11", amomin_w , R, IFDEF(CONFIG_ShowInstName, printf("AMOMIN.W\n"));    assert(0));
  INSTPAT("10100?? ????? ????? 010 ????? 01011 11", amomax_w , R, IFDEF(CONFIG_ShowInstName, printf("AMOMAX.W\n"));    assert(0));
  INSTPAT("11000?? ????? ????? 010 ????? 01011 11", amominu_w, R, IFDEF(CONFIG_ShowInstName, printf("AMOMINU.W\n"));   assert(0));
  INSTPAT("11100?? ????? ????? 010 ????? 01011 11", amomaxu_w, R, IFDEF(CONFIG_ShowInstName, printf("AMOMAXU.W\n"));   assert(0));

  INSTPAT("00010?? 00000 ????? 011 ????? 01011 11", lr_d     , R, IFDEF(CONFIG_ShowInstName, printf("LR.D\n"));        assert(0));
  INSTPAT("00011?? ????? ????? 011 ????? 01011 11", sc_d     , R, IFDEF(CONFIG_ShowInstName, printf("SC.D\n"));        assert(0));
  INSTPAT("00001?? ????? ????? 011 ????? 01011 11", amoswap_d, R, IFDEF(CONFIG_ShowInstName, printf("AMOSWAP.D\n"));   assert(0));
  INSTPAT("00000?? ????? ????? 011 ????? 01011 11", amoadd_d , R, IFDEF(CONFIG_ShowInstName, printf("AMOADD.D\n"));    assert(0));
  INSTPAT("00100?? ????? ????? 011 ????? 01011 11", amoxor_d , R, IFDEF(CONFIG_ShowInstName, printf("AMOXOR.D\n"));    assert(0));
  INSTPAT("01100?? ????? ????? 011 ????? 01011 11", amoand_d , R, IFDEF(CONFIG_ShowInstName, printf("AMOAND.D\n"));    assert(0));
  INSTPAT("01000?? ????? ????? 011 ????? 01011 11", amoor_d  , R, IFDEF(CONFIG_ShowInstName, printf("AMOOR.D\n"));     assert(0));
  INSTPAT("10000?? ????? ????? 011 ????? 01011 11", amomin_d , R, IFDEF(CONFIG_ShowInstName, printf("AMOMIN.D\n"));    assert(0));
  INSTPAT("10100?? ????? ????? 011 ????? 01011 11", amomax_d , R, IFDEF(CONFIG_ShowInstName, printf("AMOMAX.D\n"));    assert(0));
  INSTPAT("11000?? ????? ????? 011 ????? 01011 11", amominu_d, R, IFDEF(CONFIG_ShowInstName, printf("AMOMINU.D\n"));   assert(0));
  INSTPAT("11100?? ????? ????? 011 ????? 01011 11", amomaxu_d, R, IFDEF(CONFIG_ShowInstName, printf("AMOMAXU.D\n"));   assert(0));
  #endif

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
