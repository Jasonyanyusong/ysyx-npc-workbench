/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing 101 High School
*
* YSYX-NPCB is licensed under Mulan PSL v2.
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

package npcb
import chisel3._
import chisel3.util._

object inst_types{
    def inst_R     = "b000".U
    def inst_I     = "b001".U
    def inst_S     = "b010".U
    def inst_B     = "b011".U
    def inst_U     = "b100".U
    def inst_J     = "b101".U
    def inst_N     = "b110".U
    def inst_error = "b111".U
}

object EXU_opcode{
    // EXU needs to do something for every instruction except EBREAK
    def EXU_DoNothing = "b000000".U
    def EXU_LUI       = "b000001".U
    def EXU_AUIPC     = "b000010".U
    def EXU_JAL       = "b000011".U
    def EXU_JALR      = "b000100".U
    def EXU_BEQ       = "b000101".U
    def EXU_BNE       = "b000110".U
    def EXU_BGE       = "b000111".U
    def EXU_BLTU      = "b001000".U
    def EXU_BGEU      = "b001001".U
    def EXU_LB        = "b001010".U
    def EXU_LH        = "b001011".U
    def EXU_LW        = "b001100".U
    def EXU_LBU       = "b001101".U
    def EXU_LHU       = "b001110".U
    def EXU_SB        = "b001111".U
    def EXU_SH        = "b010000".U
    def EXU_SW        = "b010001".U
    def EXU_ADDI      = "b010010".U
    def EXU_SLTI      = "b010011".U
    def EXU_SLTIU     = "b010100".U
    def EXU_XORI      = "b010101".U
    def EXU_ORI       = "b010110".U
    def EXU_ANDI      = "b010111".U
    def EXU_SLLI      = "b011000".U
    def EXU_SRLI      = "b011001".U
    def EXU_SRAI      = "b011010".U
    def EXU_ADD       = "b011011".U
    def EXU_SUB       = "b011100".U
    def EXU_SLL       = "b011101".U
    def EXU_SLT       = "b011110".U
    def EXU_SLTU      = "b011111".U
    def EXU_XOR       = "b100000".U
    def EXU_SRL       = "b100001".U
    def EXU_SRA       = "b100010".U
    def EXU_OR        = "b100011".U
    def EXU_AND       = "b100100".U
    def EXU_LWU       = "b100101".U
    def EXU_LD        = "b100110".U
    def EXU_SD        = "b100111".U
    def EXU_ADDIW     = "b101000".U
    def EXU_SLLIW     = "b101001".U
    def EXU_SRLIW     = "b101010".U
    def EXU_SRAIW     = "b101011".U
    def EXU_ADDW      = "b101100".U
    def EXU_SUBW      = "b101101".U
    def EXU_SLLW      = "b101110".U
    def EXU_SRLW      = "b101111".U
    def EXU_SRAW      = "b110000".U
    def EXU_MUL       = "b110001".U
    def EXU_MULH      = "b110010".U
    def EXU_MULHSU    = "b110011".U
    def EXU_MULHU     = "b110100".U
    def EXU_DIV       = "b110101".U
    def EXU_DIVU      = "b110110".U
    def EXU_REM       = "b110111".U
    def EXU_REMU      = "b111000".U
    def EXU_MULW      = "b111001".U
    def EXU_DIVW      = "b111010".U
    def EXU_DIVUW     = "b111011".U
    def EXU_REMW      = "b111100".U
    def EXU_REMUW     = "b111101".U
    def EXU_BLT       = "b111111".U
}

object LSU_opcode{
    def LSU_DoNothing = "b000000".U
    def LSU_LB        = "b000001".U
    def LSU_LBU       = "b000010".U
    def LSU_LH        = "b000011".U
    def LSU_LHU       = "b000100".U
    def LSU_LW        = "b000101".U
    def LSU_LWU       = "b001011".U
    def LSU_LD        = "b000110".U
    def LSU_SB        = "b000111".U
    def LSU_SH        = "b001000".U
    def LSU_SW        = "b001001".U
    def LSU_SD        = "b001010".U
}

object RV_Inst{
    // All Insts except EBREAK requrie EXU
    // RV64I Instructions
    def LUI       = BitPat("b???????_?????_?????_???_?????_01101_11") // U
    def AUIPC     = BitPat("b???????_?????_?????_???_?????_00101_11") // U
    def JAL       = BitPat("b???????_?????_?????_???_?????_11011_11") // J
    def JALR      = BitPat("b???????_?????_?????_000_?????_11001_11") // I
    def BEQ       = BitPat("b???????_?????_?????_000_?????_11000_11") // B
    def BNE       = BitPat("b???????_?????_?????_001_?????_11000_11") // B
    def BLT       = BitPat("b???????_?????_?????_100_?????_11000_11") // B
    def BGE       = BitPat("b???????_?????_?????_101_?????_11000_11") // B
    def BLTU      = BitPat("b???????_?????_?????_110_?????_11000_11") // B
    def BGEU      = BitPat("b???????_?????_?????_111_?????_11000_11") // B
    def LB        = BitPat("b???????_?????_?????_000_?????_00000_11") // I LSU
    def LH        = BitPat("b???????_?????_?????_001_?????_00000_11") // I LSU
    def LW        = BitPat("b???????_?????_?????_010_?????_00000_11") // I LSU
    def LBU       = BitPat("b???????_?????_?????_100_?????_00000_11") // I LSU
    def LHU       = BitPat("b???????_?????_?????_101_?????_00000_11") // I LSU
    def SB        = BitPat("b???????_?????_?????_000_?????_01000_11") // S LSU
    def SH        = BitPat("b???????_?????_?????_001_?????_01000_11") // S LSU
    def SW        = BitPat("b???????_?????_?????_010_?????_01000_11") // S LSU
    def ADDI      = BitPat("b???????_?????_?????_000_?????_00100_11") // I
    def SLTI      = BitPat("b???????_?????_?????_010_?????_00100_11") // I
    def SLTIU     = BitPat("b???????_?????_?????_011_?????_00100_11") // I
    def XORI      = BitPat("b???????_?????_?????_100_?????_00100_11") // I
    def ORI       = BitPat("b???????_?????_?????_110_?????_00100_11") // I
    def ANDI      = BitPat("b???????_?????_?????_111_?????_00100_11") // I
    def SLLI      = BitPat("b000000?_?????_?????_001_?????_00100_11") // I
    def SRLI      = BitPat("b000000?_?????_?????_101_?????_00100_11") // I
    def SRAI      = BitPat("b010000?_?????_?????_101_?????_00100_11") // I
    def ADD       = BitPat("b0000000_?????_?????_000_?????_01100_11") // R
    def SUB       = BitPat("b0100000_?????_?????_000_?????_01100_11") // R
    def SLL       = BitPat("b0000000_?????_?????_001_?????_01100_11") // R
    def SLT       = BitPat("b0000000_?????_?????_010_?????_01100_11") // R
    def SLTU      = BitPat("b0000000_?????_?????_011_?????_01100_11") // R
    def XOR       = BitPat("b0000000_?????_?????_100_?????_01100_11") // R
    def SRL       = BitPat("b0000000_?????_?????_101_?????_01100_11") // R
    def SRA       = BitPat("b0100000_?????_?????_101_?????_01100_11") // R
    def OR        = BitPat("b0000000_?????_?????_110_?????_01100_11") // R
    def AND       = BitPat("b0000000_?????_?????_111_?????_01100_11") // R
    def EBREAK    = BitPat("b0000000_00001_00000_000_00000_11100_11") // N
    def LWU       = BitPat("b???????_?????_?????_110_?????_00000_11") // I
    def LD        = BitPat("b???????_?????_?????_011_?????_00000_11") // I LSU
    def SD        = BitPat("b???????_?????_?????_011_?????_01000_11") // S LSU
    def ADDIW     = BitPat("b???????_?????_?????_000_?????_00110_11") // I
    def SLLIW     = BitPat("b0000000_?????_?????_001_?????_00110_11") // R
    def SRLIW     = BitPat("b0000000_?????_?????_101_?????_00110_11") // R
    def SRAIW     = BitPat("b0100000_?????_?????_101_?????_00110_11") // R
    def ADDW      = BitPat("b0000000_?????_?????_000_?????_01110_11") // R
    def SUBW      = BitPat("b0100000_?????_?????_000_?????_01110_11") // R
    def SLLW      = BitPat("b0000000_?????_?????_001_?????_01110_11") // R
    def SRLW      = BitPat("b0000000_?????_?????_101_?????_01110_11") // R
    def SRAW      = BitPat("b0100000_?????_?????_101_?????_01110_11") // R
    // RV64M Instructions
    def MUL       = BitPat("b0000001 ????? ????? 000 ????? 01100 11") // R
    def MULH      = BitPat("b0000001 ????? ????? 001 ????? 01100 11") // R
    def MULHSU    = BitPat("b0000001 ????? ????? 010 ????? 01100 11") // R
    def MULHU     = BitPat("b0000001 ????? ????? 011 ????? 01100 11") // R
    def DIV       = BitPat("b0000001 ????? ????? 100 ????? 01100 11") // R
    def DIVU      = BitPat("b0000001 ????? ????? 101 ????? 01100 11") // R
    def REM       = BitPat("b0000001 ????? ????? 110 ????? 01100 11") // R
    def REMU      = BitPat("b0000001 ????? ????? 111 ????? 01100 11") // R
    def MULW      = BitPat("b0000001 ????? ????? 000 ????? 01110 11") // R
    def DIVW      = BitPat("b0000001 ????? ????? 100 ????? 01110 11") // R
    def DIVUW     = BitPat("b0000001 ????? ????? 101 ????? 01110 11") // R
    def REMW      = BitPat("b0000001 ????? ????? 110 ????? 01110 11") // R
    def REMUW     = BitPat("b0000001 ????? ????? 111 ????? 01110 11") // R

}

