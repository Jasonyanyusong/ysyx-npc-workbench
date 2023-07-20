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

package npca_future
import chisel3._
import chisel3.util._

class IDUN extends Module{
    val io = IO(new Bundle{
        val IDUN_I_inst                  = Input(UInt(32.W))
        
        val IDUN_O_rs1                   = Output(UInt(5.W))
        val IDUN_O_rs2                   = Output(UInt(5.W))
        val IDUN_O_rd                    = Output(UInt(5.W))

        val IDUN_O_EXU_Int_opcode        = Output(UInt(4.W))
        val IDUN_O_EXU_Int_sign          = Output(UInt(2.W))
        val IDUN_O_EXU_Int_computeLength = Output(UInt(2.W))
        val IDUN_O_EXU_Int_resultPart    = Output(Bool())
        val IDUN_O_EXU_Int_operand       = Output(UInt(4.W))

        val IDUN_O_LSU_optype            = Output(UInt(4.W))
        val IDUN_O_LSU_sign              = Output(Bool())
        val IDUN_O_LSU_len               = Output(UInt(2.W))
        val IDUN_O_LSU_memOperationType  = Output(UInt(2.W))

        val IDUN_O_AMO_memOps            = Output(UInt(4.W))
        val IDUN_O_AMO_opLen             = Output(UInt(2.W))
        val IDUN_O_AMO_aqrl              = Output(UInt(2.W))

        val IDUN_O_writeBackRegType      = Output(UInt(4.W))
        val IDUN_O_privState             = Output(UInt(4.W))
        val IDUN_O_nextPCStatic          = Output(Bool())
        val IDUN_O_PCJumpReason          = Output(UInt(4.W))

        val IDUN_O_csrOperation          = Output(UInt(4.W))
        val IDUN_O_privOperation         = Output(UInt(4.W))
        val IDUN_O_csrAddr               = Output(UInt(12.W)) // 0-4095 CSR index

        val IDUN_O_devodeSuccess         = Output(Bool()) // if success, use this decoder's val, else use IDUC's val

        val IDUN_O_imm                   = Output(UInt(64.W))
    })

    io.IDUN_O_rs1 := io.IDUN_I_inst(19, 15)
    io.IDUN_O_rs2 := io.IDUN_I_inst(24, 20)
    io.IDUN_O_rd  := io.IDUN_I_inst(11,  7)

    // all types of immediates are bit-manipulated, concated
    val immI = io.IDUN_I_inst(31, 20)
    val immS = Cat(io.IDUN_I_inst(31, 25), io.IDUN_I_inst(11, 7))
    val immB = Cat(io.IDUN_I_inst(31, 31), io.IDUN_I_inst(7, 7), io.IDUN_I_inst(30, 25), io.IDUN_I_inst(11, 8), 0.U)
    val immU = Cat(io.IDUN_I_inst(31, 12), Fill(12, 0.U))
    val immJ = Cat(io.IDUN_I_inst(31, 31), io.IDUN_I_inst(19, 12), io.IDUN_I_inst(20, 20), io.IDUN_I_inst(30, 21), 0.U)

    // sign-extend these immediates for later calculation
    val SignExtend_immI = Cat(Fill(52, immI(11)), immI)
    val SignExtend_immS = Cat(Fill(52, immS(11)), immS)
    val SignExtend_immB = Cat(Fill(51, immB(12)), immB)
    val SignExtend_immU = Cat(Fill(32, immU(31)), immU)
    val SignExtend_immJ = Cat(Fill(43, immJ(20)), immJ)
    val NoImmediateNum  = Fill(64, 0.U) // if an instruction's type is not I, S, B, U, J

    // now we are ready to perform the decode work
    // we will use some scala variables called "IDUN_opcodes" to store decode results by calling the ListLookup API
    // these results will be treated as the final output of IDU

    // variables "IDUN_opcodes" have several parts, and will be sepreated for each execution Unit:
    //  1) Operation code for EXecution Unit, bind output with "IDUN_O_EXU_Int_opcode"
    //  2) Sign/Unsign computation switch for EXecution Unit, bind output with "IDUN_O_EXU_Int_sign"
    //  3) Computation Length for EXecution Unit, now support Word and Double-Word, bind output with "IDUN_O_EXU_Int_computeLength"
    //  4) Output cut part for EXecution Unit, used because special case for MULH, MULHU, MULHSU, bind output with "IDUN_O_EXU_Int_resultPart"
    //  5) Operand Information for EXecution Unit, used to select operand #2 from GPR[rs2] or immediate value, bind output with "IDUN_O_EXU_Int_operand"

