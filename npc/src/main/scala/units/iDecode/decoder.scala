/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing 101 High School
* Copyright (c) 2023 Yusong Yan, University of Washington - Seattle
*
* YSYX-NPC is licensed under Mulan PSL v2.
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

package npc.units.iDecode

import chisel3._
import chisel3.util._

import npc.helper.defs.Base._
import npc.helper.rv64im.Inst._
import npc.helper.opcode.OpEXU._
import npc.helper.opcode.OpLSU._
import npc.helper.opcode.OpWBU._
import npc.helper.opcode.OpPriv._
import npc.helper.opcode.OpState._

object iDecodeInternal extends Bundle{
    // ON-PIPELINE VALUES
    val oSlaveReady = Output(Bool())
    val iSlaveValid = Input(Bool())

    val iMasterReady = Input(Bool())
    val oMasterValid = Output(Bool())

    val iInst = Input(UInt(InstWidth.W))
    val oDynamicNextPC = Output(UInt(AddrWidth.W))
    val oStaticNextPC = Output(UInt(AddrWidth.W))
    
    // Combine EXU, LSU, WBU 's decode information together
    // 1   1   1   1   1   1   0   0   0   0   0   0   0   0   0   0
    // 5   4   3   2   1   0   9   8   7   6   5   4   3   2   1   0
    // |---|   |-----------------------|   |---|   |---|   |---|   |
    // Priv               EXU              LSlen   LSfunc  WBTyp  Debug State (DS)
    val oDecodeBundle = Output(UInt(DecodeWidth.W))

    val EX_src1 = Output(UInt(DataWidth.W))
    val EX_src2 = Output(UInt(DataWidth.W))
    // Load-Store's src1 is EXU's result
    val LS_src2 = Output(UInt(DataWidth.W))

    // OFF-PIPELINE VALUES
    val CSR_FetchAddr = Output(UInt(CSRIDWidth.W)) // RV have 4096 CSRs
    val CSR_FetchRetu = Input(UInt(DataWidth.W))
    val CSR_ChangeVal = Output(UInt(DataWidth.W))

    val oRS1 = Output(UInt(RegIDWidth.W))
    val oRS2 = Output(UInt(RegIDWidth.W))
    val oRD  = Output(UInt(RegIDWidth.W))

    val iSRC1 = Input(UInt(DataWidth.W))
    val iSRC2 = Input(UInt(DataWidth.W))

    val iPC = Input(UInt(AddrWidth.W))
    val oPC = Output(UInt(AddrWidth.W))
    val oSNPC = Output(UInt(AddrWidth.W))
    val oDNPC = Output(UInt(AddrWidth.W))

    val iCSR_mcause = Input(UInt(DataWidth.W))
    val iCSR_mtvec = Input(UInt(DataWidth.W))
    val iCSR_mstatus = Input(UInt(DataWidth.W))
    val iCSR_mepc = Input(UInt(DataWidth.W))

    val oCSR_mcause = Output(UInt(DataWidth.W))
    val oCSR_mtvec = Output(UInt(DataWidth.W))
    val oCSR_mstatus = Output(UInt(DataWidth.W))
    val oCSR_mepc = Output(UInt(DataWidth.W))
}

class IDU extends Module{
    ioInternal = IO(new iDecodeInternal)
    // Instruction Decode Unit: First generate opreation code, then send operands to EXU and LSU (for operand send to LSU, it needs to be delivered to EXU first), last write back PC and CSR (WBU only write back GPR)

    val DecodeReg = RegInit(0.U(DecodeWidth.W))
    val iDecodeEnable = RegInit(true.B)

    val iDecPrivReg = RegInit(0.U(2.W))
    val iDecEXUReg = RegInit(0.U(7.W))
    val iDecLSlenReg = RegInit(0.U(2.W))
    val iDecLSfuncReg = RegInit(0.U(2.W))
    val iDecWBTypReg = RegInit(0.U(2.W))
    val iDecDSReg = RegInit(0.U(1.W))