class IFU extends Module{
    // Now we make IFU just a passthrough because we do not fetch instruction from memory directly
    val io = IO(new Bundle{
        val IFU_I_PC = Input(UInt(64.W))
        val IFU_O_PC = Output(UInt(64.W))
        val IFU_I_inst = Input(UInt(32.W))
        val IFU_O_inst = Output(UInt(32.W))
        val IFU_O_error = Output(Bool())
    })
    io.IFU_O_inst := io.IFU_I_inst
    io.IFU_O_PC := io.IFU_I_PC
    io.IFU_O_error := false.B
}

class IDU extends Module{
    val io = IO(new Bundle{
        val IDU_I_inst = Input(UInt(32.W))
        val IDU_O_ModifyMem = Output(Bool())
        val IDU_O_rs1 = Output(UInt(5.W)) // Same for all
        val IDU_I_src1 = Input(UInt(64.W))
        val IDU_O_src1 = Input(UInt(64.W)) // Same for all
        val IDU_O_rs2 = Output(UInt(5.W)) // Same for all
        val IDU_I_src2 = Input(UInt(64.W))
        val IDU_O_src2 = Input(UInt(64.W)) // Same for all
        val IDU_O_rd = Output(UInt(5.W)) // Same for all
        val IDU_O_EXUopcode = Output(UInt(6.W))
        val IDU_O_LSUopcode = Output(UInt(6.W))
        val IDU_O_snpcISdnpc = Output(Bool())
        val IDU_O_GPRneedWriteBack = Output(Bool())
        val IDU_O_imm = Output(UInt(64.W))
        val IDU_O_halt = Output(Bool())
        val IDU_O_error = Output(Bool())
    })

    io.IDU_O_rs1 := io.IDU_I_inst(19, 15) // Cut rs1 from BITS(i, 19, 15)
    io.IDU_O_rs2 := io.IDU_I_inst(24, 20) // Cut rs2 from BITS(i, 24, 20)
    io.IDU_O_rd := io.IDU_I_inst(11, 7) // Cut rd from BITS(i, 11, 7)
    io.IDU_O_src1 := io.IDU_I_src1
    io.IDU_O_src2 := io.IDU_I_src2

    io.IDU_O_halt := false.B
    io.IDU_O_error := false.B

    val immI = io.IDU_I_inst(31, 20)
    val SignExtend_immI = Cat(Fill(52, immI(11)), immI)
    val immS = Cat(io.IDU_I_inst(31, 25), io.IDU_I_inst(11, 7))
    val SignExtend_immS = Cat(Fill(52, immS(11)), immS)
    val immB = Cat(io.IDU_I_inst(31, 31), io.IDU_I_inst(7, 7), io.IDU_I_inst(30, 25), io.IDU_I_inst(11, 8), 0.U)
    val SignExtend_immB = Cat(Fill(51, immB(12)), immB)
    val immU = Cat(io.IDU_I_inst(31, 12), Fill(12, 0.U))
    val SignExtend_immU = Cat(Fill(32, immU(31)), immU)
    val immJ = Cat(io.IDU_I_inst(31, 31), io.IDU_I_inst(19, 12), io.IDU_I_inst(20, 20), io.IDU_I_inst(30, 21), 0.U)
    val SignExtend_immJ = Cat(Fill(43, immJ(20)), immJ)
    val immR = 0.U
    val SignExtend_immR = Cat(Fill(63, immR(0)), immJ) // When we found error in decoding, we will automatically return this imm value since it is 0, this will reduce the cause of bugs

    // We use a List called IDU_opcodes to record operats, the list have structure:
    // List(inst_type(15, 13), ModifyMem(12, 12), EXUopcode(11, 6), LSUopcode(9, 4), snpcISdnpc(3, 3), GPRneedWriteBack(2, 2), error(1, 1), halt(0, 0))
    // Here snpcISdnpc does not contain B type instructions even though they may be equal

