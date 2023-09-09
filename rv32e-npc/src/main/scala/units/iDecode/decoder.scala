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
import npc.helper.defs.PipeLineDefs._
import npc.helper.defs.PipeLine_Bundle._

import npc.helper.rv32e.Inst._
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
    
    // Structure of Decode Bundle, although not use directly, this is also important

    // Combine EXU, LSU, WBU 's decode information together
    // 1   1   1   1   1   1   0   0   0   0   0   0   0   0   0   0
    // 5   4   3   2   1   0   9   8   7   6   5   4   3   2   1   0
    // |---|   |-----------------------|   |---|   |---|   |---|   |
    // Priv               EXU              LSlen   LSfunc  WBTyp  Debug State (DS)

    val oFeedBackPCChanged = Output(Bool())
    val oFeedBackNewPCVal = Output(UInt(AddrWidth.W))

    val oFeedBackDecodingJumpInstr = Output(Bool())

    val oIsDecodingBranch = Output(Bool())
    val oBranchDecodeOK = Output(Bool())

    val PipeLine_IF2ID_MsgBundle = Input(UInt(PipeLine_IF2ID_Width.W))
    val PipeLine_ID2EX_MsgBundle = Output(UInt(PipeLine_ID2EX_Width.W))
    val PipeLine_ID2EX_ChangeReg = Output(Bool())

    // OFF-PIPELINE VALUES

    val oRS1 = Output(UInt(RegIDWidth.W))
    val oRS2 = Output(UInt(RegIDWidth.W))
    val oRD  = Output(UInt(RegIDWidth.W))

    val iSRC1 = Input(UInt(DataWidth.W))
    val iSRC2 = Input(UInt(DataWidth.W))

    val iSRC1Dirty = Input(Bool())
    val iSRC2Dirty = Input(Bool())

    val iCSR_ZicsrOldVal = Input(UInt(DataWidth.W))
    val oCSR_ZicsrNewVal = Output(UInt(DataWidth.W))
    val oCSR_ZicsrWSCIdx = Output(UInt(CSRIDWidth.W))

    val iCSR_mtvec = Input(UInt(DataWidth.W))
    val iCSR_mepc = Input(UInt(DataWidth.W))
}

class IDU extends Module{
    val ioInternal = IO(new iDecodeInternal)

    val IDU_Busy = RegInit(false.B)
    val PipeLine_IF2ID_MsgBuffer = RegInit(0.U(PipeLine_IF2ID_Width.W))

    val IDU_StateOK = (ioInternal.iSlaveValid.asBool && ioInternal.iMasterReady.asBool) || IDU_Busy

    PipeLine_IF2ID_MsgBuffer := Mux(IDU_Busy, PipeLine_IF2ID_MsgBuffer, ioInternal.PipeLine_IF2ID_MsgBundle)
    val IDU_ProcessMsg = Mux(IDU_Busy, PipeLine_IF2ID_MsgBuffer, ioInternal.PipeLine_IF2ID_MsgBundle).asTypeOf(PipeLine_IF2ID_Bundle)

    val PipeLine_Instr = IDU_ProcessMsg.Instr
    val PipeLine_PC = IDU_ProcessMsg.PC

    val DecodingJumpInstr = Lookup(
        PipeLine_Instr, false.B, Array(
            JAL -> true.B, JALR -> true.B,
            BEQ -> true.B, BNE -> true.B, BLT -> true.B, BGE -> true.B, BLTU -> true.B, BGEU -> true.B,
            ECALL -> true.B, MRET -> true.B
        )
    )

