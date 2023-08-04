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

    // OFF-PIPELINE VALUES

    val oRS1 = Output(UInt(RegIDWidth.W))
    val oRS2 = Output(UInt(RegIDWidth.W))

    val iSRC1 = Input(UInt(DataWidth.W))
    val iSRC2 = Input(UInt(DataWidth.W))

    val iPC = Input(UInt(AddrWidth.W))
    val oDNPC = Output(UInt(AddrWidth.W))

    val iCSR_ZicsrOldVal = Input(UInt(DataWidth.W))
    val oCSR_ZicsrNewVal = Output(UInt(DataWidth.W))
    val oCSR_ZicsrWSCIdx = Output(UInt(CSRIDWidth.W))
}

class IDU extends Module{
    val ioInternal = IO(new iDecodeInternal)
    // Instruction Decode Unit: First generate opreation code, then send operands to EXU and LSU (for operand send to LSU, it needs to be delivered to EXU first), last write back PC and CSR (WBU only write back GPR)

    val DecodeVal = 0.U(DecodeWidth.W)
    val iDecodeEnable = true.B

    val iDecPrivVal = 0.U(iDecPrivValLen.W)
    val iDecEXUVal = 0.U(iDecEXUValLen.W)
    val iDecLSlenVal = 0.U(iDecLSlenValLen.W)
    val iDecLSfuncVal = 0.U(iDecLSfuncValLen.W)
    val iDecWBTypVal = 0.U(iDecWBTypValLen.W)
    val iDecDSVal = 0.U(iDecDSValLen.W)

    val IDU_NotBusy = RegInit(true.B)

    // Only can decode instruction if Master (IFU) 's output is valid
    Mux(ioInternal.iSlaveValid.asBool, iDecodeEnable := true.B, iDecodeEnable := false.B)

    // Decode PrivReg
    Mux(iDecodeEnable.asBool, iDecPrivVal := 0.U(iDecPrivValLen.W), iDecPrivVal := Lookup(
        ioInternal.iInst, PR_NORM, Array(
            ECALL -> PR_ECALL, MRET -> PR_MRET,
            CSRRW -> PR_ZICSR, CSRRC -> PR_ZICSR, CSRRS -> PR_ZICSR,
            CSRRWI -> PR_ZICSR, CSRRCI -> PR_ZICSR, CSRRSI -> PR_ZICSR
        )
    ))