    var IDU_opcodes = ListLookup(
        /*Compare Item: */io.IDU_I_inst,
        /*Default: */       List(inst_types.inst_error, EXU_opcode.EXU_DoNothing, LSU_opcode.LSU_DoNothing), Array(
        RV_Inst.LUI      -> List(inst_types.inst_U    , EXU_opcode.EXU_LUI      , LSU_opcode.LSU_DoNothing),
        RV_Inst.AUIPC    -> List(inst_types.inst_U    , EXU_opcode.EXU_AUIPC    , LSU_opcode.LSU_DoNothing),
        RV_Inst.JAL      -> List(inst_types.inst_J    , EXU_opcode.EXU_JAL      , LSU_opcode.LSU_DoNothing),
        RV_Inst.JALR     -> List(inst_types.inst_I    , EXU_opcode.EXU_JALR     , LSU_opcode.LSU_DoNothing),
        RV_Inst.BEQ      -> List(inst_types.inst_B    , EXU_opcode.EXU_BEQ      , LSU_opcode.LSU_DoNothing),
        RV_Inst.BNE      -> List(inst_types.inst_B    , EXU_opcode.EXU_BNE      , LSU_opcode.LSU_DoNothing),
        RV_Inst.BLT      -> List(inst_types.inst_B    , EXU_opcode.EXU_BLT      , LSU_opcode.LSU_DoNothing),
        RV_Inst.BGE      -> List(inst_types.inst_B    , EXU_opcode.EXU_BGE      , LSU_opcode.LSU_DoNothing),
        RV_Inst.BLTU     -> List(inst_types.inst_B    , EXU_opcode.EXU_BLTU     , LSU_opcode.LSU_DoNothing),
        RV_Inst.BGEU     -> List(inst_types.inst_B    , EXU_opcode.EXU_BGEU     , LSU_opcode.LSU_DoNothing),
        RV_Inst.LB       -> List(inst_types.inst_I    , EXU_opcode.EXU_LB       , LSU_opcode.LSU_LB       ),
        RV_Inst.LH       -> List(inst_types.inst_I    , EXU_opcode.EXU_LH       , LSU_opcode.LSU_LH       ),
        RV_Inst.LW       -> List(inst_types.inst_I    , EXU_opcode.EXU_LW       , LSU_opcode.LSU_LW       ),
        RV_Inst.LBU      -> List(inst_types.inst_I    , EXU_opcode.EXU_LBU      , LSU_opcode.LSU_LBU      ),
        RV_Inst.LHU      -> List(inst_types.inst_I    , EXU_opcode.EXU_LHU      , LSU_opcode.LSU_LHU      ),
        RV_Inst.SB       -> List(inst_types.inst_S    , EXU_opcode.EXU_SB       , LSU_opcode.LSU_SB       ),
        RV_Inst.SH       -> List(inst_types.inst_S    , EXU_opcode.EXU_SH       , LSU_opcode.LSU_SH       ),
        RV_Inst.SW       -> List(inst_types.inst_S    , EXU_opcode.EXU_SW       , LSU_opcode.LSU_SW       ),
        RV_Inst.ADDI     -> List(inst_types.inst_I    , EXU_opcode.EXU_ADDI     , LSU_opcode.LSU_DoNothing),
        RV_Inst.SLTI     -> List(inst_types.inst_I    , EXU_opcode.EXU_SLTI     , LSU_opcode.LSU_DoNothing),
        RV_Inst.SLTIU    -> List(inst_types.inst_I    , EXU_opcode.EXU_SLTIU    , LSU_opcode.LSU_DoNothing),
        RV_Inst.XORI     -> List(inst_types.inst_I    , EXU_opcode.EXU_XORI     , LSU_opcode.LSU_DoNothing),
        RV_Inst.ORI      -> List(inst_types.inst_I    , EXU_opcode.EXU_ORI      , LSU_opcode.LSU_DoNothing),
        RV_Inst.ANDI     -> List(inst_types.inst_I    , EXU_opcode.EXU_ANDI     , LSU_opcode.LSU_DoNothing),
        RV_Inst.SLLI     -> List(inst_types.inst_I    , EXU_opcode.EXU_SLLI     , LSU_opcode.LSU_DoNothing),
        RV_Inst.SRLI     -> List(inst_types.inst_I    , EXU_opcode.EXU_SRLI     , LSU_opcode.LSU_DoNothing),
        RV_Inst.SRAI     -> List(inst_types.inst_I    , EXU_opcode.EXU_SRAI     , LSU_opcode.LSU_DoNothing),
        RV_Inst.ADD      -> List(inst_types.inst_R    , EXU_opcode.EXU_ADD      , LSU_opcode.LSU_DoNothing),
        RV_Inst.SUB      -> List(inst_types.inst_R    , EXU_opcode.EXU_SUB      , LSU_opcode.LSU_DoNothing),
        RV_Inst.SLL      -> List(inst_types.inst_R    , EXU_opcode.EXU_SLL      , LSU_opcode.LSU_DoNothing),
        RV_Inst.SLT      -> List(inst_types.inst_R    , EXU_opcode.EXU_SLT      , LSU_opcode.LSU_DoNothing),
        RV_Inst.SLTU     -> List(inst_types.inst_R    , EXU_opcode.EXU_SLTU     , LSU_opcode.LSU_DoNothing),
        RV_Inst.XOR      -> List(inst_types.inst_R    , EXU_opcode.EXU_XOR      , LSU_opcode.LSU_DoNothing),
        RV_Inst.SRL      -> List(inst_types.inst_R    , EXU_opcode.EXU_SRL      , LSU_opcode.LSU_DoNothing),
        RV_Inst.SRA      -> List(inst_types.inst_R    , EXU_opcode.EXU_SRA      , LSU_opcode.LSU_DoNothing),
        RV_Inst.OR       -> List(inst_types.inst_R    , EXU_opcode.EXU_OR       , LSU_opcode.LSU_DoNothing),
        RV_Inst.AND      -> List(inst_types.inst_R    , EXU_opcode.EXU_AND      , LSU_opcode.LSU_DoNothing),
        RV_Inst.EBREAK   -> List(inst_types.inst_N    , EXU_opcode.EXU_DoNothing, LSU_opcode.LSU_DoNothing), // We set snpc === dnpc for EBREAK
        RV_Inst.LWU      -> List(inst_types.inst_I    , EXU_opcode.EXU_LWU      , LSU_opcode.LSU_LWU      ),
        RV_Inst.LD       -> List(inst_types.inst_I    , EXU_opcode.EXU_LD       , LSU_opcode.LSU_LD       ),
        RV_Inst.SD       -> List(inst_types.inst_S    , EXU_opcode.EXU_SD       , LSU_opcode.LSU_SD       ),
        RV_Inst.ADDIW    -> List(inst_types.inst_I    , EXU_opcode.EXU_ADDIW    , LSU_opcode.LSU_DoNothing),
        RV_Inst.SLLIW    -> List(inst_types.inst_R    , EXU_opcode.EXU_SLLIW    , LSU_opcode.LSU_DoNothing),
        RV_Inst.SRLIW    -> List(inst_types.inst_R    , EXU_opcode.EXU_SRLIW    , LSU_opcode.LSU_DoNothing),
        RV_Inst.SRAIW    -> List(inst_types.inst_R    , EXU_opcode.EXU_SRAIW    , LSU_opcode.LSU_DoNothing),
        RV_Inst.ADDW     -> List(inst_types.inst_R    , EXU_opcode.EXU_ADDW     , LSU_opcode.LSU_DoNothing),
        RV_Inst.ADDIW    -> List(inst_types.inst_R    , EXU_opcode.EXU_ADDIW    , LSU_opcode.LSU_DoNothing),
        RV_Inst.SUBW     -> List(inst_types.inst_R    , EXU_opcode.EXU_SUBW     , LSU_opcode.LSU_DoNothing),
        RV_Inst.SLLW     -> List(inst_types.inst_R    , EXU_opcode.EXU_SLLW     , LSU_opcode.LSU_DoNothing),
        RV_Inst.SRAW     -> List(inst_types.inst_R    , EXU_opcode.EXU_SRAW     , LSU_opcode.LSU_DoNothing),
        RV_Inst.MUL      -> List(inst_types.inst_R    , EXU_opcode.EXU_MUL      , LSU_opcode.LSU_DoNothing),
        RV_Inst.MULH     -> List(inst_types.inst_R    , EXU_opcode.EXU_MULH     , LSU_opcode.LSU_DoNothing),
        RV_Inst.MULHSU   -> List(inst_types.inst_R    , EXU_opcode.EXU_MULHSU   , LSU_opcode.LSU_DoNothing),
        RV_Inst.MULHU    -> List(inst_types.inst_R    , EXU_opcode.EXU_MULHU    , LSU_opcode.LSU_DoNothing),
        RV_Inst.DIV      -> List(inst_types.inst_R    , EXU_opcode.EXU_DIV      , LSU_opcode.LSU_DoNothing),
        RV_Inst.DIVU     -> List(inst_types.inst_R    , EXU_opcode.EXU_DIVU     , LSU_opcode.LSU_DoNothing),
        RV_Inst.REM      -> List(inst_types.inst_R    , EXU_opcode.EXU_REM      , LSU_opcode.LSU_DoNothing),
        RV_Inst.REMU     -> List(inst_types.inst_R    , EXU_opcode.EXU_REMU     , LSU_opcode.LSU_DoNothing),
        RV_Inst.MULW     -> List(inst_types.inst_R    , EXU_opcode.EXU_MULW     , LSU_opcode.LSU_DoNothing),
        RV_Inst.DIVW     -> List(inst_types.inst_R    , EXU_opcode.EXU_DIVW     , LSU_opcode.LSU_DoNothing),
        RV_Inst.DIVUW    -> List(inst_types.inst_R    , EXU_opcode.EXU_DIVUW    , LSU_opcode.LSU_DoNothing),
        RV_Inst.REMW     -> List(inst_types.inst_R    , EXU_opcode.EXU_REMW     , LSU_opcode.LSU_DoNothing),
        RV_Inst.REMUW    -> List(inst_types.inst_R    , EXU_opcode.EXU_REMUW    , LSU_opcode.LSU_DoNothing))
        )

    val IDU_inst_type          = IDU_opcodes(0)
    //io.IDU_O_ModifyMem        := IDU_opcodes(1)
    io.IDU_O_EXUopcode        := IDU_opcodes(1)
    io.IDU_O_LSUopcode        := IDU_opcodes(2)
    //io.IDU_O_snpcISdnpc       := IDU_opcodes(4)
    //io.IDU_O_GPRneedWriteBack := IDU_opcodes(5)
    //io.IDU_O_error            := IDU_opcodes(6)
    //io.IDU_O_halt             := IDU_opcodes(7)