    //  6) Operation code for Load-Store Unit, bind output with "IDUN_O_LSU_optype"
    //  7) Sign/Unsign computation switch for Load-Store Unit, bind output with "IDUN_O_LSU_sign"
    //  8) Length of memory manipulation, bind output with "IDUN_O_LSU_len"
    //  9) Memory operation type for Load-Store Unit, bind output with "IDUN_O_LSU_memOperationType"

    // 10) Switch for register write-back type, for later WBU use, this will help to protect register from unexpected writing, bind output with "IDUN_O_writeBackRegType"
    // 11) Priv Arch indicator, can tell simulation environment to stop simulating, bind output with "IDUN_O_privState"
    // 12) Instruction Indicator, debug-only signal, every instruction have unique number, this will be disabled if not debugging, bind output with "IDUN_O_InstIndicator"
    // 13) Instruction Type Indicator, debug-only signal, every instruction type have unique number, this will be diabled if not debugging, bind output with "IDUN_O_InstType"
    // 14) Immediate Value, bind output with "IDUN_O_imm"
    // 15) next PC type, static (+4) or may be dynamic, bind output with "IDUN_O_nextPCStatic"
    // 16) reason of PC jump (if not jump then select NoJumpPC), so Write-Back Unit can process PC Mask if needed, bind output with "IDUN_O_PCJumpReason"
    // 17) bool, indicator of sucessfully decoding, if not, means that this is a 16 bit inst, will use IDUC's result

