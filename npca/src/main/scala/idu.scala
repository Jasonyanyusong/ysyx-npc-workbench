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
import chisel3-util._

class IDU extends Module{
    val io = IO(new Bundle{
        val IDU_I_inst = Input(UInt(32.W))

        val IDU_O_rs1 = Output(UInt(5.W))
        val IDU_O_rs2 = Output(UInt(5.W))
        val IDU_O_rd  = Output(UInt(5.W))

        val IDU_O_InstIndicator = Output(UInt(10.W)) // debug only, do not participate in logic
        val IDU_O_InstType = Output(UInt(4.W)) // debug only, do not participate in logic

        val IDU_O_EXU_Int_opcode = Output(UInt(4.W))
        val IDU_O_EXU_Int_sign = Output(UInt(2.W))
        val IDU_O_EXU_Int_computeLength = Output(UInt(2.W))
        val IDU_O_EXU_Int_resultPart = Output(Bool())
        val IDU_O_EXU_Int_operand = Output(UInt(4.W))

        val IDU_O_LSU_optype = Output(UInt(4.W))
        val IDU_O_LSU_sign = Output(Bool())
        val IDU_O_LSU_len = Output(UInt(2.W))
        val IDU_O_LSU_memOperationType = Output(UInt(2.W))

        val IDU_O_writeBackRegType = Output(UInt(4.W))
        val IDU_O_privState = Output(UInt(4.W))
        val IDU_O_nextPCStatic = Output(Bool())
        val IDU_O_PCJumpReadson = Output(UInt(4.W))

        val IDU_O_imm = Output(UInt(64.W))
    })

    io.IDU_O_rs1 := io.IDU_I_inst(19, 15)
    io.IDU_O_rs2 := io.IDU_I_inst(24, 20)
    io.IDU_O_rd  := io.IDU_I_inst(11,  7)

    io.IDU_O_isHalt := opcode_IDU_isHalt.IDU_NOTHALT

    // all types of immediates are bit-manipulated, concated
    val immI = io.IDU_I_inst(31, 20)
    val immS = Cat(io.IDU_I_inst(31, 25), io.IDU_I_inst(11, 7))
    val immB = Cat(io.IDU_I_inst(31, 31), io.IDU_I_inst(7, 7), io.IDU_I_inst(30, 25), io.IDU_I_inst(11, 8), 0.U)
    val immU = Cat(io.IDU_I_inst(31, 12), Fill(12, 0.U))
    val immJ = Cat(io.IDU_I_inst(31, 31), io.IDU_I_inst(19, 12), io.IDU_I_inst(20, 20), io.IDU_I_inst(30, 21), 0.U)

    // sign-extend these immediates for later calculation
    val SignExtend_immI = Cat(Fill(52, immI(11)), immI)
    val SignExtend_immS = Cat(Fill(52, immS(11)), immS)
    val SignExtend_immB = Cat(Fill(51, immB(12)), immB)
    val SignExtend_immU = Cat(Fill(32, immU(31)), immU)
    val SignExtend_immJ = Cat(Fill(43, immJ(20)), immJ)
    val NoImmediateNum  = Fill(64, 0.U) // if an instruction's type is not I, S, B, U, J

    // now we are ready to perform the decode work
    // we will use some scala variables called "IDU_opcodes" to store decode results by calling the ListLookup API
    // these results will be treated as the final output of IDU

    // variables "IDU_opcodes" have several parts, and will be sepreated for each execution Unit:
    //  1) Operation code for EXecution Unit, bind output with "IDU_O_EXU_Int_opcode"
    //  2) Sign/Unsign computation switch for EXecution Unit, bind output with "IDU_O_EXU_Int_sign"
    //  3) Computation Length for EXecution Unit, now support Word and Double-Word, bind output with "IDU_O_EXU_Int_computeLength"
    //  4) Output cut part for EXecution Unit, used because special case for MULH, MULHU, MULHSU, bind output with "IDU_O_EXU_Int_resultPart"
    //  5) Operand Information for EXecution Unit, used to select operand #2 from GPR[rs2] or immediate value, bind output with "IDU_O_EXU_Int_operand"