    var IDU_switchs = ListLookup(
        /*Compare Item: */io.IDU_I_inst,
        /*Default: */       List(0.U, 1.U, 0.U, 1.U, 1.U), Array(
        RV_Inst.LUI      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.AUIPC    -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.JAL      -> List(0.U, 0.U, 1.U, 0.U, 0.U),
        RV_Inst.JALR     -> List(0.U, 0.U, 1.U, 0.U, 0.U),
        RV_Inst.BEQ      -> List(0.U, 0.U, 0.U, 0.U, 0.U),
        RV_Inst.BNE      -> List(0.U, 0.U, 0.U, 0.U, 0.U),
        RV_Inst.BLT      -> List(0.U, 0.U, 0.U, 0.U, 0.U),
        RV_Inst.BGE      -> List(0.U, 0.U, 0.U, 0.U, 0.U),
        RV_Inst.BLTU     -> List(0.U, 0.U, 0.U, 0.U, 0.U),
        RV_Inst.BGEU     -> List(0.U, 0.U, 0.U, 0.U, 0.U),
        RV_Inst.LB       -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.LH       -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.LW       -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.LBU      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.LHU      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SB       -> List(1.U, 1.U, 0.U, 0.U, 0.U),
        RV_Inst.SH       -> List(1.U, 1.U, 0.U, 0.U, 0.U),
        RV_Inst.SW       -> List(1.U, 1.U, 0.U, 0.U, 0.U),
        RV_Inst.ADDI     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLTI     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLTIU    -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.XORI     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.ORI      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.ANDI     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLLI     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SRLI     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SRAI     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.ADD      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SUB      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLL      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLT      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLTU     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.XOR      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SRL      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SRA      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.OR       -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.AND      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.EBREAK   -> List(0.U, 1.U, 0.U, 0.U, 1.U), // We set snpc === dnpc for EBREAK
        RV_Inst.LWU      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.LD       -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SD       -> List(1.U, 1.U, 0.U, 0.U, 0.U),
        RV_Inst.ADDIW    -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLLIW    -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SRLIW    -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SRAIW    -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.ADDW     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.ADDIW    -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SUBW     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLLW     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SRAW     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.MUL      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.MULH     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.MULHSU   -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.MULHU    -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.DIV      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.DIVU     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.REM      -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.REMU     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.MULW     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.DIVW     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.DIVUW    -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.REMW     -> List(0.U, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.REMUW    -> List(0.U, 1.U, 1.U, 0.U, 0.U))
        )

    ///val IDU_inst_type          = IDU_opcodes(0)
    io.IDU_O_ModifyMem        := IDU_switchs(0)
    ///io.IDU_O_EXUopcode        := IDU_opcodes(2)
    ///io.IDU_O_LSUopcode        := IDU_opcodes(3)
    io.IDU_O_snpcISdnpc       := IDU_switchs(1)
    io.IDU_O_GPRneedWriteBack := IDU_switchs(2)
    io.IDU_O_error            := IDU_switchs(3)
    io.IDU_O_halt             := IDU_switchs(4)

    /*val IDU_imm = ListLookup(IDU_inst_type, 
        List(SignExtend_immR),
        Array(
            inst_types.inst_error -> List(SignExtend_immR),
            inst_types.inst_N     -> List(SignExtend_immR),
            inst_types.inst_R     -> List(SignExtend_immR),
            inst_types.inst_I     -> List(SignExtend_immI),
            inst_types.inst_S     -> List(SignExtend_immS),
            inst_types.inst_B     -> List(SignExtend_immB),
            inst_types.inst_U     -> List(SignExtend_immU),
            inst_types.inst_J     -> List(SignExtend_immJ),
        ))*/

    io.IDU_O_imm := MuxCase(SignExtend_immR,
    Array(
        (IDU_inst_type === inst_types.inst_error) -> SignExtend_immR,
        (IDU_inst_type === inst_types.inst_N)     -> SignExtend_immR,
        (IDU_inst_type === inst_types.inst_R)     -> SignExtend_immR,
        (IDU_inst_type === inst_types.inst_I)     -> SignExtend_immI,
        (IDU_inst_type === inst_types.inst_S)     -> SignExtend_immS,
        (IDU_inst_type === inst_types.inst_B)     -> SignExtend_immB,
        (IDU_inst_type === inst_types.inst_U)     -> SignExtend_immU,
        (IDU_inst_type === inst_types.inst_J)     -> SignExtend_immJ,
    ))
}

class EXU extends Module{
    val io = IO(new Bundle{
        val EXU_I_src1 = Input(UInt(64.W))
        val EXU_I_src2 = Input(UInt(64.W))
        val EXU_I_imm = Input(UInt(64.W))
        val EXU_I_opcode = Input(UInt(6.W))
        val EXU_I_currentPC = Input(UInt(64.W))
        val EXU_O_result = Output(UInt(64.W))
        val EXU_O_snpcNEQdnpc = Output(Bool())
        val EXU_O_error = Output(Bool())
    })
    io.EXU_O_error := false.B
    val EXU_src1_signed = io.EXU_I_src1.asSInt
    val EXU_src1_unsigned = io.EXU_I_src1.asUInt
    val EXU_src2_signed = io.EXU_I_src2.asSInt
    val EXU_src2_unsigned = io.EXU_I_src2.asUInt
    val EXU_imm_signed = io.EXU_I_imm.asSInt
    val EXU_imm_unsigned = io.EXU_I_imm.asUInt

    // We use a list to manage the output of EXU
    // List(result(65, 2), snpcNEQdnpc(1), error(0))

