/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing 101 High School
* Copyright (c) 2023 Yusong Yan, University of Washington - Seattle
*
* YSYX-NPCA is licensed under Mulan PSL v2.
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

package npca
import chisel3._
import chisel3.util._

object inst_types{
    // We will use a four bit representation of inst types, so it will be very easy to add/modify or optimize our codes
    // Enum all RISC-V Instruction types, so Instruction Decode Unit will know how to devide immediate number, how to get rs and rd
    def inst_R = 0.U(4.W)
    def inst_I = 1.U(4.W)
    def inst_S = 2.U(4.W)
    def inst_B = 3.U(4.W)
    def inst_U = 4.U(4.W)
    def inst_J = 5.U(4.W)
    def inst_N = 6.U(4.W) // ebreak, ecall, mret
    def inst_E = 7.U(4.W) // E indicates that we cought an invalid instruction
}

object rv64_opcodes{
    // We will use aa ten bit representation of opreation codes, so if need later add inst, this is OK (support up to 1024 different instructions)
    // Both EXU and LSU will use this opcode, for LSU, we can set default to DoNothing if the inst is not load-store related
    // This module is more likely to be used in debugging, IDU will not use that, it will generate a better opcode send to EXU and LSU

    // RV64I
    def opcode_DoNothing =  0.U(10.W)
    def opcode_LUI       =  1.U(10.W)
    def opcode_AUIPC     =  2.U(10.W)
    def opcode_JAL       =  3.U(10.W)
    def opcode_JALR      =  4.U(10.W)
    def opcode_BEQ       =  5.U(10.W)
    def opcode_BNE       =  6.U(10.W)
    def opcode_BLT       =  7.U(10.W)
    def opcode_BGE       =  8.U(10.W)
    def opcode_BLTU      =  9.U(10.W)
    def opcode_BGEU      = 10.U(10.W)
    def opcode_LB        = 11.U(10.W)
    def opcode_LH        = 12.U(10.W)
    def opcode_LW        = 13.U(10.W)
    def opcode_LBU       = 14.U(10.W)
    def opcode_LHU       = 15.U(10.W)
    def opcode_SB        = 16.U(10.W)
    def opcode_SH        = 17.U(10.W)
    def opcode_SW        = 18.U(10.W)
    def opcode_ADDI      = 19.U(10.W)
    def opcode_SLTI      = 20.U(10.W)
    def opcode_SLTIU     = 21.U(10.W)
    def opcode_XORI      = 22.U(10.W)
    def opcode_ORI       = 23.U(10.W)
    def opcode_ANDI      = 24.U(10.W)
    def opcode_SLLI      = 25.U(10.W)
    def opcode_SRLI      = 26.U(10.W)
    def opcode_SRAI      = 27.U(10.W)
    def opcode_ADD       = 28.U(10.W)
    def opcode_SUB       = 29.U(10.W)
    def opcode_SLL       = 30.U(10.W)
    def opcode_SLT       = 31.U(10.W)
    def opcode_SLTU      = 32.U(10.W)
    def opcode_XOR       = 33.U(10.W)
    def opcode_SRL       = 34.U(10.W)
    def opcode_SRA       = 35.U(10.W)
    def opcode_OR        = 36.U(10.W)
    def opcode_AND       = 37.U(10.W)
    def opcode_LWU       = 38.U(10.W)
    def opcode_LD        = 39.U(10.W)
    def opcode_SD        = 40.U(10.W)
    def opcode_ADDIW     = 41.U(10.W)
    def opcode_SLLIW     = 42.U(10.W)
    def opcode_SRLIW     = 43.U(10.W)
    def opcode_SRAIW     = 44.U(10.W)
    def opcode_ADDW      = 45.U(10.W)
    def opcode_SUBW      = 46.U(10.W)
    def opcode_SLLW      = 47.U(10.W)
    def opcode_SRLW      = 48.U(10.W)
    def opcode_SRAW      = 49.U(10.W)