    var IDUN_opcodes_EXU_Int = ListLookup(
        /*Compare Item: */           io.IDUN_I_inst,
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
        rv64_bitpat.bitpat_SRAI   -> List(opcodes_EXU_Int.Int_SHR, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_ADD    -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SUB    -> List(opcodes_EXU_Int.Int_SUB, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SLL    -> List(opcodes_EXU_Int.Int_SHL, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SLT    -> List(opcodes_EXU_Int.Int_BLT, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SLTU   -> List(opcodes_EXU_Int.Int_BLT, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_XOR    -> List(opcodes_EXU_Int.Int_XOR, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SRL    -> List(opcodes_EXU_Int.Int_SHR, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SRA    -> List(opcodes_EXU_Int.Int_SHR, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_OR     -> List(opcodes_EXU_Int.Int_OR,  opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_AND    -> List(opcodes_EXU_Int.Int_AND, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_LWU    -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_LD     -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SD     -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_ADDIW  -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SLLIW  -> List(opcodes_EXU_Int.Int_SHL, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SRLIW  -> List(opcodes_EXU_Int.Int_SHR, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_SRAIW  -> List(opcodes_EXU_Int.Int_SHR, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_RegImm),
        rv64_bitpat.bitpat_ADDW   -> List(opcodes_EXU_Int.Int_ADD, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SUBW   -> List(opcodes_EXU_Int.Int_SUB, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SLLW   -> List(opcodes_EXU_Int.Int_SHL, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SRLW   -> List(opcodes_EXU_Int.Int_SHR, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_SRAW   -> List(opcodes_EXU_Int.Int_SHR, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_MUL    -> List(opcodes_EXU_Int.Int_MUL, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_MULH   -> List(opcodes_EXU_Int.Int_MUL, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_High, opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_MULHSU -> List(opcodes_EXU_Int.Int_MUL, opcodes_EXU_Int_sign.Int_Signed_Unsigned,   opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_High, opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_MULHU  -> List(opcodes_EXU_Int.Int_MUL, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_High, opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_DIV    -> List(opcodes_EXU_Int.Int_DIV, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_DIVU   -> List(opcodes_EXU_Int.Int_DIV, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_REM    -> List(opcodes_EXU_Int.Int_REM, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_REMU   -> List(opcodes_EXU_Int.Int_REM, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Double, opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_MULW   -> List(opcodes_EXU_Int.Int_MUL, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_DIVW   -> List(opcodes_EXU_Int.Int_DIV, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_DIVUW  -> List(opcodes_EXU_Int.Int_DIV, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_REMW   -> List(opcodes_EXU_Int.Int_REM, opcodes_EXU_Int_sign.Int_Signed_Signed,     opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg),
        rv64_bitpat.bitpat_REMUW  -> List(opcodes_EXU_Int.Int_REM, opcodes_EXU_Int_sign.Int_Unsigned_Unsigned, opcodes_EXU_Int_computeLength.Int_Word,   opcodes_EXU_Int_resultPart.Int_Low,  opcodes_EXU_Int_opreand.Int_TwoReg)
        )
    )

    val IDUN_opcodes_priv = ListLookup(
        /*Compare Item: */            io.IDUN_I_inst,
        /*Default Vals: */            List(opcodes_IDU_csrOps.csr_nope , opcodes_IDU_privOps.priv_nope ), Array(
        rv64_bitpat.bitpat_CSRRW   -> List(opcodes_IDU_csrOps.csr_write, opcodes_IDU_privOps.priv_nope ),
        rv64_bitpat.bitpat_CSRRS   -> List(opcodes_IDU_csrOps.csr_set  , opcodes_IDU_privOps.priv_nope ),
        rv64_bitpat.bitpat_CSRRC   -> List(opcodes_IDU_csrOps.csr_clear, opcodes_IDU_privOps.priv_nope ),
        rv64_bitpat.bitpat_ECALL   -> List(opcodes_IDU_csrOps.csr_nope , opcodes_IDU_privOps.priv_ecall),
        rv64_bitpat.bitpat_MRET    -> List(opcodes_IDU_csrOps.csr_nope , opcodes_IDU_privOps.priv_mret )
        )
    )

    var IDUN_opcodes_LSU = ListLookup(
        /*Compare Item: */           io.IDUN_I_inst,
        /*Default Vals: */           List(opcodes_LSU.LSU_NOPE,  opcodes_LSU_sign.LSU_Unsigned, opcodes_LSU_len.LSU_Byte,   opcodes_LSU_memOpreationType.LSU_NOP), Array(
        rv64_bitpat.bitpat_LB     -> List(opcodes_LSU.LSU_LOAD,  opcodes_LSU_sign.LSU_Signed,   opcodes_LSU_len.LSU_Byte,   opcodes_LSU_memOpreationType.LSU_R),
        rv64_bitpat.bitpat_LH     -> List(opcodes_LSU.LSU_LOAD,  opcodes_LSU_sign.LSU_Signed,   opcodes_LSU_len.LSU_Half,   opcodes_LSU_memOpreationType.LSU_R),
        rv64_bitpat.bitpat_LW     -> List(opcodes_LSU.LSU_LOAD,  opcodes_LSU_sign.LSU_Signed,   opcodes_LSU_len.LSU_Word,   opcodes_LSU_memOpreationType.LSU_R),
        rv64_bitpat.bitpat_LBU    -> List(opcodes_LSU.LSU_LOAD,  opcodes_LSU_sign.LSU_Unsigned, opcodes_LSU_len.LSU_Byte,   opcodes_LSU_memOpreationType.LSU_R),
        rv64_bitpat.bitpat_LHU    -> List(opcodes_LSU.LSU_LOAD,  opcodes_LSU_sign.LSU_Unsigned, opcodes_LSU_len.LSU_Half,   opcodes_LSU_memOpreationType.LSU_R),
        rv64_bitpat.bitpat_SB     -> List(opcodes_LSU.LSU_LOAD,  opcodes_LSU_sign.LSU_Signed,   opcodes_LSU_len.LSU_Byte,   opcodes_LSU_memOpreationType.LSU_W),
        rv64_bitpat.bitpat_SH     -> List(opcodes_LSU.LSU_LOAD,  opcodes_LSU_sign.LSU_Signed,   opcodes_LSU_len.LSU_Half,   opcodes_LSU_memOpreationType.LSU_W),
        rv64_bitpat.bitpat_SW     -> List(opcodes_LSU.LSU_LOAD,  opcodes_LSU_sign.LSU_Signed,   opcodes_LSU_len.LSU_Word,   opcodes_LSU_memOpreationType.LSU_W),
        rv64_bitpat.bitpat_LWU    -> List(opcodes_LSU.LSU_LOAD,  opcodes_LSU_sign.LSU_Unsigned, opcodes_LSU_len.LSU_Word,   opcodes_LSU_memOpreationType.LSU_R),
        rv64_bitpat.bitpat_LD     -> List(opcodes_LSU.LSU_LOAD,  opcodes_LSU_sign.LSU_Signed,   opcodes_LSU_len.LSU_Double, opcodes_LSU_memOpreationType.LSU_R),
        rv64_bitpat.bitpat_SD     -> List(opcodes_LSU.LSU_LOAD,  opcodes_LSU_sign.LSU_Signed,   opcodes_LSU_len.LSU_Double, opcodes_LSU_memOpreationType.LSU_W)
        )
    )

    var IDUN_opcodes_AMO = ListLookup(
        /*Compare Item: */             io.IDUN_I_inst,
        /*Default Vals: */             List(opcodes_AMO_memOps.amo_nope, opcodes_AMO_opLen.amoLen_Double), Array(
        rv64_bitpat.bitpat_LRW      -> List(opcodes_AMO_memOps.amo_lr,   opcodes_AMO_opLen.amoLen_Word),
        rv64_bitpat.bitpat_SCW      -> List(opcodes_AMO_memOps.amo_sc,   opcodes_AMO_opLen.amoLen_Word),
        rv64_bitpat.bitpat_AMOSWAPW -> List(opcodes_AMO_memOps.amo_swap, opcodes_AMO_opLen.amoLen_Word),
        rv64_bitpat.bitpat_AMOADDW  -> List(opcodes_AMO_memOps.amo_add,  opcodes_AMO_opLen.amoLen_Word),
        rv64_bitpat.bitpat_AMOXORW  -> List(opcodes_AMO_memOps.amo_xor,  opcodes_AMO_opLen.amoLen_Word),
        rv64_bitpat.bitpat_AMOORW   -> List(opcodes_AMO_memOps.amo_or,   opcodes_AMO_opLen.amoLen_Word),
        rv64_bitpat.bitpat_AMOMINW  -> List(opcodes_AMO_memOps.amo_min,  opcodes_AMO_opLen.amoLen_Word),
        rv64_bitpat.bitpat_AMOMAXW  -> List(opcodes_AMO_memOps.amo_max,  opcodes_AMO_opLen.amoLen_Word),
        rv64_bitpat.bitpat_AMOMINUW -> List(opcodes_AMO_memOps.amo_minu, opcodes_AMO_opLen.amoLen_Word),
        rv64_bitpat.bitpat_AMOMAXUW -> List(opcodes_AMO_memOps.amo_maxu, opcodes_AMO_opLen.amoLen_Word),
        rv64_bitpat.bitpat_LRD      -> List(opcodes_AMO_memOps.amo_lr,   opcodes_AMO_opLen.amoLen_Double),
        rv64_bitpat.bitpat_SCD      -> List(opcodes_AMO_memOps.amo_sc,   opcodes_AMO_opLen.amoLen_Double),
        rv64_bitpat.bitpat_AMOSWAPD -> List(opcodes_AMO_memOps.amo_swap, opcodes_AMO_opLen.amoLen_Double),
        rv64_bitpat.bitpat_AMOADDD  -> List(opcodes_AMO_memOps.amo_add,  opcodes_AMO_opLen.amoLen_Double),
        rv64_bitpat.bitpat_AMOXORD  -> List(opcodes_AMO_memOps.amo_xor,  opcodes_AMO_opLen.amoLen_Double),
        rv64_bitpat.bitpat_AMOORD   -> List(opcodes_AMO_memOps.amo_or,   opcodes_AMO_opLen.amoLen_Double),
        rv64_bitpat.bitpat_AMOMIND  -> List(opcodes_AMO_memOps.amo_min,  opcodes_AMO_opLen.amoLen_Double),
        rv64_bitpat.bitpat_AMOMAXD  -> List(opcodes_AMO_memOps.amo_max,  opcodes_AMO_opLen.amoLen_Double),
        rv64_bitpat.bitpat_AMOMINUD -> List(opcodes_AMO_memOps.amo_minu, opcodes_AMO_opLen.amoLen_Double),
        rv64_bitpat.bitpat_AMOMAXUD -> List(opcodes_AMO_memOps.amo_maxu, opcodes_AMO_opLen.amoLen_Double)
        )
    )

    var IDUN_opcodes_MACRO = ListLookup(
        /*Compare Item: */           io.IDUN_I_inst,
        /*Default Vals: */           List(opcodes_writeBackGPRType.WB_GPR_NOP,      opcodes_IDU_privState.ERROR,  rv64_opcodes.opcode_DoNothing, inst_types.inst_E, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_fail), Array(
        rv64_bitpat.bitpat_LUI    -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_LUI,       inst_types.inst_U, SignExtend_immU, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_AUIPC  -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_AUIPC,     inst_types.inst_U, SignExtend_immU, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_JAL    -> List(opcodes_writeBackGPRType.WB_GPR_SNPC,     opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_JAL,       inst_types.inst_J, SignExtend_immJ, opcodes_nextPCTypes.PC_Next_Dynamic, opcodes_PCJumpReason.JAL_Inst  , decode_idun.idun_success),
        rv64_bitpat.bitpat_JALR   -> List(opcodes_writeBackGPRType.WB_GPR_SNPC,     opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_JALR,      inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Dynamic, opcodes_PCJumpReason.JALR_Inst , decode_idun.idun_success),
        rv64_bitpat.bitpat_BEQ    -> List(opcodes_writeBackGPRType.WB_GPR_NOP,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_BEQ,       inst_types.inst_B, SignExtend_immB, opcodes_nextPCTypes.PC_Next_Dynamic, opcodes_PCJumpReason.BranchInst, decode_idun.idun_success),
        rv64_bitpat.bitpat_BNE    -> List(opcodes_writeBackGPRType.WB_GPR_NOP,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_BNE,       inst_types.inst_B, SignExtend_immB, opcodes_nextPCTypes.PC_Next_Dynamic, opcodes_PCJumpReason.BranchInst, decode_idun.idun_success),
        rv64_bitpat.bitpat_BLT    -> List(opcodes_writeBackGPRType.WB_GPR_NOP,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_BLT,       inst_types.inst_B, SignExtend_immB, opcodes_nextPCTypes.PC_Next_Dynamic, opcodes_PCJumpReason.BranchInst, decode_idun.idun_success),
        rv64_bitpat.bitpat_BGE    -> List(opcodes_writeBackGPRType.WB_GPR_NOP,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_BGE,       inst_types.inst_B, SignExtend_immB, opcodes_nextPCTypes.PC_Next_Dynamic, opcodes_PCJumpReason.BranchInst, decode_idun.idun_success),
        rv64_bitpat.bitpat_BLTU   -> List(opcodes_writeBackGPRType.WB_GPR_NOP,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_BLTU,      inst_types.inst_B, SignExtend_immB, opcodes_nextPCTypes.PC_Next_Dynamic, opcodes_PCJumpReason.BranchInst, decode_idun.idun_success),
        rv64_bitpat.bitpat_BGEU   -> List(opcodes_writeBackGPRType.WB_GPR_NOP,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_BGEU,      inst_types.inst_B, SignExtend_immB, opcodes_nextPCTypes.PC_Next_Dynamic, opcodes_PCJumpReason.BranchInst, decode_idun.idun_success),
        rv64_bitpat.bitpat_LB     -> List(opcodes_writeBackGPRType.WB_GPR_LSU,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_LB,        inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_LH     -> List(opcodes_writeBackGPRType.WB_GPR_LSU,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_LH,        inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_LW     -> List(opcodes_writeBackGPRType.WB_GPR_LSU,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_LW,        inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_LBU    -> List(opcodes_writeBackGPRType.WB_GPR_LSU,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_LBU,       inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_LHU    -> List(opcodes_writeBackGPRType.WB_GPR_LSU,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_LHU,       inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SB     -> List(opcodes_writeBackGPRType.WB_GPR_NOP,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SB,        inst_types.inst_S, SignExtend_immS, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SH     -> List(opcodes_writeBackGPRType.WB_GPR_NOP,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SH,        inst_types.inst_S, SignExtend_immS, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SW     -> List(opcodes_writeBackGPRType.WB_GPR_NOP,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SW,        inst_types.inst_S, SignExtend_immS, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_ADDI   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_ADDI,      inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SLTI   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Bool, opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SLTI,      inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SLTIU  -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Bool, opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SLTIU,     inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_XORI   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_XORI,      inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_ORI    -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_ORI,       inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_ANDI   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_ANDI,      inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SLLI   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SLLI,      inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SRLI   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SRLI,      inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SRAI   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SRAI,      inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_ADD    -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_ADD,       inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SUB    -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SUB,       inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SLL    -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SLL,       inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SLT    -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Bool, opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SLT,       inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SLTU   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Bool, opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SLTU,      inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_XOR    -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_XOR,       inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SRL    -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SRL,       inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SRA    -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SRA,       inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_OR     -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_OR,        inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_AND    -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_AND,       inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_LWU    -> List(opcodes_writeBackGPRType.WB_GPR_LSU,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_LWU,       inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_LD     -> List(opcodes_writeBackGPRType.WB_GPR_LSU,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_LD,        inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SD     -> List(opcodes_writeBackGPRType.WB_GPR_NOP,      opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SD,        inst_types.inst_S, SignExtend_immS, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_ADDIW  -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_ADDIW,     inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SLLIW  -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SLLIW,     inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SRLIW  -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SRLIW,     inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SRAIW  -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SRAIW,     inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_ADDW   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_ADDW,      inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SUBW   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SUBW,      inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SLLW   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SLLW,      inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SRLW   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SRLW,      inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_SRAW   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_SRAW,      inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_MUL    -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_MUL,       inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_MULH   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_MULH,      inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_MULHSU -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_MULHSU,    inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_MULHU  -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_MULHU,     inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_DIV    -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_DIV,       inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_DIVU   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_DIVU,      inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_REM    -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_REM,       inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_REMU   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_REMU,      inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_MULW   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_MULW,      inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_DIVW   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_DIVW,      inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_DIVUW  -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_DIVUW,     inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_REMW   -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_REMW,      inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_REMUW  -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_REMUW,     inst_types.inst_R, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_ECALL  -> List(opcodes_writeBackGPRType.WB_GPR_NOP,      opcodes_IDU_privState.ECALL,  rv64_opcodes.opcode_ECALL,     inst_types.inst_N, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Dynamic, opcodes_PCJumpReason.ECALL_Inst, decode_idun.idun_success),
        rv64_bitpat.bitpat_EBREAK -> List(opcodes_writeBackGPRType.WB_GPR_NOP,      opcodes_IDU_privState.EBREAK, rv64_opcodes.opcode_EBREAK,    inst_types.inst_N, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_MRET   -> List(opcodes_writeBackGPRType.WB_GPR_NOP,      opcodes_IDU_privState.MRET,   rv64_opcodes.opcode_MRET,      inst_types.inst_N, NoImmediateNum,  opcodes_nextPCTypes.PC_Next_Dynamic, opcodes_PCJumpReason.MRET_Inst , decode_idun.idun_success),
        rv64_bitpat.bitpat_CSRRW  -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_CSRRW,     inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_CSRRS  -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_CSRRS,     inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success),
        rv64_bitpat.bitpat_CSRRC  -> List(opcodes_writeBackGPRType.WB_GPR_EXU_Val,  opcodes_IDU_privState.NORMAL, rv64_opcodes.opcode_CSRRC,     inst_types.inst_I, SignExtend_immI, opcodes_nextPCTypes.PC_Next_Static,  opcodes_PCJumpReason.NoJumpPC  , decode_idun.idun_success)
        )
    )

    // Decoding finished internally, write these datas to their sperate ports
    io.IDUN_O_EXU_Int_opcode        := IDUN_opcodes_EXU_Int(0)
    io.IDUN_O_EXU_Int_sign          := IDUN_opcodes_EXU_Int(1)
    io.IDUN_O_EXU_Int_computeLength := IDUN_opcodes_EXU_Int(2)
    io.IDUN_O_EXU_Int_resultPart    := IDUN_opcodes_EXU_Int(3)
    io.IDUN_O_EXU_Int_operand       := IDUN_opcodes_EXU_Int(4)

    io.IDUN_O_LSU_optype            := IDUN_opcodes_LSU(0)
    io.IDUN_O_LSU_sign              := IDUN_opcodes_LSU(1)
    io.IDUN_O_LSU_len               := IDUN_opcodes_LSU(2)
    io.IDUN_O_LSU_memOperationType  := IDUN_opcodes_LSU(3)

    io.IDUN_O_AMO_memOps            := IDUN_opcodes_AMO(0)
    io.IDUN_O_AMO_opLen             := IDUN_opcodes_AMO(1)
    io.IDUN_O_AMO_aqrl              := io.IDUN_I_inst(26, 25)

    io.IDUN_O_writeBackRegType      := IDUN_opcodes_MACRO(0)
    io.IDUN_O_privState             := IDUN_opcodes_MACRO(1)
    io.IDUN_O_InstIndicator         := IDUN_opcodes_MACRO(2)
    io.IDUN_O_InstType              := IDUN_opcodes_MACRO(3)
    io.IDUN_O_imm                   := IDUN_opcodes_MACRO(4)
    io.IDUN_O_nextPCStatic          := IDUN_opcodes_MACRO(5)
    io.IDUN_O_PCJumpReason          := IDUN_opcodes_MACRO(6)
    io.IDUN_O_devodeSuccess         := IDUN_opcodes_MACRO(7)

    io.IDUN_O_csrOperation          := IDUN_opcodes_priv(0)
    io.IDUN_O_privOperation         := IDUN_opcodes_priv(1)
    io.IDUN_O_csrAddr               := immI.asUInt
}