    /*var EXU_output = ListLookup(
        /*Compare Item: */ io.EXU_I_opcode,
        // If none of the opcodes are matched, we set result to 0, with static next pc and raise error
        /*Default: */                   List(0.U(64.W)                                                                                                                 , 0.U                                                  , 1.U), Array(
        EXU_opcode.EXU_DoNothing     -> List(0.U(64.W)                                                                                                                 , 0.U                                                  , 0.U),
        EXU_opcode.EXU_LUI           -> List(EXU_imm_unsigned                                                                                                          , 0.U                                                  , 0.U),
        EXU_opcode.EXU_AUIPC         -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , 0.U                                                  , 0.U),
        EXU_opcode.EXU_JAL           -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , 1.U                                                  , 0.U),
        EXU_opcode.EXU_JALR          -> List((EXU_src1_unsigned + EXU_src2_unsigned) & ~1.U                                                                            , 1.U                                                  , 0.U),
        EXU_opcode.EXU_BEQ           -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , (!(EXU_src1_unsigned === EXU_src2_unsigned)).asUInt  , 0.U),
        EXU_opcode.EXU_BNE           -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , (!(EXU_src1_unsigned =/= EXU_src2_unsigned)).asUInt  , 0.U),
        EXU_opcode.EXU_BLT           -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , (!(EXU_src1_signed < EXU_src2_signed)).asUInt        , 0.U),
        EXU_opcode.EXU_BGE           -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , (!(EXU_src1_signed >= EXU_src2_signed)).asUInt       , 0.U),
        EXU_opcode.EXU_BLTU          -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , (!(EXU_src1_unsigned < EXU_src2_unsigned)).asUInt    , 0.U),
        EXU_opcode.EXU_BGEU          -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , (!(EXU_src1_unsigned >= EXU_src2_unsigned)).asUInt   , 0.U),
        EXU_opcode.EXU_LB            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_LH            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_LW            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_LBU           -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_LHU           -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SB            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SH            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SW            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_ADDI          -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SLTI          -> List(Mux(EXU_src1_signed < EXU_imm_signed, 1.U(64.W), 0.U(64.W))                                                               , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SLTIU         -> List(Mux(EXU_src1_unsigned < EXU_imm_unsigned, 1.U(64.W), 0.U(64.W))                                                           , 0.U                                                  , 0.U),
        EXU_opcode.EXU_XORI          -> List(EXU_src1_unsigned ^ EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_ORI           -> List(EXU_src1_unsigned | EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_ANDI          -> List(EXU_src1_unsigned & EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SLLI          -> List(EXU_src1_unsigned << EXU_imm_unsigned(5, 0)                                                                               , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SRLI          -> List(EXU_src1_unsigned >> EXU_imm_unsigned(5, 0)                                                                               , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SRAI          -> List(EXU_src1_signed >> EXU_imm_unsigned(5, 0)                                                                                 , 0.U                                                  , 0.U),
        EXU_opcode.EXU_ADD           -> List(EXU_src1_unsigned + EXU_src2_unsigned                                                                                     , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SUB           -> List(EXU_src1_unsigned - EXU_src2_unsigned                                                                                     , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SLL           -> List(EXU_src1_unsigned << EXU_src2_unsigned(5, 0)                                                                              , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SLT           -> List(Mux(EXU_src1_signed < EXU_src2_signed, 1.U(64.W), 0.U(64.W))                                                              , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SLTU          -> List(Mux(EXU_src1_unsigned < EXU_src2_unsigned, 1.U(64.W), 0.U(64.W))                                                          , 0.U                                                  , 0.U),
        EXU_opcode.EXU_XOR           -> List(EXU_src1_unsigned ^ EXU_src2_unsigned                                                                                     , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SRL           -> List(EXU_src1_unsigned >> EXU_src2_unsigned(5, 0)                                                                              , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SRA           -> List(EXU_src1_signed >> EXU_src2_signed(5, 0)                                                                                  , 0.U                                                  , 0.U),
        EXU_opcode.EXU_OR            -> List(EXU_src1_unsigned | EXU_src2_unsigned                                                                                     , 0.U                                                  , 0.U),
        EXU_opcode.EXU_AND           -> List(EXU_src1_unsigned & EXU_src2_unsigned                                                                                     , 0.U                                                  , 0.U),
        EXU_opcode.EXU_LWU           -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_LD            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SD            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.EXU_ADDIW         -> List(Cat(Fill(32, (EXU_src1_unsigned + EXU_imm_unsigned)(31)), (EXU_src1_unsigned + EXU_imm_unsigned)(31, 0))                  , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SLLIW         -> List(Cat(Fill(32, (EXU_src1_unsigned << EXU_imm_unsigned(4, 0))(31)), (EXU_src1_unsigned << EXU_imm_unsigned(4, 0))(31, 0))    , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SRLIW         -> List(Cat(Fill(32, (EXU_src1_unsigned >> EXU_imm_unsigned(4, 0))(31)), (EXU_src1_unsigned >> EXU_imm_unsigned(4, 0))(31, 0))    , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SRAIW         -> List(Cat(Fill(32, (EXU_src1_signed >> EXU_imm_unsigned(4, 0))(31)), (EXU_src1_signed >> EXU_imm_signed(4, 0))(31, 0))          , 0.U                                                  , 0.U),
        EXU_opcode.EXU_ADDW          -> List(Cat(Fill(32, (EXU_src1_unsigned + EXU_src2_unsigned)(31)), (EXU_src1_unsigned + EXU_src2_unsigned)(31, 0))                , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SUBW          -> List(Cat(Fill(32, (EXU_src1_unsigned - EXU_src2_unsigned)(31)), (EXU_src1_unsigned - EXU_src2_unsigned)(31, 0))                , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SLLW          -> List(Cat(Fill(32, (EXU_src1_unsigned << EXU_src2_unsigned(4, 0))(31)), (EXU_src1_unsigned << EXU_imm_unsigned(4, 0))(31, 0))   , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SRLW          -> List(Cat(Fill(32, (EXU_src1_unsigned >> EXU_src2_unsigned(4, 0))(31)), (EXU_src1_unsigned >> EXU_imm_unsigned(4, 0))(31, 0))   , 0.U                                                  , 0.U),
        EXU_opcode.EXU_SRAW          -> List(Cat(Fill(32, (EXU_src1_signed >> EXU_src2_unsigned(4, 0))(31)), (EXU_src1_signed >> EXU_imm_signed(4, 0))(31, 0))         , 0.U                                                  , 0.U),
        EXU_opcode.EXU_MUL           -> List((EXU_src1_unsigned * EXU_src2_unsigned)(63, 0)                                                                            , 0.U                                                  , 0.U),
        EXU_opcode.EXU_MULH          -> List((EXU_src1_signed * EXU_src2_signed)(127, 64)                                                                              , 0.U                                                  , 0.U),
        EXU_opcode.EXU_MULHSU        -> List((EXU_src1_signed * EXU_src2_unsigned)(127, 64)                                                                            , 0.U                                                  , 0.U),
        EXU_opcode.EXU_MULHU         -> List((EXU_src1_unsigned * EXU_src2_unsigned)(127, 64)                                                                          , 0.U                                                  , 0.U),
        EXU_opcode.EXU_DIV           -> List((EXU_src1_signed / EXU_src2_signed)                                                                                       , 0.U                                                  , 0.U),
        EXU_opcode.EXU_DIVU          -> List((EXU_src1_unsigned / EXU_src2_unsigned)                                                                                   , 0.U                                                  , 0.U),
        EXU_opcode.EXU_REM           -> List((EXU_src1_signed % EXU_src2_signed)                                                                                       , 0.U                                                  , 0.U),
        EXU_opcode.EXU_REMU          -> List((EXU_src1_unsigned % EXU_src2_unsigned)                                                                                   , 0.U                                                  , 0.U),
        EXU_opcode.EXU_MULW          -> List(Cat(Fill(32, (EXU_src1_unsigned * EXU_src2_unsigned)(31)), (EXU_src1_unsigned * EXU_src2_unsigned)(31, 0))                , 0.U                                                  , 0.U),
        EXU_opcode.EXU_DIVW          -> List(Cat(Fill(32, (EXU_src1_signed / EXU_src2_signed)(31)), (EXU_src1_signed / EXU_src2_signed)(31, 0))                        , 0.U                                                  , 0.U),
        EXU_opcode.EXU_DIVUW         -> List(Cat(Fill(32, (EXU_src1_unsigned / EXU_src2_unsigned)(31)), (EXU_src1_unsigned / EXU_src2_unsigned)(31, 0))                , 0.U                                                  , 0.U),
        EXU_opcode.EXU_REMW          -> List(Cat(Fill(32, (EXU_src1_signed % EXU_src2_signed)(31)), (EXU_src1_signed % EXU_src2_signed)(31, 0))                        , 0.U                                                  , 0.U),
        EXU_opcode.EXU_REMUW         -> List(Cat(Fill(32, (EXU_src1_unsigned % EXU_src2_unsigned)(31)), (EXU_src1_unsigned % EXU_src2_unsigned)(31, 0))                , 0.U                                                  , 0.U))
    )*/

    //io.EXU_O_result := EXU_output(0)
    //io.EXU_O_snpcNEQdnpc := EXU_output(1)
    //io.EXU_O_error := EXU_output(2)

    io.EXU_O_result := MuxCase(0.U(64.W),
    Array(
        (io.EXU_I_opcode === EXU_opcode.EXU_DoNothing) -> (0.U(64.W)),
        (io.EXU_I_opcode === EXU_opcode.EXU_LUI)       -> (EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_AUIPC)     -> (io.EXU_I_currentPC + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_JAL)       -> (io.EXU_I_currentPC + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_JALR)      -> ((EXU_src1_unsigned + EXU_src2_unsigned) & ~1.U),
        (io.EXU_I_opcode === EXU_opcode.EXU_BEQ)       -> (io.EXU_I_currentPC + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_BNE)       -> (io.EXU_I_currentPC + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_BLT)       -> (io.EXU_I_currentPC + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_BGE)       -> (io.EXU_I_currentPC + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_BLTU)      -> (io.EXU_I_currentPC + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_BGEU)      -> (io.EXU_I_currentPC + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_LB)        -> (EXU_src1_unsigned + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_LH)        -> (EXU_src1_unsigned + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_LW)        -> (EXU_src1_unsigned + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_LBU)       -> (EXU_src1_unsigned + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_LHU)       -> (EXU_src1_unsigned + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_SB)        -> (EXU_src1_unsigned + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_SH)        -> (EXU_src1_unsigned + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_SW)        -> (EXU_src1_unsigned + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_ADDI)      -> (EXU_src1_unsigned + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_SLTI)      -> (Mux(EXU_src1_signed < EXU_imm_signed, 1.U(64.W), 0.U(64.W))),
        (io.EXU_I_opcode === EXU_opcode.EXU_SLTIU)     -> (Mux(EXU_src1_unsigned < EXU_imm_unsigned, 1.U(64.W), 0.U(64.W))),
        (io.EXU_I_opcode === EXU_opcode.EXU_XORI)      -> (EXU_src1_unsigned ^ EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_ORI)       -> (EXU_src1_unsigned | EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_ANDI)      -> (EXU_src1_unsigned & EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_SLLI)      -> (EXU_src1_unsigned << EXU_imm_unsigned(5, 0)),
        (io.EXU_I_opcode === EXU_opcode.EXU_SRLI)      -> (EXU_src1_unsigned >> EXU_imm_unsigned(5, 0)),
        (io.EXU_I_opcode === EXU_opcode.EXU_SRAI)      -> (EXU_src1_unsigned >> EXU_imm_unsigned(5, 0)),
        (io.EXU_I_opcode === EXU_opcode.EXU_ADD)       -> (EXU_src1_unsigned + EXU_src2_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_SUB)       -> (EXU_src1_unsigned - EXU_src2_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_SLL)       -> (EXU_src1_unsigned << EXU_src2_unsigned(5, 0)),
        (io.EXU_I_opcode === EXU_opcode.EXU_SLT)       -> (Mux(EXU_src1_signed < EXU_src2_signed, 1.U(64.W), 0.U(64.W))),
        (io.EXU_I_opcode === EXU_opcode.EXU_SLTU)      -> (Mux(EXU_src1_unsigned < EXU_src2_unsigned, 1.U(64.W), 0.U(64.W))),
        (io.EXU_I_opcode === EXU_opcode.EXU_XOR)       -> (EXU_src1_unsigned ^ EXU_src2_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_SRL)       -> (EXU_src1_unsigned >> EXU_src2_unsigned(5, 0)),
        (io.EXU_I_opcode === EXU_opcode.EXU_SRA)       -> (EXU_src1_signed >> EXU_src2_signed(5, 0)),
        (io.EXU_I_opcode === EXU_opcode.EXU_OR)        -> (EXU_src1_unsigned | EXU_src2_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_AND)       -> (EXU_src1_unsigned & EXU_src2_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_LWU)       -> (EXU_src1_unsigned + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_LD)        -> (EXU_src1_unsigned + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_SD)        -> (EXU_src1_unsigned + EXU_imm_unsigned),
        (io.EXU_I_opcode === EXU_opcode.EXU_ADDIW)     -> (Cat(Fill(32, (EXU_src1_unsigned + EXU_imm_unsigned)(31)), (EXU_src1_unsigned + EXU_imm_unsigned)(31, 0))),
        (io.EXU_I_opcode === EXU_opcode.EXU_SLLIW)     -> (Cat(Fill(32, (EXU_src1_unsigned << EXU_imm_unsigned(4, 0))(31)), (EXU_src1_unsigned << EXU_imm_unsigned(4, 0))(31, 0))),
        (io.EXU_I_opcode === EXU_opcode.EXU_SRLIW)     -> (Cat(Fill(32, (EXU_src1_unsigned >> EXU_imm_unsigned(4, 0))(31)), (EXU_src1_unsigned >> EXU_imm_unsigned(4, 0))(31, 0))),
        (io.EXU_I_opcode === EXU_opcode.EXU_SRAIW)     -> (Cat(Fill(32, (EXU_src1_signed >> EXU_imm_unsigned(4, 0))(31)), (EXU_src1_signed >> EXU_imm_signed(4, 0))(31, 0))),
        (io.EXU_I_opcode === EXU_opcode.EXU_ADDW)      -> (Cat(Fill(32, (EXU_src1_unsigned + EXU_src2_unsigned)(31)), (EXU_src1_unsigned + EXU_src2_unsigned)(31, 0))),
        (io.EXU_I_opcode === EXU_opcode.EXU_SUBW)      -> (Cat(Fill(32, (EXU_src1_unsigned - EXU_src2_unsigned)(31)), (EXU_src1_unsigned - EXU_src2_unsigned)(31, 0))),
        (io.EXU_I_opcode === EXU_opcode.EXU_SLLW)      -> (Cat(Fill(32, (EXU_src1_unsigned << EXU_src2_unsigned(4, 0))(31)), (EXU_src1_unsigned << EXU_imm_unsigned(4, 0))(31, 0))),
        (io.EXU_I_opcode === EXU_opcode.EXU_SRLW)      -> (Cat(Fill(32, (EXU_src1_unsigned >> EXU_src2_unsigned(4, 0))(31)), (EXU_src1_unsigned >> EXU_imm_unsigned(4, 0))(31, 0))),
        (io.EXU_I_opcode === EXU_opcode.EXU_SRAW)      -> (Cat(Fill(32, (EXU_src1_signed >> EXU_src2_unsigned(4, 0))(31)), (EXU_src1_signed >> EXU_imm_signed(4, 0))(31, 0))),
        (io.EXU_I_opcode === EXU_opcode.EXU_MUL)       -> ((EXU_src1_unsigned * EXU_src2_unsigned)(63, 0)),
        (io.EXU_I_opcode === EXU_opcode.EXU_MULH)      -> ((EXU_src1_signed * EXU_src2_signed)(127, 64)),
        (io.EXU_I_opcode === EXU_opcode.EXU_MULHSU)    -> ((EXU_src1_signed * EXU_src2_unsigned)(127, 64)),
        (io.EXU_I_opcode === EXU_opcode.EXU_MULHU)     -> ((EXU_src1_unsigned * EXU_src2_unsigned)(127, 64)),
        (io.EXU_I_opcode === EXU_opcode.EXU_DIV)       -> ((EXU_src1_signed / EXU_src2_signed)),
        (io.EXU_I_opcode === EXU_opcode.EXU_DIVU)      -> ((EXU_src1_unsigned / EXU_src2_unsigned)),
        (io.EXU_I_opcode === EXU_opcode.EXU_REM)       -> ((EXU_src1_signed % EXU_src2_signed)),
        (io.EXU_I_opcode === EXU_opcode.EXU_REMU)      -> ((EXU_src1_unsigned % EXU_src2_unsigned)),
        (io.EXU_I_opcode === EXU_opcode.EXU_MULW)      -> (Cat(Fill(32, (EXU_src1_unsigned * EXU_src2_unsigned)(31)), (EXU_src1_unsigned * EXU_src2_unsigned)(31, 0))),
        (io.EXU_I_opcode === EXU_opcode.EXU_DIVW)      -> (Cat(Fill(32, (EXU_src1_signed / EXU_src2_signed)(31)), (EXU_src1_signed / EXU_src2_signed)(31, 0))),
        (io.EXU_I_opcode === EXU_opcode.EXU_DIVUW)     -> (Cat(Fill(32, (EXU_src1_unsigned / EXU_src2_unsigned)(31)), (EXU_src1_unsigned / EXU_src2_unsigned)(31, 0))),
        (io.EXU_I_opcode === EXU_opcode.EXU_REMW)      -> (Cat(Fill(32, (EXU_src1_signed % EXU_src2_signed)(31)), (EXU_src1_signed % EXU_src2_signed)(31, 0))),
        (io.EXU_I_opcode === EXU_opcode.EXU_REMUW)     -> (Cat(Fill(32, (EXU_src1_unsigned % EXU_src2_unsigned)(31)), (EXU_src1_unsigned % EXU_src2_unsigned)(31, 0))),
    ))

    io.EXU_O_snpcNEQdnpc := MuxCase(false.B,
    Array(
        (io.EXU_I_opcode === EXU_opcode.EXU_JAL)       -> (true.B),
        (io.EXU_I_opcode === EXU_opcode.EXU_JALR)      -> (true.B),
        (io.EXU_I_opcode === EXU_opcode.EXU_BEQ)       -> (!(EXU_src1_unsigned === EXU_src2_unsigned)),
        (io.EXU_I_opcode === EXU_opcode.EXU_BNE)       -> (!(EXU_src1_unsigned =/= EXU_src2_unsigned)),
        (io.EXU_I_opcode === EXU_opcode.EXU_BLT)       -> (!(EXU_src1_signed < EXU_src2_signed)),
        (io.EXU_I_opcode === EXU_opcode.EXU_BGE)       -> (!(EXU_src1_signed >= EXU_src2_signed)),
        (io.EXU_I_opcode === EXU_opcode.EXU_BLTU)      -> (!(EXU_src1_unsigned < EXU_src2_unsigned)),
        (io.EXU_I_opcode === EXU_opcode.EXU_BGEU)      -> (!(EXU_src1_unsigned >= EXU_src2_unsigned))
    ))

    io.EXU_O_error := MuxCase(true.B,
    Array(
        (io.EXU_I_opcode === EXU_opcode.EXU_DoNothing) -> (false.B)
    ))
}

class PCU extends Module{
    val io = IO(new Bundle{
        val PCU_I_CurrentPC = Input(UInt(64.W))
        val PCU_I_imm = Input(UInt(64.W))
        val PCU_I_willJump = Input(Bool())
        val PCU_O_DynamicNextPC = Output(UInt(64.W))
        val PCU_O_StaticNextPC = Output(UInt(64.W))
        val PCU_O_error = Output(Bool())
    })
    io.PCU_O_StaticNextPC := io.PCU_I_CurrentPC + 4.U
    io.PCU_O_DynamicNextPC := Mux(io.PCU_I_willJump, io.PCU_I_CurrentPC + io.PCU_I_imm, io.PCU_I_CurrentPC + 4.U)
    io.PCU_O_error := false.B
}

class LSU extends Module{
    val io = IO(new Bundle{
        val LSU_I_src1 = Input(UInt(64.W))
        val LSU_I_src2 = Input(UInt(64.W))
        //LSU_I_EXUresult = Input(UInt(64.W))
        val LSU_I_opcode = Input(UInt(6.W))
        val LSU_I_ModifyMem = Input(Bool())
        val LSU_O_result = Output(UInt(64.W))
        // Following signals are pulled to top
        val LSU_O_memAddr = Output(UInt(64.W))
        val LSU_O_memRW = Output(Bool()) // Low: Read, High: Write
        val LSU_I_memR = Input(UInt(64.W))
        val LSU_O_memW = Output(UInt(64.W))
        val LSU_O_error = Output(Bool())
        val LSU_O_len = Output(UInt(2.W)) // 00: Bit 01: Half 10: Word 11: Double Word
    })
    io.LSU_O_error := false.B

    // We use a list to manage the output of LSU
    // List(result, memAddr, memRW, memW, error, len)

    /*var LSU_output = ListLookup(
        /*Compare Item: */ io.LSU_I_opcode,
        /*Default: */               List(0.U(64.W)                                        , 0.U(64.W)     , 0.U, 0.U(64.W)     , 1.U , "b00".U), Array(
        LSU_opcode.LSU_DoNothing -> List(0.U(64.W)                                        , 0.U(64.W)     , 0.U, 0.U(64.W)     , 0.U , "b00".U),
        LSU_opcode.LSU_LB        -> List(Cat(Fill(56, io.LSU_I_memR(7)) , io.LSU_I_memR)  , io.LSU_I_src1 , 0.U, 0.U(64.W)     , 0.U , "b00".U),
        LSU_opcode.LSU_LBU       -> List(Cat(Fill(56, 0.U) , io.LSU_I_memR)               , io.LSU_I_src1 , 0.U, 0.U(64.W)     , 0.U , "b00".U),
        LSU_opcode.LSU_LH        -> List(Cat(Fill(48, io.LSU_I_memR(15)) , io.LSU_I_memR) , io.LSU_I_src1 , 0.U, 0.U(64.W)     , 0.U , "b01".U),
        LSU_opcode.LSU_LHU       -> List(Cat(Fill(48, 0.U) , io.LSU_I_memR)               , io.LSU_I_src1 , 0.U, 0.U(64.W)     , 0.U , "b01".U),
        LSU_opcode.LSU_LW        -> List(Cat(Fill(32, io.LSU_I_memR(31)) , io.LSU_I_memR) , io.LSU_I_src1 , 0.U, 0.U(64.W)     , 0.U , "b10".U),
        LSU_opcode.LSU_LWU       -> List(Cat(Fill(32, 0.U) , io.LSU_I_memR)               , io.LSU_I_src1 , 0.U, 0.U(64.W)     , 0.U , "b10".U),
        LSU_opcode.LSU_LD        -> List(Cat(Fill(32, io.LSU_I_memR(31)) , io.LSU_I_memR) , io.LSU_I_src1 , 0.U, 0.U(64.W)     , 0.U , "b11".U),
        LSU_opcode.LSU_SB        -> List(0.U(64.W)                                        , io.LSU_I_src1 , 1.U, io.LSU_I_src2 , 0.U , "b00".U),
        LSU_opcode.LSU_SH        -> List(0.U(64.W)                                        , io.LSU_I_src1 , 1.U, io.LSU_I_src2 , 0.U , "b01".U),
        LSU_opcode.LSU_SW        -> List(0.U(64.W)                                        , io.LSU_I_src1 , 1.U, io.LSU_I_src2 , 0.U , "b10".U),
        LSU_opcode.LSU_SD        -> List(0.U(64.W)                                        , io.LSU_I_src1 , 1.U, io.LSU_I_src2 , 0.U , "b11".U))
    )*/

    io.LSU_O_result := MuxCase(0.U(64.W),
    Array(
        (io.LSU_I_opcode === LSU_opcode.LSU_DoNothing) -> 0.U(64.W),
        (io.LSU_I_opcode === LSU_opcode.LSU_LB)        -> Cat(Fill(56, io.LSU_I_memR(7)) , io.LSU_I_memR),
        (io.LSU_I_opcode === LSU_opcode.LSU_LBU)       -> Cat(Fill(56, 0.U) , io.LSU_I_memR),
        (io.LSU_I_opcode === LSU_opcode.LSU_LH)        -> Cat(Fill(48, io.LSU_I_memR(15)) , io.LSU_I_memR),
        (io.LSU_I_opcode === LSU_opcode.LSU_LHU)       -> Cat(Fill(48, 0.U) , io.LSU_I_memR),
        (io.LSU_I_opcode === LSU_opcode.LSU_LW)        -> Cat(Fill(32, io.LSU_I_memR(31)) , io.LSU_I_memR),
        (io.LSU_I_opcode === LSU_opcode.LSU_LWU)       -> Cat(Fill(32, 0.U) , io.LSU_I_memR),
        (io.LSU_I_opcode === LSU_opcode.LSU_LD)        -> io.LSU_I_memR,
        (io.LSU_I_opcode === LSU_opcode.LSU_SB)        -> 0.U(64.W),
        (io.LSU_I_opcode === LSU_opcode.LSU_SH)        -> 0.U(64.W),
        (io.LSU_I_opcode === LSU_opcode.LSU_SW)        -> 0.U(64.W),
        (io.LSU_I_opcode === LSU_opcode.LSU_SD)        -> 0.U(64.W)
    ))

    io.LSU_O_memAddr := MuxCase(0.U(64.W),
    Array(
        (io.LSU_I_opcode === LSU_opcode.LSU_DoNothing) -> 0.U(64.W),
        (io.LSU_I_opcode === LSU_opcode.LSU_LB)        -> io.LSU_I_src1,
        (io.LSU_I_opcode === LSU_opcode.LSU_LBU)       -> io.LSU_I_src1,
        (io.LSU_I_opcode === LSU_opcode.LSU_LH)        -> io.LSU_I_src1,
        (io.LSU_I_opcode === LSU_opcode.LSU_LHU)       -> io.LSU_I_src1,
        (io.LSU_I_opcode === LSU_opcode.LSU_LW)        -> io.LSU_I_src1,
        (io.LSU_I_opcode === LSU_opcode.LSU_LWU)       -> io.LSU_I_src1,
        (io.LSU_I_opcode === LSU_opcode.LSU_LD)        -> io.LSU_I_src1,
        (io.LSU_I_opcode === LSU_opcode.LSU_SB)        -> io.LSU_I_src1,
        (io.LSU_I_opcode === LSU_opcode.LSU_SH)        -> io.LSU_I_src1,
        (io.LSU_I_opcode === LSU_opcode.LSU_SW)        -> io.LSU_I_src1,
        (io.LSU_I_opcode === LSU_opcode.LSU_SD)        -> io.LSU_I_src1
    ))

    io.LSU_O_memRW := MuxCase(false.B,
    Array(
        (io.LSU_I_opcode === LSU_opcode.LSU_DoNothing) -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_LB)        -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_LBU)       -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_LH)        -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_LHU)       -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_LW)        -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_LWU)       -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_LD)        -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_SB)        -> true.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_SH)        -> true.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_SW)        -> true.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_SD)        -> true.B
    ))

    io.LSU_O_memW := MuxCase(0.U(64.W),
    Array(
        (io.LSU_I_opcode === LSU_opcode.LSU_DoNothing) -> 0.U(64.W),
        (io.LSU_I_opcode === LSU_opcode.LSU_LB)        -> 0.U(64.W),
        (io.LSU_I_opcode === LSU_opcode.LSU_LBU)       -> 0.U(64.W),
        (io.LSU_I_opcode === LSU_opcode.LSU_LH)        -> 0.U(64.W),
        (io.LSU_I_opcode === LSU_opcode.LSU_LHU)       -> 0.U(64.W),
        (io.LSU_I_opcode === LSU_opcode.LSU_LW)        -> 0.U(64.W),
        (io.LSU_I_opcode === LSU_opcode.LSU_LWU)       -> 0.U(64.W),
        (io.LSU_I_opcode === LSU_opcode.LSU_LD)        -> 0.U(64.W),
        (io.LSU_I_opcode === LSU_opcode.LSU_SB)        -> io.LSU_I_src2,
        (io.LSU_I_opcode === LSU_opcode.LSU_SH)        -> io.LSU_I_src2,
        (io.LSU_I_opcode === LSU_opcode.LSU_SW)        -> io.LSU_I_src2,
        (io.LSU_I_opcode === LSU_opcode.LSU_SD)        -> io.LSU_I_src2
    ))

    io.LSU_O_error := MuxCase(true.B,
    Array(
        (io.LSU_I_opcode === LSU_opcode.LSU_DoNothing) -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_LB)        -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_LBU)       -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_LH )       -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_LHU)       -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_LW)        -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_LWU)       -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_LD)        -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_SB)        -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_SH)        -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_SW)        -> false.B,
        (io.LSU_I_opcode === LSU_opcode.LSU_SD)        -> false.B
    ))

    io.LSU_O_len := MuxCase("b00".U,
    Array(
        (io.LSU_I_opcode === LSU_opcode.LSU_DoNothing) -> "b00".U,
        (io.LSU_I_opcode === LSU_opcode.LSU_LB)        -> "b00".U,
        (io.LSU_I_opcode === LSU_opcode.LSU_LBU)       -> "b00".U,
        (io.LSU_I_opcode === LSU_opcode.LSU_LH)        -> "b01".U,
        (io.LSU_I_opcode === LSU_opcode.LSU_LHU)       -> "b01".U,
        (io.LSU_I_opcode === LSU_opcode.LSU_LW)        -> "b10".U,
        (io.LSU_I_opcode === LSU_opcode.LSU_LWU)       -> "b10".U,
        (io.LSU_I_opcode === LSU_opcode.LSU_LD)        -> "b11".U,
        (io.LSU_I_opcode === LSU_opcode.LSU_SB)        -> "b00".U,
        (io.LSU_I_opcode === LSU_opcode.LSU_SH)        -> "b01".U,
        (io.LSU_I_opcode === LSU_opcode.LSU_SW)        -> "b10".U,
        (io.LSU_I_opcode === LSU_opcode.LSU_SD)        -> "b11".U
    ))

    //io.LSU_O_result := LSU_output(0)
    //io.LSU_O_memAddr := LSU_output(1)
    //io.LSU_O_memRW := LSU_output(2)
    //io.LSU_O_memW := LSU_output(3)
    //io.LSU_O_error := LSU_output(4)
    //io.LSU_O_len := LSU_output(5)
}

