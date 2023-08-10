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
import npc.helper.defs.InstType._
import npc.helper.rv64im.Inst._
import npc.helper.opcode.OpEXU._
import npc.helper.opcode.OpLSULen._
import npc.helper.opcode.OpLSUFunc._
import npc.helper.opcode.OpWBU._
import npc.helper.opcode.OpPriv._
import npc.helper.opcode.OpState._

class iDecodeInternal extends Bundle{
    // ON-PIPELINE VALUES
    val oSlaveReady = Output(Bool())
    val iSlaveValid = Input(Bool())

    val iMasterReady = Input(Bool())
    val oMasterValid = Output(Bool())

    val iInst = Input(UInt(InstWidth.W))
    
    // Combine EXU, LSU, WBU 's decode information together
    // 1   1   1   1   1   1   0   0   0   0   0   0   0   0   0   0
    // 5   4   3   2   1   0   9   8   7   6   5   4   3   2   1   0
    // |---|   |-----------------------|   |---|   |---|   |---|   |
    // Priv               EXU              LSlen   LSfunc  WBTyp  Debug State (DS)
    val oDecodeBundle = Output(UInt(DecodeWidth.W))

    val oEXU_src1 = Output(UInt(DataWidth.W))
    val oEXU_src2 = Output(UInt(DataWidth.W))
    // Load-Store's src1 is EXU's result
    val oLSU_src2 = Output(UInt(DataWidth.W))

    val oRD = Output(UInt(RegIDWidth.W))

    val iPC = Input(UInt(AddrWidth.W))
    val oPC = Output(UInt(AddrWidth.W))

    val oFeedBackPCChanged = Output(Bool())
    val oFeedBackNewPCVal = Output(UInt(AddrWidth.W))

    // OFF-PIPELINE VALUES

    val oRS1 = Output(UInt(RegIDWidth.W))
    val oRS2 = Output(UInt(RegIDWidth.W))

    val iSRC1 = Input(UInt(DataWidth.W))
    val iSRC2 = Input(UInt(DataWidth.W))

    val iCSR_ZicsrOldVal = Input(UInt(DataWidth.W))
    val oCSR_ZicsrNewVal = Output(UInt(DataWidth.W))
    val oCSR_ZicsrWSCIdx = Output(UInt(CSRIDWidth.W))

    val iCSR_mtvec = Input(UInt(DataWidth.W))
    val iCSR_mepc = Input(UInt(DataWidth.W))
}

class IDU extends Module{
    val ioInternal = IO(new iDecodeInternal)
    // Instruction Decode Unit: First generate opreation code, then send operands to EXU and LSU (for operand send to LSU, it needs to be delivered to EXU first), last write back PC and CSR (WBU only write back GPR)

    val DecodeVal = RegInit(0.U(DecodeWidth.W))
    //val iDecodeEnable = RegInit(true.B)

    val iDecPrivVal = RegInit(0.U(iDecPrivValLen.W))
    val iDecEXUVal = RegInit(0.U(iDecEXUValLen.W))
    val iDecLSlenVal = RegInit(0.U(iDecLSlenValLen.W))
    val iDecLSfuncVal = RegInit(0.U(iDecLSfuncValLen.W))
    val iDecWBTypVal = RegInit(0.U(iDecWBTypValLen.W))
    val iDecDSVal = RegInit(0.U(iDecDSValLen.W))

    val InstructionType = RegInit(0.U(InstTypeWidth.W))
    val ImmOut = RegInit(0.U(DataWidth.W))

    val RS1Addr = RegInit(0.U(RegIDWidth.W))
    val RS2Addr = RegInit(0.U(RegIDWidth.W))
    val SRC1Val = RegInit(0.U(DataWidth.W))
    val SRC2Val = RegInit(0.U(DataWidth.W))
    val RDAddr = RegInit(0.U(RegIDWidth.W))

    val SNPC = RegInit(0.U(AddrWidth.W))
    val DNPC = RegInit(0.U(AddrWidth.W))
    val FeedBackJumpPC = RegInit(false.B)
    val FeedbackPCVal = RegInit(0.U(AddrWidth.W))

    val OldCSR = RegInit(0.U(DataWidth.W))
    val ZicsrWSCIdx = RegInit(0.U(CSRIDWidth.W))
    val ZicsrNewVal = RegInit(0.U(DataWidth.W))

    val EXU_SRC1 = RegInit(0.U(DataWidth.W))
    val EXU_SRC2 = RegInit(0.U(DataWidth.W))
    val LSU_SRC2 = RegInit(0.U(DataWidth.W))

    val IDU_NotBusy = RegInit(true.B)

    // Connect imm-generator
    val ImmGenerator = Module(new immGen)
    ImmGenerator.ioSubmodule.iInst := ioInternal.iInst
    ImmGenerator.ioSubmodule.iType := InstructionType