    val IDU_InstructionType = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, instR, Array(
                LUI -> instU, AUIPC -> instU,

                JAL -> instJ,

                JALR -> instI,
                LB -> instI, LH -> instI, LW -> instI, LBU -> instI, LHU -> instI,
                ADDI -> instI, SLTI -> instI, SLTIU -> instI, XORI -> instI, ORI -> instI, ANDI -> instI, SLLI -> instI, SRLI -> instI, SRAI -> instI,

                CSRRW -> instI, CSRRS -> instI, CSRRC -> instI, CSRRWI -> instI, CSRRSI -> instI, CSRRCI -> instI,

                ADD -> instR, SUB -> instR, SLL -> instR, SLT -> instR, SLTU -> instR, XOR -> instR, SRL -> instR, SRA -> instR, OR -> instR, AND -> instR,

                SB -> instS, SH -> instS, SW -> instS,

                BEQ -> instB, BNE -> instB, BLT -> instB, BGE -> instB, BLTU -> instB, BGEU -> instB,
            )
        ), instR
    )

    val RS1 = Mux(IDU_StateOK, Mux(IDU_InstructionType === instU || IDU_InstructionType === instJ, 0.U(RegIDWidth.W), IDU_ProcessMsg.Instr(RS1Hi, RS1Lo)), 0.U(RegIDWidth.W))
    val RS2 = Mux(IDU_StateOK, Mux(IDU_InstructionType === instI || IDU_InstructionType === instU || IDU_InstructionType === instJ, 0.U(RegIDWidth.W), IDU_ProcessMsg.Instr(RS2Hi, RS2Lo)), 0.U(RegIDWidth.W))
    val RD  = Mux(IDU_StateOK, Mux(IDU_InstructionType === instS || IDU_InstructionType === instB, 0.U(RegIDWidth.W), IDU_ProcessMsg.Instr(RDHi , RDLo )), 0.U(RegIDWidth.W))

    // Get RS1, RS2 and RD
    // In RV32-E we add 1 so that the first bit will not be included, and will not affect Zicsr's uimm
    //val RS1 = Mux(IDU_StateOK, IDU_ProcessMsg.Instr(RS1Hi, RS1Lo), 0.U(RegIDWidth.W))
    //val RS2 = Mux(IDU_StateOK, IDU_ProcessMsg.Instr(RS2Hi, RS2Lo), 0.U(RegIDWidth.W))
    //val RD  = Mux(IDU_StateOK, IDU_ProcessMsg.Instr(RDHi , RDLo ), 0.U(RegIDWidth.W))

    ioInternal.oRS1 := RS1 & "b01111".U
    ioInternal.oRS2 := RS2 & "b01111".U
    ioInternal.oRD := RD & "b01111".U

    val RSRegistersDirty = (ioInternal.iSRC1Dirty || ioInternal.iSRC2Dirty)

    ioInternal.oIsDecodingBranch := Lookup(
        PipeLine_Instr, false.B, Array(
            BEQ -> true.B, BNE -> true.B, BLT -> true.B, BGE -> true.B, BLTU -> true.B, BGEU -> true.B
        )
    ) && RSRegistersDirty

    ioInternal.oBranchDecodeOK := Lookup(
        PipeLine_Instr, false.B, Array(
            BEQ -> true.B, BNE -> true.B, BLT -> true.B, BGE -> true.B, BLTU -> true.B, BGEU -> true.B
        )
    ) && !RSRegistersDirty

    // judge IDU's Busy state
    IDU_Busy := RSRegistersDirty

    val IDU_SRC1 = Mux(IDU_StateOK, ioInternal.iSRC1, 0.U(DataWidth.W))
    val IDU_SRC2 = Mux(IDU_StateOK, ioInternal.iSRC2, 0.U(DataWidth.W))

    //val PipeLine_Instr = IDU_ProcessMsg.Instr
    //val PipeLine_PC = IDU_ProcessMsg.PC

    val Priv = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, PR_NORM, Array(
                ECALL -> PR_ECALL, MRET -> PR_MRET,
                CSRRW -> PR_ZICSR, CSRRC -> PR_ZICSR, CSRRS -> PR_ZICSR,
                CSRRWI -> PR_ZICSR, CSRRCI -> PR_ZICSR, CSRRSI -> PR_ZICSR
            )
        ), 0.U(iDecPrivValLen.W)
    )

    val EXU_OP = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, EX_NOP, Array(
                LUI -> EX_PS1, CSRRW -> EX_PS1, CSRRS -> EX_PS1, CSRRC -> EX_PS1, CSRRWI -> EX_PS1, CSRRCI -> EX_PS1, CSRRSI -> EX_PS1,

                AUIPC -> EX_ADD, SB -> EX_ADD, SH -> EX_ADD, SW -> EX_ADD, ADD -> EX_ADD, ADDI -> EX_ADD,
                LB -> EX_ADD, LBU -> EX_ADD, LH -> EX_ADD, LHU -> EX_ADD, LW -> EX_ADD,

                SUB -> EX_SUB,

                AND -> EX_AND, ANDI -> EX_AND, OR -> EX_OR, ORI -> EX_OR, XOR -> EX_XOR, XORI -> EX_XOR,

                SLT -> EX_SLT, SLTI -> EX_SLT, SLTU -> EX_SLTU, SLTIU -> EX_SLTU,

                SLL -> EX_SLL, SLLI -> EX_SLL, SRL -> EX_SRL, SRLI -> EX_SRL, SRA -> EX_SRA, SRAI -> EX_SRA,

                //JAL -> EX_NOP, JALR -> EX_NOP, BEQ -> EX_NOP, BNE -> EX_NOP, BLT -> EX_NOP, BLTU -> EX_NOP, BGE -> EX_NOP, BGEU -> EX_NOP,
            )
        ), 0.U(iDecEXUValLen.W)
    )

    val LSU_OP_LEN = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, 0.U(2.W), Array(
                LB -> LS_B, LBU -> LS_B, SB -> LS_B,
                LH -> LS_H, LHU -> LS_H, SH -> LS_H,
                LW -> LS_W,              SW -> LS_W,
            )
        ), 0.U(iDecLSlenValLen.W)
    )

    val LSU_OP_FUNC = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, LS_NOP, Array(
                LB  -> LS_LD ,  LH -> LS_LD ,  LW -> LS_LD ,
                SB  -> LS_ST ,  SH -> LS_ST ,  SW -> LS_ST ,
                LBU -> LS_LDU, LHU -> LS_LDU, 
            )
        ), 0.U(iDecLSfuncValLen.W)
    )

    printf("[RTL : IDU] LSU_OP_FUNC = %d\n", LSU_OP_FUNC)

    val WBU_OP = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, WB_EXU, Array(
                SB -> WB_NOP, SH -> WB_NOP, SW -> WB_NOP,
                ECALL -> WB_NOP, EBREAK -> WB_NOP, MRET -> WB_NOP,
                BEQ -> WB_NOP, BNE -> WB_NOP, BGE -> WB_NOP, BGEU -> WB_NOP, BLT -> WB_NOP, BLTU -> WB_NOP,

                LB -> WB_LSU, LBU -> WB_LSU, LH -> WB_LSU, LHU -> WB_LSU, LW -> WB_LSU,

                JAL -> WB_SNPC, JALR -> WB_SNPC
            )
        ), 0.U(iDecWBTypValLen.W)
    )

    val DEBUG_STATE = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, NPC_RUNNING, Array(
                EBREAK -> NPC_STOPPED
            )
        ), 0.U(iDecDSValLen.W)
    )

    val IDU_DecodeBundle = Mux(IDU_StateOK, Cat(Seq(
            Priv, EXU_OP, LSU_OP_LEN, LSU_OP_FUNC, WBU_OP, DEBUG_STATE
        )), 0.U(DecodeWidth.W)
    )

    printf("[RTL : IDU] DecodeBundle = %d\n", IDU_DecodeBundle)

    /*val IDU_InstructionType = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, instR, Array(
                LUI -> instU, AUIPC -> instU,

                JAL -> instJ,

                JALR -> instI,
                LB -> instI, LH -> instI, LW -> instI, LBU -> instI, LHU -> instI,
                ADDI -> instI, SLTI -> instI, SLTIU -> instI, XORI -> instI, ORI -> instI, ANDI -> instI, SLLI -> instI, SRLI -> instI, SRAI -> instI,

                CSRRW -> instI, CSRRS -> instI, CSRRC -> instI, CSRRWI -> instI, CSRRSI -> instI, CSRRCI -> instI,

                ADD -> instR, SUB -> instR, SLL -> instR, SLT -> instR, SLTU -> instR, XOR -> instR, SRL -> instR, SRA -> instR, OR -> instR, AND -> instR,

                SB -> instS, SH -> instS, SW -> instS,

                BEQ -> instB, BNE -> instB, BLT -> instB, BGE -> instB, BLTU -> instB, BGEU -> instB,
            )
        ), instR
    )*/

    // Connect imm-generator
    val ImmGenerator = Module(new immGen)
    ImmGenerator.ioSubmodule.iInst := PipeLine_Instr
    ImmGenerator.ioSubmodule.iType := IDU_InstructionType
    val IDU_Imm = Mux(IDU_StateOK, ImmGenerator.ioSubmodule.oImm, 0.U(DataWidth.W))

    val IDU_SNPC = Mux(IDU_StateOK, PipeLine_PC + InstSize.U, 0.U(AddrWidth.W))
    val IDU_DNPC = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, PipeLine_PC + InstSize.U, Array(
                JAL  -> (PipeLine_PC.asUInt + IDU_Imm.asUInt),
                JALR -> ((IDU_SRC1.asUInt + IDU_Imm.asUInt) & Cat(Fill(AddrWidth - 1, 1.U(1.W)), Fill(1, 0.U(1.W)))),

                BEQ  -> Mux(IDU_SRC1.asUInt === IDU_SRC2.asUInt, PipeLine_PC.asUInt + IDU_Imm.asUInt, PipeLine_PC.asUInt + InstSize.U),
                BNE  -> Mux(IDU_SRC1.asUInt =/= IDU_SRC2.asUInt, PipeLine_PC.asUInt + IDU_Imm.asUInt, PipeLine_PC.asUInt + InstSize.U),
                BLT  -> Mux(IDU_SRC1.asSInt  <  IDU_SRC2.asSInt, PipeLine_PC.asUInt + IDU_Imm.asUInt, PipeLine_PC.asUInt + InstSize.U),
                BGE  -> Mux(IDU_SRC1.asSInt  >= IDU_SRC2.asSInt, PipeLine_PC.asUInt + IDU_Imm.asUInt, PipeLine_PC.asUInt + InstSize.U),
                BLTU -> Mux(IDU_SRC1.asUInt  <  IDU_SRC2.asUInt, PipeLine_PC.asUInt + IDU_Imm.asUInt, PipeLine_PC.asUInt + InstSize.U),
                BGEU -> Mux(IDU_SRC1.asUInt  >= IDU_SRC2.asUInt, PipeLine_PC.asUInt + IDU_Imm.asUInt, PipeLine_PC.asUInt + InstSize.U),

                ECALL -> ioInternal.iCSR_mtvec.asUInt,
                EBREAK -> PipeLine_PC,
                MRET  -> ioInternal.iCSR_mepc.asUInt
            )
        ), 0.U(AddrWidth.W)
    )

    val IDU_JudgePCJump = Mux(IDU_StateOK, Mux(IDU_SNPC === IDU_DNPC, false.B, true.B), false.B)
    val IDU_JumpPCAddr  = Mux(IDU_StateOK, IDU_DNPC, 0.U(AddrWidth.W))

    val IDU_ZicsrWSCIdx = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, 0.U(CSRIDWidth.W), Array(
                CSRRW -> IDU_Imm,  CSRRS -> IDU_Imm,  CSRRC -> IDU_Imm,
                CSRRWI -> IDU_Imm, CSRRSI -> IDU_Imm, CSRRCI -> IDU_Imm,
            )
        ), 0.U(CSRIDWidth.W)
    )

    val IDU_OldCSR = Mux(IDU_StateOK, ioInternal.iCSR_ZicsrOldVal, 0.U(DataWidth.W))
    val IDU_Zicsr_uimm = Mux(IDU_StateOK, PipeLine_Instr(RS1Hi, RS1Lo), 0.U(5.W))

    val IDU_ZicsrNewVal = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, 0.U(DataWidth.W), Array(
                CSRRW -> IDU_SRC1,
                CSRRS -> (IDU_SRC1 | IDU_OldCSR),
                CSRRC -> (IDU_SRC1 & IDU_OldCSR),
                CSRRWI -> IDU_Zicsr_uimm,
                CSRRSI -> (IDU_Zicsr_uimm | IDU_OldCSR),
                CSRRCI -> (IDU_Zicsr_uimm & IDU_OldCSR)
            )
        ), 0.U(DataWidth.W)
    )

    val IDU_EXU_SRC1 = Mux(IDU_StateOK, MuxCase(0.U(DataWidth.W), Array(
        (IDU_InstructionType === instR) -> IDU_SRC1.asUInt,
        (IDU_InstructionType === instI) -> Lookup(PipeLine_Instr, IDU_SRC1.asUInt, Array(
            CSRRW -> IDU_OldCSR, CSRRS -> IDU_OldCSR, CSRRC -> IDU_OldCSR,
            CSRRWI -> IDU_OldCSR, CSRRSI -> IDU_OldCSR, CSRRCI -> IDU_OldCSR
        )),
        (IDU_InstructionType === instS) -> IDU_SRC1.asUInt,
        (IDU_InstructionType === instB) -> 0.U(DataWidth.W),
        (IDU_InstructionType === instU) -> IDU_Imm.asUInt,
        (IDU_InstructionType === instJ) -> 0.U(DataWidth.W)
    )), 0.U(DataWidth.W))

    val IDU_EXU_SRC2 = Mux(IDU_StateOK, MuxCase(0.U(DataWidth.W), Array(
        (IDU_InstructionType === instR) -> IDU_SRC2.asUInt,
        (IDU_InstructionType === instI) -> IDU_Imm.asUInt,
        (IDU_InstructionType === instS) -> IDU_Imm.asUInt,
        (IDU_InstructionType === instB) -> 0.U(DataWidth.W),
        (IDU_InstructionType === instU) -> PipeLine_PC.asUInt,
        (IDU_InstructionType === instJ) -> 0.U(DataWidth.W)
    )), 0.U(DataWidth.W))

    val IDU_LSU_SRC2 = Mux(IDU_StateOK, MuxCase(0.U(DataWidth.W), Array(
        (IDU_InstructionType === instR) -> 0.U(DataWidth.W),
        (IDU_InstructionType === instI) -> 0.U(DataWidth.W),
        (IDU_InstructionType === instS) -> IDU_SRC2.asUInt,
        (IDU_InstructionType === instB) -> 0.U(DataWidth.W),
        (IDU_InstructionType === instU) -> 0.U(DataWidth.W),
        (IDU_InstructionType === instJ) -> 0.U(DataWidth.W)
    )), 0.U(DataWidth.W))

    val PrePare_PipeLine_ID2EX_Bundle = Mux(IDU_StateOK, Cat(Seq(PipeLine_Instr, PipeLine_PC, IDU_DNPC, IDU_DecodeBundle, RD, IDU_EXU_SRC1, IDU_EXU_SRC2, IDU_LSU_SRC2)), 0.U(PipeLine_ID2EX_Width.W))
    ioInternal.PipeLine_ID2EX_MsgBundle := PrePare_PipeLine_ID2EX_Bundle
    ioInternal.PipeLine_ID2EX_ChangeReg := (IDU_StateOK && (!RSRegistersDirty))

    ioInternal.oFeedBackPCChanged := IDU_JudgePCJump && (!RSRegistersDirty)
    ioInternal.oFeedBackNewPCVal := IDU_JumpPCAddr

    ioInternal.oFeedBackDecodingJumpInstr := (DecodingJumpInstr && IDU_Busy)

    ioInternal.oCSR_ZicsrWSCIdx := IDU_ZicsrWSCIdx
    ioInternal.oCSR_ZicsrNewVal := IDU_ZicsrNewVal

    // Connect Pipline Signals
    ioInternal.oMasterValid := ((!RSRegistersDirty) && ioInternal.iSlaveValid) || ((DecodingJumpInstr && IDU_Busy && (!RSRegistersDirty)))
    ioInternal.oSlaveReady := ((!RSRegistersDirty) && ioInternal.iMasterReady) && !IDU_JudgePCJump
}