class WBU extends Module{
    val io = IO(new Bundle{
        val WBU_I_EXUresult = Input(UInt(64.W))
        val WBU_I_LSUresult = Input(UInt(64.W))
        val WBU_I_LSUenable = Input(Bool())
        val WBU_I_rd = Input(UInt(5.W))
        val WBU_O_GPRidx = Output(UInt(5.W))
        val WBU_O_GPRWriteBack = Output(UInt(64.W))
        val WBU_O_error = Output(Bool())
    })
    io.WBU_O_GPRidx := io.WBU_I_rd
    io.WBU_O_GPRWriteBack := Mux(io.WBU_I_LSUenable, io.WBU_I_LSUresult, io.WBU_I_EXUresult)
    // Mux(io.WBU_I_IDUsnpcISdnpc, io.WBU_O_nextPC := io.WBU_I_currentPC + 4.U, Mux(io.WBU_I_EXUsnpcNEQdnpc, io.WBU_O_nextPC := io.WBU_I_nextPC))
    io.WBU_O_error := false.B
}

class NPCB extends Module{
    val io = IO(new Bundle{
        val NPC_sendCurrentPC = Output(UInt(64.W))
        val NPC_getInst = Input(UInt(32.W))
        val NPC_LSU_O_memAddr = Output(UInt(64.W))
        val NPC_LSU_O_memRW = Output(Bool()) // Low: Read, High: Write
        val NPC_LSU_I_memR = Input(UInt(64.W))
        val NPC_LSU_O_memW = Output(UInt(64.W))
        val NPC_LSU_O_lem = Output(UInt(2.W))

        val NPC_GPRchanged = Output(Bool())
        val NPC_halt = Output(Bool())
        val NPC_error = Output(Bool())
    })