    // Only can decode instruction if Master (IFU) 's output is valid
    Mux(ioInternal.iSlaveValid.asBool, iDecodeEnable := true.B, iDecodeEnable := false.B)

    // Decode PrivReg
    Mux(iDecodeEnable.asBool, iDecPrivReg := 0.U(2.W), iDecPrivReg := Lookup(
        ioInternal.iInst, PR_NORM, Array(
            ECALL -> PR_ECALL, MRET -> PR_MRET, EBREAK -> PR_EBREAK
        )
    ))

    // Decode EXUReg
    Mux(iDecodeEnable.asBool, iDecEXUReg := 0.U(7.W), iDecEXUReg := Lookup(
        ioInternal.iInst, EX_NOP, Array(
            LUI -> EX_PS1, 

            AUIPC -> EX_ADD, SB -> EX_ADD, SH -> EX_ADD, SW -> EX_ADD, SD -> EX_ADD, ADD -> EX_ADD, ADDI -> EX_ADD,
            LB -> EX_ADD, LBU -> EX_ADD, LH -> EX_ADD, LHU -> EX_ADD, LW -> EX_ADD, LWU -> EX_ADD, LD -> EX_ADD,
            ADDW -> EX_ADDW, ADDIW -> EX_ADDW,

            SUB -> EX_SUB, SUBW -> EX_SUBW,

            AND -> EX_AND, ANDI -> EX_AND,

            OR -> EX_OR, ORI -> EX_OR,

            XOR -> EX_XOR, XORI -> EX_XOR,

            SLT -> EX_SLT, SLTI -> EX_SLT,

            SLTU -> EX_SLTU, SLTIU -> EX_SLTU,

            SLL -> EX_SLL, SLLI -> EX_SLL, SLLW -> EX_SLLW, SLLIW -> EX_SLLW,

            SRL -> EX_SRL, SRLI -> EX_SRL, SRLW -> EX_SRLW, SRLIW -> EX_SRLW,

            SRA -> EX_SRA, SRAI -> EX_SRA, SRAW -> EX_SRAW, SRLIW -> EX_SRAW,

            MUL -> EX_MUL, MULH -> EX_MULH, MULHSU -> EX_MULHSU, MULHU -> EX_MULHU,

            DIV -> EX_DIV, DIVU -> EX_DIVU, DIVW -> EX_DIVW, DIVUW -> EX_DIVUW,

            REM -> EX_REM, REMU -> EX_REMU, REMW -> EX_REMW, REMUW -> EX_REMUW,

            //JAL -> EX_NOP, JALR -> EX_NOP, BEQ -> EX_NOP, BNE -> EX_NOP, BLT -> EX_NOP, BLTU -> EX_NOP, BGE -> EX_NOP, BGEU -> EX_NOP,
        )
    ))

    // Decode LSlenReg
    Mux(iDecodeEnable.asBool, iDecLSlenReg := 0.U(2.W), iDecLSlenReg := Lookup(
        ioInternal.iInst, 0.U(2.W), Array(
            LB -> LS_B, LBU -> LS_B, SB -> LS_B,
            LH -> LS_H, LHU -> LS_H, SH -> LS_H,
            LW -> LS_W, LWU -> LS_W, SW -> LS_W,
            LD -> LS_D,              SD -> LS_D
        )
    ))

    // Decode LSfuncReg
    Mux(iDecodeEnable.asBool, iDecLSfuncReg := LS_NOP, iDecLSfuncReg := Lookup(
        ioInternal.iInst, LS_NOP, Array(
            LB  -> LS_LD ,  LH -> LS_LD ,  LW -> LS_LD ,  LD -> LS_LD ,
            SB  -> LS_ST ,  SH -> LS_ST ,  SW -> LS_ST ,  SD -> LS_ST ,
            LBU -> LS_LDU, LHU -> LS_LDU, LWU -> LS_LDU
        )
    ))