    // RV64M
    def opcode_MUL       = 50.U(10.W)
    def opcode_MULH      = 51.U(10.W)
    def opcode_MULHSU    = 52.U(10.W)
    def opcode_MULHU     = 53.U(10.W)
    def opcode_DIV       = 54.U(10.W)
    def opcode_DIVU      = 55.U(10.W)
    def opcode_REM       = 56.U(10.W)
    def opcode_REMU      = 57.U(10.W)
    def opcode_MULW      = 58.U(10.W)
    def opcode_DIVW      = 59.U(10.W)
    def opcode_DIVUW     = 60.U(10.W)
    def opcode_REMW      = 61.U(10.W)
    def opcode_REMUW     = 62.U(10.W)

    // Priv
    def opcode_ECALL     = 63.U(10.W)
    def opcode_EBREAK    = 64.U(10.W)

    // RV64 Zicsr
    def opcode_CSRRW     = 65.U(10.W)
    def opcode_CSRRS     = 66.U(10.W)
    def opcode_CSRRC     = 67.U(10.W)
    def opcode_CSRRWI    = 68.U(10.W)
    def opcode_CSRRSI    = 69.U(10.W)
    def opcode_CSRRCI    = 70.U(10.W)
}

object rv64_bitpat{
    // We will use BitPat to store and identify instruction's bit pattern
    // This will be used directly in Instruction Decode Unit

    // RV64I
    def bitpat_LUI    = BitPat("b???????_?????_?????_???_?????_01101_11")
    def bitpat_AUIPC  = BitPat("b???????_?????_?????_???_?????_00101_11")
    def bitpat_JAL    = BitPat("b???????_?????_?????_???_?????_11011_11")
    def bitpat_JALR   = BitPat("b???????_?????_?????_000_?????_11001_11")
    def bitpat_BEQ    = BitPat("b???????_?????_?????_000_?????_11000_11")
    def bitpat_BNE    = BitPat("b???????_?????_?????_001_?????_11000_11")
    def bitpat_BLT    = BitPat("b???????_?????_?????_100_?????_11000_11")
    def bitpat_BGE    = BitPat("b???????_?????_?????_101_?????_11000_11")
    def bitpat_BLTU   = BitPat("b???????_?????_?????_110_?????_11000_11")
    def bitpat_BGEU   = BitPat("b???????_?????_?????_111_?????_11000_11")
    def bitpat_LB     = BitPat("b???????_?????_?????_000_?????_00000_11")
    def bitpat_LH     = BitPat("b???????_?????_?????_001_?????_00000_11")
    def bitpat_LW     = BitPat("b???????_?????_?????_010_?????_00000_11")
    def bitpat_LBU    = BitPat("b???????_?????_?????_100_?????_00000_11")
    def bitpat_LHU    = BitPat("b???????_?????_?????_101_?????_00000_11")
    def bitpat_SB     = BitPat("b???????_?????_?????_000_?????_01000_11")
    def bitpat_SH     = BitPat("b???????_?????_?????_001_?????_01000_11")
    def bitpat_SW     = BitPat("b???????_?????_?????_010_?????_01000_11")
    def bitpat_ADDI   = BitPat("b???????_?????_?????_000_?????_00100_11")
    def bitpat_SLTI   = BitPat("b???????_?????_?????_010_?????_00100_11")
    def bitpat_SLTIU  = BitPat("b???????_?????_?????_011_?????_00100_11")
    def bitpat_XORI   = BitPat("b???????_?????_?????_100_?????_00100_11")
    def bitpat_ORI    = BitPat("b???????_?????_?????_110_?????_00100_11")
    def bitpat_ANDI   = BitPat("b???????_?????_?????_111_?????_00100_11")
    def bitpat_SLLI   = BitPat("b000000?_?????_?????_001_?????_00100_11")
    def bitpat_SRLI   = BitPat("b000000?_?????_?????_101_?????_00100_11")
    def bitpat_SRAI   = BitPat("b010000?_?????_?????_101_?????_00100_11")
    def bitpat_ADD    = BitPat("b0000000_?????_?????_000_?????_01100_11")
    def bitpat_SUB    = BitPat("b0100000_?????_?????_000_?????_01100_11")
    def bitpat_SLL    = BitPat("b0000000_?????_?????_001_?????_01100_11")
    def bitpat_SLT    = BitPat("b0000000_?????_?????_010_?????_01100_11")
    def bitpat_SLTU   = BitPat("b0000000_?????_?????_011_?????_01100_11")
    def bitpat_XOR    = BitPat("b0000000_?????_?????_100_?????_01100_11")
    def bitpat_SRL    = BitPat("b0000000_?????_?????_101_?????_01100_11")
    def bitpat_SRA    = BitPat("b0100000_?????_?????_101_?????_01100_11")
    def bitpat_OR     = BitPat("b0000000_?????_?????_110_?????_01100_11")
    def bitpat_AND    = BitPat("b0000000_?????_?????_111_?????_01100_11")
    def bitpat_LWU    = BitPat("b???????_?????_?????_110_?????_00000_11")
    def bitpat_LD     = BitPat("b???????_?????_?????_011_?????_00000_11")
    def bitpat_SD     = BitPat("b???????_?????_?????_011_?????_01000_11")
    def bitpat_ADDIW  = BitPat("b???????_?????_?????_000_?????_00110_11")
    def bitpat_SLLIW  = BitPat("b0000000_?????_?????_001_?????_00110_11")
    def bitpat_SRLIW  = BitPat("b0000000_?????_?????_101_?????_00110_11")
    def bitpat_SRAIW  = BitPat("b0100000_?????_?????_101_?????_00110_11")
    def bitpat_ADDW   = BitPat("b0000000_?????_?????_000_?????_01110_11")
    def bitpat_SUBW   = BitPat("b0100000_?????_?????_000_?????_01110_11")
    def bitpat_SLLW   = BitPat("b0000000_?????_?????_001_?????_01110_11")
    def bitpat_SRLW   = BitPat("b0000000_?????_?????_101_?????_01110_11")
    def bitpat_SRAW   = BitPat("b0100000_?????_?????_101_?????_01110_11")