    val PC = RegInit(0.U(64.W))
    val GPR = Mem(32, UInt(64.W))
    def GPR_read (index: UInt) = Mux(index === 0.U, 0.U(64.W), GPR(index)) // So that we make sure R(0) is always 0

    // Step I: Fetch one piece of instruction from Mem
    val npcb_IFU = Module(new IFU)
    npcb_IFU.io.IFU_I_inst := io.NPC_getInst
    npcb_IFU.io.IFU_I_PC := PC

    // Step II: Decode the instruction: 1) generate OPCODE for LSU and EXU 2) cut RS1, RS2 and RD 3) get SRC1 and SRC2 from GPR 4) Decide if this instruction may cause PC jump
    val npcb_IDU = Module(new IDU)
    npcb_IDU.io.IDU_I_inst := npcb_IFU.io.IFU_I_inst
    npcb_IDU.io.IDU_I_src1 := GPR_read(npcb_IDU.io.IDU_O_rs1)
    npcb_IDU.io.IDU_I_src2 := GPR_read(npcb_IDU.io.IDU_O_rs2)
    io.NPC_halt := npcb_IDU.io.IDU_O_halt

    // Step III: ALU execution
    val npcb_EXU = Module(new EXU)
    npcb_EXU.io.EXU_I_src1 := npcb_IDU.io.IDU_O_src1
    npcb_EXU.io.EXU_I_src2 := npcb_IDU.io.IDU_O_src2
    npcb_EXU.io.EXU_I_imm := npcb_IDU.io.IDU_O_imm
    npcb_EXU.io.EXU_I_opcode := npcb_IDU.io.IDU_O_EXUopcode
    npcb_EXU.io.EXU_I_currentPC := PC