    //  6) Operation code for Load-Store Unit, bind output with "IDU_O_LSU_optype"
    //  7) Sign/Unsign computation switch for Load-Store Unit, bind output with "IDU_O_LSU_sign"
    //  8) Length of memory manipulation, bind output with "IDU_O_LSU_len"
    //  9) Memory operation type for Load-Store Unit, bind output with "IDU_O_LSU_memOperationType"

    // 10) Switch for register write-back type, for later WBU use, this will help to protect register from unexpected writing, bind output with "IDU_O_writeBackRegType"
    // 11) Priv Arch indicator, can tell simulation environment to stop simulating, bind output with "IDU_O_privState"
    // 12) Instruction Indicator, debug-only signal, every instruction have unique number, this will be disabled if not debugging, bind output with "IDU_O_InstIndicator"
    // 13) Instruction Type Indicator, debug-only signal, every instruction type have unique number, this will be diabled if not debugging, bind output with "IDU_O_InstType"
    // 14) Immediate Value, bind output with "IDU_O_imm"
    // 15) next PC type, static (+4) or may be dynamic, bind output with "IDU_O_nextPCStatic"
    // 16) reason of PC jump (if not jump then select NoJumpPC), so Write-Back Unit can process PC Mask if needed, bind output with "IDU_O_PCJumpReadson"
    var IDU_opcodes_EXU_Int = ListLookup(
        /*Compare Item: */           io.IDU_I_inst,
        /*Default Vals: */           List(opcodes_EXU_Int.Int_NOP, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg), Array(
        rv64_bitpat.bitpat_LUI    -> List(opcodes_EXU_Int.Int_NOP, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_OneImm),
        rv64_bitpat.bitpat_AUIPC  -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_PCwImm),
        rv64_bitpat.bitpat_JAL    -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_PCwImm),
        rv64_bitpat.bitpat_JALR   -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_BEQ    -> List(opcodes_EXU_Int.Int_BEQ, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_UseAll),
        rv64_bitpat.bitpat_BNE    -> List(opcodes_EXU_Int.Int_BNE, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_UseAll),
        rv64_bitpat.bitpat_BLT    -> List(opcodes_EXU_Int.Int_BLT, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_UseAll),
        rv64_bitpat.bitpat_BGE    -> List(opcodes_EXU_Int.Int_BGE, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_UseAll),
        rv64_bitpat.bitpat_BLTU   -> List(opcodes_EXU_Int.Int_BLT, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_UseAll),
        rv64_bitpat.bitpat_BGEU   -> List(opcodes_EXU_Int.Int_BGE, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_UseAll),
        rv64_bitpat.bitpat_LB     -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_LH     -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_LW     -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_LBU    -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_LHU    -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SB     -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SH     -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SW     -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_ADDI   -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SLTI   -> List(opcodes_EXU_Int.Int_BLT, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SLTIU  -> List(opcodes_EXU_Int.Int_BLT, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_XORI   -> List(opcodes_EXU_Int.Int_XOR, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_ORI    -> List(opcodes_EXU_Int.Int_OR,  opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_ANDI   -> List(opcodes_EXU_Int.Int_AND, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SLLI   -> List(opcodes_EXU_Int.Int_SHL, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SRLI   -> List(opcodes_EXU_Int.Int_SHR, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SRAI   -> List(opcodes_EXU_Int.Int_SRA, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_ADD    -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SUB    -> List(opcodes_EXU_Int.Int_SUB, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SLL    -> List(opcodes_EXU_Int.Int_SHL, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SLT    -> List(opcodes_EXU_Int.Int_BLT, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SLTU   -> List(opcodes_EXU_Int.Int_BLT, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_XOR    -> List(opcodes_EXU_Int.Int_XOR, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SRL    -> List(opcodes_EXU_Int.Int_SHR, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SRA    -> List(opcodes_EXU_Int.Int_SRA, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_OR     -> List(opcodes_EXU_Int.Int_OR,  opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_AND    -> List(opcodes_EXU_Int.Int_AND, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_LUW    -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_LD     -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SD     -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_ADDIW  -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SLLIW  -> List(opcodes_EXU_Int.Int_SHL, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SRLIW  -> List(opcodes_EXU_Int.Int_SHR, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SRAIW  -> List(opcodes_EXU_Int.Int_SRA, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_ADDW   -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SUBW   -> List(opcodes_EXU_Int.Int_SUB, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SLLW   -> List(opcodes_EXU_Int.Int_SHL, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SRLW   -> List(opcodes_EXU_Int.Int_SHR, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SRAW   -> List(opcodes_EXU_Int.Int_SRA, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_MUL    -> List(opcodes_EXU_Int.Int_MUL, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_MULH   -> List(opcodes_EXU_Int.Int_MUL, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_High, opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_MULHSU -> List(opcodes_EXU_Int.Int_MUL, opcodes_EXU_Int_sign.Int_Signed_Unsigned,   opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_High, opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_MULHU  -> List(opcodes_EXU_Int.Int_MUL, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_High, opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_DIV    -> List(opcodes_EXU_Int.Int_DIV, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_DIVU   -> List(opcodes_EXU_Int.Int_DIV, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_REM    -> List(opcodes_EXU_Int.Int_REM, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_REMU   -> List(opcodes_EXU_Int.Int_REM, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_MUL    -> List(opcodes_EXU_Int.Int_MUL, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_DIVW   -> List(opcodes_EXU_Int.Int_DIV, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_DIVUW  -> List(opcodes_EXU_Int.Int_DIV, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_REMW   -> List(opcodes_EXU_Int.Int_REM, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_REMUW  -> List(opcodes_EXU_Int.Int_REM, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg)
        )
    )

    var IDU_opcodes_LSU = ListLookup(
        /*Compare Item: */           io.IDU_I_inst,
        /*Default Vals: */           List(opcodes_LSU.LSU_NOPE, opcodes_LSU_sign.LSU_Unsigned, opcodes_LSU_len.LSU_Byte, opcodes_LSU_memOpreationType.LSU_NOP), Array(
        // TODO: Add more Instructions here
        )
    )

    var IDU_opcodes_MACRO = ListLookup(
        /*Compare Item: */           io.IDU_I_inst,
        /*Default Vals: */           List(opcodes_writeBackGPRType.WB_GPR_NOP, opcodes_IDU_privState.ERROR, rv64_opcodes.opcode_DoNothing, inst_types.inst_E, NoImmediateNum, opcodes_nextPCTypes.PC_Next_Static, opcodes_PCJumpReason.NoJumpPC), Array(
        // TODO: Add more Instructions here
        )
    )

    // Decoding finished internally, write these datas to their sperate ports
    io.IDU_O_EXU_Int_opcode        := IDU_opcodes_EXU_Int(0)
    io.IDU_O_EXU_Int_sign          := IDU_opcodes_EXU_Int(1)
    io.IDU_O_EXU_Int_computeLength := IDU_opcodes_EXU_Int(2)
    io.IDU_O_EXU_Int_resultPart    := IDU_opcodes_EXU_Int(3)
    io.IDU_O_EXU_Int_operand       := IDU_opcodes_EXU_Int(4)

    io.IDU_O_LSU_optype            := IDU_opcodes_LSU(0)
    io.IDU_O_LSU_sign              := IDU_opcodes_LSU(1)
    io.IDU_O_LSU_len               := IDU_opcodes_LSU(2)
    io.IDU_O_LSU_memOperationType  := IDU_opcodes_LSU(3)

    io.IDU_O_writeBackRegType      := IDU_opcodes_MACRO(0)
    io.IDU_O_privState             := IDU_opcodes_MACRO(1)
    io.IDU_O_InstIndicator         := IDU_opcodes_MACRO(2)
    io.IDU_O_InstType              := IDU_opcodes_MACRO(3)
    io.IDU_O_imm                   := IDU_opcodes_MACRO(4)
    io.IDU_O_nextPCStatic          := IDU_opcodes_MACRO(5)
    io.IDU_O_PCJumpReadson         := IDU_opcodes_MACRO(6)
}