    // RV64M
    def bitpat_MUL    = BitPat("b0000001_?????_?????_000_?????_01100_11")
    def bitpat_MULH   = BitPat("b0000001_?????_?????_001_?????_01100 11")
    def bitpat_MULHSU = BitPat("b0000001_?????_?????_010_?????_01100 11")
    def bitpat_MULHU  = BitPat("b0000001_?????_?????_011_?????_01100 11")
    def bitpat_DIV    = BitPat("b0000001_?????_?????_100_?????_01100 11")
    def bitpat_DIVU   = BitPat("b0000001_?????_?????_101_?????_01100 11")
    def bitpat_REM    = BitPat("b0000001_?????_?????_110_?????_01100 11")
    def bitpat_REMU   = BitPat("b0000001_?????_?????_111_?????_01100 11")
    def bitpat_MULW   = BitPat("b0000001_?????_?????_000_?????_01110 11")
    def bitpat_DIVW   = BitPat("b0000001_?????_?????_100_?????_01110 11")
    def bitpat_DIVUW  = BitPat("b0000001_?????_?????_101_?????_01110 11")
    def bitpat_REMW   = BitPat("b0000001_?????_?????_110_?????_01110 11")
    def bitpat_REMUW  = BitPat("b0000001_?????_?????_111_?????_01110 11")

    // Priv
    def bitpat_ECALL  = BitPat("b0000000_00000_00000_000_00000_11100_11")
    def bitpat_EBREAK = BitPat("b0000000_00001_00000_000_00000_11100_11")
    def bitpat_MRET   = BitPat("b0011000_00010_00000_000_00000_11100_11")

    // RV64 Zicsr
    def bitpat_CSRRW  = BitPat("b???????_?????_?????_001_?????_11100_11")
    def bitpat_CSRRS  = BitPat("b???????_?????_?????_010_?????_11100_11")
    def bitpat_CSRRC  = BitPat("b???????_?????_?????_011_?????_11100_11")
    def bitpat_CSRRWI = BitPat("b???????_?????_?????_101_?????_11100_11")
    def bitpat_CSRRSI = BitPat("b???????_?????_?????_110_?????_11100_11")
    def bitpat_CSRRCI = BitPat("b???????_?????_?????_111_?????_11100_11")
}