    // Step IV: LSU execution (we access memory using Verilator, NEMU and AM's debug interfaces)
    val npcb_LSU = Module(new LSU)
    npcb_LSU.io.LSU_I_src1 := npcb_EXU.io.EXU_O_result // LSU will get src1 + imm from EXU's result
    npcb_LSU.io.LSU_I_src2 := npcb_IDU.io.IDU_O_src2
    //npcb_LSU.io.LSU_I_EXUresult := npcb_EXU.io.EXU_O_result
    npcb_LSU.io.LSU_I_ModifyMem := npcb_IDU.io.IDU_O_ModifyMem
    npcb_LSU.io.LSU_I_opcode := npcb_IDU.io.IDU_O_LSUopcode

    // Step V: Write back data to a GPR and PC
    val npcb_WBU = Module(new WBU)
    val npcb_PCU = Module(new PCU)
    npcb_WBU.io.WBU_I_EXUresult := npcb_EXU.io.EXU_O_result
    npcb_WBU.io.WBU_I_LSUresult := npcb_LSU.io.LSU_O_result
    npcb_PCU.io.PCU_I_CurrentPC := PC
    npcb_PCU.io.PCU_I_willJump := (! npcb_IDU.io.IDU_O_snpcISdnpc) && npcb_EXU.io.EXU_O_snpcNEQdnpc
    npcb_PCU.io.PCU_I_imm := npcb_IDU.io.IDU_O_imm
    npcb_WBU.io.WBU_I_rd := npcb_IDU.io.IDU_O_rd
    io.NPC_GPRchanged := npcb_IDU.io.IDU_O_GPRneedWriteBack
    GPR(npcb_WBU.io.WBU_O_GPRidx) := Mux(npcb_IDU.io.IDU_O_GPRneedWriteBack, npcb_WBU.io.WBU_O_GPRWriteBack, GPR_read(npcb_WBU.io.WBU_O_GPRidx))
    PC := npcb_PCU.io.PCU_O_DynamicNextPC
}