    // Decode WBTypReg
    Mux(iDecodeEnable.asBool, iDecWBTypReg := WB_NOP, iDecWBTypReg := Lookup(
        ioInternal.iInst, WB_EXU, Array(
            SB -> WB_NOP, SH -> WB_NOP, SW -> WB_NOP, SD -> WB_NOP,
            ECALL -> WB_NOP, EBREAK -> WB_NOP, MRET -> WB_NOP,
            BEQ -> WB_NOP, BNE -> WB_NOP, BGE -> WB_NOP, BGEU -> WB_NOP, BLT -> WB_NOP, BLTU -> WB_NOP,

            LB -> WB_LSU, LBU -> WB_LSU, LH -> WB_LSU, LHU -> WB_LSU, LW -> WB_LSU, LWU -> WB_LSU, LD -> WB_LSU,

            JAL -> WB_SNPC, JALR -> WB_SNPC
        )
    ))

    // Decode DSReg
    Mux(iDecodeEnable.asBool, iDecDSReg := 0.U(1.W), iDecDSReg := Lookup(
        ioInternal.iInst, NPC_RUNNING, Array(
            EBREAK -> NPC_STOPPED
        )
    ))

    // Combine these decode results together when iDecodeEnable is true
    Mux(iDecodeEnable.asBool, Cat(iDecPrivReg.asUInt, Cat(iDecEXUReg.asUInt, Cat(iDecLSlenReg.asUInt, Cat(iDecLSfuncReg.asUInt, Cat(iDecWBTypReg.asUInt, iDecDSReg.asUInt))))), 0.U(DecodeWidth.W))

    // Decode Instruction type
    val InstructionType = Lookup(
        ioInternal.iInst, instR, Array(
            LUI -> instU, AUIPC -> instU,

            JAL -> instJ,

            JALR -> instI,
            LB -> instI, LH -> instI, LW -> instI, LBU -> instI, LHU -> instI, LWU -> instI, LD -> instI,
            ADDI -> instI, SLTI -> instI, SLTIU -> instI, XORI -> instI, ORI -> instI, ANDI -> instI, SLLI -> instI, SRLI -> instI, SRAI -> instI,
            ADDIW -> instI, SLLIW -> instI, SRLIW -> instI, SRAIW -> instI,

            CSRRW -> instI, CSRRS -> instI, CSRRC -> instI, CSRRWI -> instI, CSRRSI -> instI, CSRRCI -> instI,

            ADD -> instR, SUB -> instR, SLL -> instR, SLT -> instR, SLTU -> instR, XOR -> instR, SRL -> instR, SRA -> instR, OR -> instR, AND -> instR,
            ADDW -> instR, SUBW -> instR, SLLW -> instR, SRLW -> instR, SRAW -> instR,
            MUL -> instR, MULH -> instR, MULHSU -> instR, MULHU -> instR, DIV -> instR, DIVU -> instR, REM -> instR, REMU -> instR,
            MULW -> instR, DIVW -> instR, DIVUW -> instR, REMW -> instR, REMUW -> instR,

            SB -> instS, SH -> instS, SW -> instS, SD -> instS,

            BEQ -> instB, BNE -> instB, BLT -> instB, BGE -> instB, BLTU -> instB, BGEU -> instB,
        )
    )

    // Connect imm-generator
    val ImmGenerator = Module(new immGen)
    ImmGenerator.ioSubmodule.iInst := ioInternal.iInst
    ImmGenerator.ioSubmodule.iType := InstructionType
    val immOut = ImmGenerator.ioSubmodule.oImm

    ioInternal.oSNPC := ioInternal.iPC + 4.U
    ioInternal.oDNPC := Lookup(
        ioInternal.iInst, ioInternal.iPC + 4.U, Array(
            // TODO: Add decoding for dynamic next PC
        ))

    // TODO: Calculate CSR (mstatus, macuse, mtvec, mepc)

    // Disable instruction decoding
    iDecodeEnable := false.B

    ioInternal.oDecodeBundle := DecodeReg
    // TODO: connect more decode signals

    ioInternal.oMasterValid := true.B

    when(ioInternal.iMasterReady.asBool){
        // Shake hand success, re-enable iDecode, decode next instruction if IFU have result
        iDecodeEnable := true.B
    }
}