object opcodes_EXU_Int{
    def Int_SHL =  0.U(4.W) // Integer Shift Left  (Both signed and unsigned)
    def Int_SHR =  1.U(4.W) // Integer Shift Right (Both signed and unsigned)
    def Int_ADD =  2.U(4.W) // Integer Add (ADD, ADDI, ADDW, ADDIW and Load-Store)
    def Int_SUB =  3.U(4.W) // Integer Subtraction (SUB)
    def Int_MUL =  4.U(4.W) // Integer Multiply (MUL, MULH, MULHU, MULHSU, MULW)
    def Int_DIV =  5.U(4.W) // Integer Division (DIV, DIVU, DIVUW, DIVW)
    def Int_REM =  6.U(4.W) // Integer Remainder (REM, REMU, REMW, REMUW)
    def Int_AND =  7.U(4.W) // Integer And (AND, ANDI)
    def Int_OR  =  8.U(4.W) // Integer Or (OR, ORI)
    def Int_XOR =  9.U(4.W) // Integer Xor (XOR, XORI)

    def Int_NOP = 10.U(4.W) // Do not perform any integer operations

    def Int_BEQ = 11.U(4.W)
    def Int_BNE = 12.U(4.W)
    def Int_BLT = 13.U(4.W)
    def Int_BGE = 14.U(4.W)
}

object opcodes_EXU_Int_sign{
    def Int_Signed_Signed     = 0.U(2.W)
    def Int_Unsigned_Unsigned = 1.U(2.W)
    def Int_Signed_Unsigned   = 2.U(2.W)
    def Int_Unsigned_Signed   = 3.U(2.W)
}

object opcodes_EXU_Int_computeLength{
    def Int_Word   = 0.U(2.W)
    def Int_Double = 1.U(2.W)
}

object opcodes_EXU_Int_resultPart{
    // This is ued for MULH, MULHSU, MULHU, these instructions will need to return the upper half of 2*XLEN result
    def Int_Low  = false.B
    def Int_High = true.B
}

object opcodes_EXU_Int_opreand{
    // distinguish rs1 & imm or rs1 & rs2
    def Int_TwoReg = 0.U(4.W)
    def Int_RegImm = 1.U(4.W)
    def Int_OneImm = 2.U(4.W)
    def Int_PCwReg = 3.U(4.W)
    def Int_PCwImm = 4.U(4.W)
    def Int_UseAll = 5.U(4.W)
}

object opcodes_LSU{
    def LSU_NOPE  = 0.U(4.W)
    def LSU_LOAD  = 1.U(4.W)
    def LSU_STORE = 2.U(4.W)
}

object opcodes_LSU_sign{
    def LSU_Signed   = true.B
    def LSU_Unsigned = false.B
}

object opcodes_LSU_len{
    def LSU_Byte   = 0.U(2.W)
    def LSU_Half   = 1.U(2.W)
    def LSU_Word   = 2.U(2.W)
    def LSU_Double = 3.U(2.W)
}

object opcodes_LSU_memOpreationType{
    def LSU_NOP = 0.U(2.W) // Normal not Load-Store Instructions
    def LSU_R   = 1.U(2.W) // Read-Only: Load Instructions
    def LSU_W   = 2.U(2.W) // Write-Only: Store Instructions
    def LSU_RW  = 3.U(2.W) // Atomic Instruction Extension
}

object opcodes_writeBackGPRType{
    def WB_GPR_NOP       = 0.U(4.W)
    def WB_GPR_EXU_Val   = 1.U(4.W)
    def WB_GPR_EXU_Bool  = 2.U(4.W)
    def WB_GPR_LSU       = 3.U(4.W)
    def WB_GPR_SNPC      = 4.U(4.W)
}

object opcodes_nextPCTypes{
    def PC_Next_Static  = true.B
    def PC_Next_Dynamic = false.B
}

object opcodes_PCJumpReason{
    def NoJumpPC   = 0.U(4.W)
    def BranchInst = 1.U(4.W)
    def JAL_Inst   = 2.U(4.W)
    def JALR_Inst  = 3.U(4.W)
    def ECALL_Inst = 4.U(4.W)
    def MRET_Inst  = 5.U(4.W)
}

object opcodes_IDU_privState{
    def NORMAL = 0.U(4.W)
    def EBREAK = 1.U(4.W)
    def ECALL  = 2.U(4.W)
    def MRET   = 3.U(4.W)

    def ERROR  = 15.U(4.W)
}