    when(ioInternal.iSlaveValid.asBool && ioInternal.iMasterReady.asBool){
        // Decode PrivReg
        iDecPrivVal := Lookup(
            ioInternal.iInst, PR_NORM, Array(
                ECALL -> PR_ECALL, MRET -> PR_MRET,
                CSRRW -> PR_ZICSR, CSRRC -> PR_ZICSR, CSRRS -> PR_ZICSR,
                CSRRWI -> PR_ZICSR, CSRRCI -> PR_ZICSR, CSRRSI -> PR_ZICSR
            )
        )

        // Decode EXUReg
        iDecEXUVal := Lookup(
            ioInternal.iInst, EX_NOP, Array(
                LUI -> EX_PS1, //CSRRW -> EX_PS1, CSRRS -> EX_PS1, CSRRC -> EX_PS1,

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

                MUL -> EX_MUL, MULH -> EX_MULH, MULHSU -> EX_MULHSU, MULHU -> EX_MULHU, MULW -> EX_MULW,

                DIV -> EX_DIV, DIVU -> EX_DIVU, DIVW -> EX_DIVW, DIVUW -> EX_DIVUW,

                REM -> EX_REM, REMU -> EX_REMU, REMW -> EX_REMW, REMUW -> EX_REMUW,

                //JAL -> EX_NOP, JALR -> EX_NOP, BEQ -> EX_NOP, BNE -> EX_NOP, BLT -> EX_NOP, BLTU -> EX_NOP, BGE -> EX_NOP, BGEU -> EX_NOP,
            )
        )

        // Decode LSlenReg
        iDecLSlenVal := Lookup(
            ioInternal.iInst, 0.U(2.W), Array(
                LB -> LS_B, LBU -> LS_B, SB -> LS_B,
                LH -> LS_H, LHU -> LS_H, SH -> LS_H,
                LW -> LS_W, LWU -> LS_W, SW -> LS_W,
                LD -> LS_D,              SD -> LS_D
            )
        )

        // Decode LSfuncReg
        iDecLSfuncVal := Lookup(
            ioInternal.iInst, LS_NOP, Array(
                LB  -> LS_LD ,  LH -> LS_LD ,  LW -> LS_LD ,  LD -> LS_LD ,
                SB  -> LS_ST ,  SH -> LS_ST ,  SW -> LS_ST ,  SD -> LS_ST ,
                LBU -> LS_LDU, LHU -> LS_LDU, LWU -> LS_LDU
            )
        )

        // Decode WBTypReg
        iDecWBTypVal := Lookup(
            ioInternal.iInst, WB_EXU, Array(
                SB -> WB_NOP, SH -> WB_NOP, SW -> WB_NOP, SD -> WB_NOP,
                ECALL -> WB_NOP, EBREAK -> WB_NOP, MRET -> WB_NOP,
                BEQ -> WB_NOP, BNE -> WB_NOP, BGE -> WB_NOP, BGEU -> WB_NOP, BLT -> WB_NOP, BLTU -> WB_NOP,

                LB -> WB_LSU, LBU -> WB_LSU, LH -> WB_LSU, LHU -> WB_LSU, LW -> WB_LSU, LWU -> WB_LSU, LD -> WB_LSU,

                JAL -> WB_SNPC, JALR -> WB_SNPC
            )
        )

        // Decode DSReg
        iDecDSVal := Lookup(
            ioInternal.iInst, NPC_RUNNING, Array(
                EBREAK -> NPC_STOPPED
            )
        )

        DecodeVal := Cat(Seq(
            iDecPrivVal, iDecEXUVal, iDecLSlenVal, iDecLSfuncVal, iDecWBTypVal, iDecDSVal
        ))

        // Decode Instruction type
        InstructionType := Lookup(
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

        ImmOut := ImmGenerator.ioSubmodule.oImm

        // Decode for registers
        RS1Addr := ioInternal.iInst(RS1Hi, RS1Lo).asUInt
        SRC1Val := ioInternal.iSRC1

        RS2Addr := ioInternal.iInst(RS2Hi, RS2Lo).asUInt
        SRC2Val := ioInternal.iSRC2

        RDAddr := ioInternal.iInst(RDHi, RDLo).asUInt

        // Decode Static-Next-PC and Dynamic-Next-PC

        SNPC := ioInternal.iPC + InstSize.U
        DNPC := Lookup(
            ioInternal.iInst, ioInternal.iPC + InstSize.U, Array(
                JAL  -> (ioInternal.iPC.asUInt + ImmOut.asUInt),
                JALR -> ((SRC1Val.asUInt + ImmOut.asUInt) & Cat(Fill(63, 1.U(1.W)), Fill(1, 0.U(1.W)))),

                BEQ  -> Mux(SRC1Val.asUInt === SRC2Val.asUInt, ioInternal.iPC.asUInt + ImmOut.asUInt, ioInternal.iPC.asUInt + InstSize.U),
                BNE  -> Mux(SRC1Val.asUInt =/= SRC2Val.asUInt, ioInternal.iPC.asUInt + ImmOut.asUInt, ioInternal.iPC.asUInt + InstSize.U),
                BLT  -> Mux(SRC1Val.asSInt  <  SRC2Val.asSInt, ioInternal.iPC.asUInt + ImmOut.asUInt, ioInternal.iPC.asUInt + InstSize.U),
                BGE  -> Mux(SRC1Val.asSInt  >= SRC2Val.asSInt, ioInternal.iPC.asUInt + ImmOut.asUInt, ioInternal.iPC.asUInt + InstSize.U),
                BLTU -> Mux(SRC1Val.asUInt  <  SRC2Val.asUInt, ioInternal.iPC.asUInt + ImmOut.asUInt, ioInternal.iPC.asUInt + InstSize.U),
                BGEU -> Mux(SRC1Val.asUInt  >= SRC2Val.asUInt, ioInternal.iPC.asUInt + ImmOut.asUInt, ioInternal.iPC.asUInt + InstSize.U),

                ECALL -> ioInternal.iCSR_mtvec.asUInt,
                MRET  -> ioInternal.iCSR_mepc.asUInt
            )
        )
        
        // Judge SNPC is DNPC, if not, trigger value feedback
        FeedBackJumpPC := Mux(SNPC === DNPC, false.B, true.B)
        FeedbackPCVal := DNPC

        ZicsrWSCIdx := Lookup(
            ioInternal.iInst, 0.U(CSRIDWidth.W), Array(
                CSRRW -> ImmOut,  CSRRS -> ImmOut,  CSRRC -> ImmOut,
                CSRRWI -> ImmOut, CSRRSI -> ImmOut, CSRRCI -> ImmOut,
            )
        )

        OldCSR := ioInternal.iCSR_ZicsrOldVal
        val Zicsr_uimm = ioInternal.iInst(RS1Hi, RS1Lo)

        ZicsrNewVal := Lookup(
            ioInternal.iInst, 0.U(DataWidth.W), Array(
                CSRRW -> SRC1Val,
                CSRRS -> (SRC1Val | OldCSR),
                CSRRC -> (SRC1Val & OldCSR),
                CSRRWI -> Zicsr_uimm,
                CSRRSI -> (Zicsr_uimm | OldCSR),
                CSRRCI -> (Zicsr_uimm & OldCSR)
            )
        )

        EXU_SRC1 := MuxCase(0.U(DataWidth.W), Array(
            (InstructionType === instR) -> SRC1Val.asUInt,
            (InstructionType === instI) -> SRC1Val.asUInt,
            (InstructionType === instS) -> SRC1Val.asUInt,
            (InstructionType === instB) -> 0.U(DataWidth.W),
            (InstructionType === instU) -> ImmOut.asUInt,
            (InstructionType === instJ) -> 0.U(DataWidth.W)
        ))

        EXU_SRC2 := MuxCase(0.U(DataWidth.W), Array(
            (InstructionType === instR) -> SRC2Val.asUInt,
            (InstructionType === instI) -> ImmOut.asUInt,
            (InstructionType === instS) -> ImmOut.asUInt,
            (InstructionType === instB) -> 0.U(DataWidth.W),
            (InstructionType === instU) -> ioInternal.iPC.asUInt,
            (InstructionType === instJ) -> 0.U(DataWidth.W)
        ))

        LSU_SRC2 := MuxCase(0.U(DataWidth.W), Array(
            (InstructionType === instR) -> 0.U(DataWidth.W),
            (InstructionType === instI) -> 0.U(DataWidth.W),
            (InstructionType === instS) -> SRC2Val.asUInt,
            (InstructionType === instB) -> 0.U(DataWidth.W),
            (InstructionType === instU) -> 0.U(DataWidth.W),
            (InstructionType === instJ) -> 0.U(DataWidth.W)
        ))
    }

    ioInternal.oRS1 := RS1Addr
    ioInternal.oRS2 := RS2Addr

    ioInternal.oFeedBackPCChanged := FeedBackJumpPC.asBool
    ioInternal.oFeedBackNewPCVal := FeedbackPCVal

    ioInternal.oCSR_ZicsrWSCIdx := ZicsrWSCIdx
    ioInternal.oCSR_ZicsrNewVal := ZicsrNewVal

    // Connect Decode Signals
    ioInternal.oDecodeBundle := DecodeVal
    ioInternal.oEXU_src1 := EXU_SRC1
    ioInternal.oEXU_src2 := EXU_SRC2
    ioInternal.oLSU_src2 := LSU_SRC2
    ioInternal.oRD := RDAddr

    // Connect Pipeline Passthroughs
    ioInternal.oPC := ioInternal.iPC

    // Connect Pipline Signals
    ioInternal.oMasterValid := (IDU_NotBusy.asBool && ioInternal.iSlaveValid)
    ioInternal.oSlaveReady := (IDU_NotBusy.asBool && ioInternal.iMasterReady)
}