    // Decode EXUReg
    Mux(iDecodeEnable.asBool, iDecEXUVal := 0.U(iDecEXUValLen.W), iDecEXUVal := Lookup(
        ioInternal.iInst, EX_NOP, Array(
            LUI -> EX_PS1, CSRRW -> EX_PS1, CSRRS -> EX_PS1, CSRRC -> EX_PS1,

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
    Mux(iDecodeEnable.asBool, iDecLSlenVal := 0.U(iDecLSlenValLen.W), iDecLSlenVal := Lookup(
        ioInternal.iInst, 0.U(2.W), Array(
            LB -> LS_B, LBU -> LS_B, SB -> LS_B,
            LH -> LS_H, LHU -> LS_H, SH -> LS_H,
            LW -> LS_W, LWU -> LS_W, SW -> LS_W,
            LD -> LS_D,              SD -> LS_D
        )
    ))

    // Decode LSfuncReg
    Mux(iDecodeEnable.asBool, iDecLSfuncVal := LS_NOP, iDecLSfuncVal := Lookup(
        ioInternal.iInst, LS_NOP, Array(
            LB  -> LS_LD ,  LH -> LS_LD ,  LW -> LS_LD ,  LD -> LS_LD ,
            SB  -> LS_ST ,  SH -> LS_ST ,  SW -> LS_ST ,  SD -> LS_ST ,
            LBU -> LS_LDU, LHU -> LS_LDU, LWU -> LS_LDU
        )
    ))

    // Decode WBTypReg
    Mux(iDecodeEnable.asBool, iDecWBTypVal := WB_NOP, iDecWBTypVal := Lookup(
        ioInternal.iInst, WB_EXU, Array(
            SB -> WB_NOP, SH -> WB_NOP, SW -> WB_NOP, SD -> WB_NOP,
            ECALL -> WB_NOP, EBREAK -> WB_NOP, MRET -> WB_NOP,
            BEQ -> WB_NOP, BNE -> WB_NOP, BGE -> WB_NOP, BGEU -> WB_NOP, BLT -> WB_NOP, BLTU -> WB_NOP,

            LB -> WB_LSU, LBU -> WB_LSU, LH -> WB_LSU, LHU -> WB_LSU, LW -> WB_LSU, LWU -> WB_LSU, LD -> WB_LSU,

            JAL -> WB_SNPC, JALR -> WB_SNPC
        )
    ))

    // Decode DSReg
    Mux(iDecodeEnable.asBool, iDecDSVal := 0.U(iDecDSValLen.W), iDecDSVal := Lookup(
        ioInternal.iInst, NPC_RUNNING, Array(
            EBREAK -> NPC_STOPPED
        )
    ))

    // Combine these decode results together when iDecodeEnable is true
    Mux(iDecodeEnable.asBool, DecodeVal := Cat(iDecPrivVal.asUInt, Cat(iDecEXUVal.asUInt, Cat(iDecLSlenVal.asUInt, Cat(iDecLSfuncVal.asUInt, Cat(iDecWBTypVal.asUInt, iDecDSVal.asUInt))))), 0.U(DecodeWidth.W))

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
    val ImmOut = 0.U(DataWidth.W)

    when(iDecodeEnable.asBool){
        ImmGenerator.ioSubmodule.iInst := ioInternal.iInst
        ImmGenerator.ioSubmodule.iType := InstructionType
        ImmOut := ImmGenerator.ioSubmodule.oImm
    }

    // Decode for registers
    val RS1Addr = 0.U(RegIDWidth.W)
    val RS2Addr = 0.U(RegIDWidth.W)
    val SRC1Val = 0.U(DataWidth.W)
    val SRC2Val = 0.U(DataWidth.W)

    val RDAddr = 0.U(RegIDWidth.W)

    when(iDecodeEnable.asBool){
        RS1Addr := ioInternal.iInst(RS1Hi, RS1Lo).asUInt
        ioInternal.oRS1 := RS1Addr
        SRC1Val := ioInternal.iSRC1

        RS2Addr := ioInternal.iInst(RS2Hi, RS2Lo).asUInt
        ioInternal.oRS2 := RS2Addr
        SRC2Val := ioInternal.iSRC2

        RDAddr := ioInternal.iInst(RDHi, RDLo).asUInt
    }

    // Decode Static-Next-PC and Dynamic-Next-PC
    val SNPC = 0.U(AddrWidth.W)
    val DNPC = 0.U(AddrWidth.W)

    when(iDecodeEnable.asBool){
        SNPC := ioInternal.iPC + InstSize.U
        DNPC := Lookup(
            ioInternal.iInst, ioInternal.iPC + InstSize.U, Array(
                JAL  -> ioInternal.iPC.asUInt + ImmOut.asUInt,
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
    }

    //ioInternal.oSNPC := SNPC
    ioInternal.oDNPC := DNPC

    val OldCSR = 0.U(DataWidth.W)

    when(iDecodeEnable.asBool){
        ioInternal.oCSR_ZicsrWSCIdx := Lookup(
            ioInternal.iInst, 0.U(CSRIDWidth.W), Array(
                CSRRW -> ImmOut,  CSRRS -> ImmOut,  CSRRC -> ImmOut,
                CSRRWI -> ImmOut, CSRRSI -> ImmOut, CSRRCI -> ImmOut,
            )
        )

        OldCSR := ioInternal.iCSR_ZicsrOldVal
        val Zicsr_uimm := ioInternal.iInst(RS1Hi, RS1Lo).asUInt

        ioInternal.oCSR_ZicsrNewVal := Lookup(
            ioInternal.iInst, 0.U(DataWidth.W), Array(
                CSRRW -> SRC1Val,
                CSRRS -> (SRC1Val | OldCSR),
                CSRRC -> (SRC1Val & OldCSR),
                CSRRWI -> Zicsr_uimm,
                CSRRSI -> (Zicsr_uimm | OldCSR),
                CSRRCI -> (Zicsr_uimm & OldCSR)
            )
        )
    }

    // Assign SRC to EXU and LSU
    val EXU_SRC1 = 0.U(DataWidth.W)
    val EXU_SRC2 = 0.U(DataWidth.W)
    val LSU_SRC2 = 0.U(DataWidth.W)

    when(iDecodeEnable.asBool){
        EXU_SRC1 := MuxCase(0.U(DataWidth.W), Array(
            InstructionType === InstR -> SRC1Val.asUInt,
            InstructionType === InstI -> SRC1Val.asUInt,
            InstructionType === InstS -> SRC1Val.asUInt,
            InstructionType === InstB -> 0.U(DataWidth.W),
            InstructionType === InstU -> ImmOut.asUInt,
            InstructionType === InstJ -> 0.U(DataWidth.W)
        ))

        EXU_SRC2 := MuxCase(0.U(DataWidth.W), Array(
            InstructionType === InstR -> SRC2Val.asUInt,
            InstructionType === InstI -> ImmOut.asUInt,
            InstructionType === InstS -> ImmOut.asUInt,
            InstructionType === InstB -> 0.U(DataWidth.W),
            InstructionType === InstU -> ioInternal.iPC.asUInt,
            InstructionType === InstJ -> 0.U(DataWidth.W)
        ))

        LSU_SRC2 := MuxCase(0.U(DataWidth.W), Array(
            InstructionType === InstR -> 0.U(DataWidth.W),
            InstructionType === InstI -> 0.U(DataWidth.W),
            InstructionType === InstS -> SRC2Val.asUInt,
            InstructionType === InstB -> 0.U(DataWidth.W),
            InstructionType === InstU -> 0.U(DataWidth.W),
            InstructionType === InstJ -> 0.U(DataWidth.W)
        ))
    }

    // Disable instruction decoding
    iDecodeEnable := false.B

    // Connect Decode Signals
    ioInternal.oDecodeBundle := DecodeVal
    ioInternal.oEXU_src1 := EXU_SRC1
    ioInternal.oEXU_src2 := EXU_SRC2
    ioInternal.oLSU_src2 := LSU_SRC2
    ioInternal.oRD := RDAddr

    // Connect Pipline Signals
    ioInternal.oMasterValid := true.B
    ioInternal.oSlaveReady := (IDU_NotBusy.asBool && ioInternal.iMasterReady)

    when(ioInternal.iMasterReady.asBool){
        // Shake hand success, re-enable iDecode, decode next instruction if IFU have result
        iDecodeEnable := true.B
    }
}