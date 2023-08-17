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
    
    // Structure of Decode Bundle, although not use directly, this is also important

    // Combine EXU, LSU, WBU 's decode information together
    // 1   1   1   1   1   1   0   0   0   0   0   0   0   0   0   0
    // 5   4   3   2   1   0   9   8   7   6   5   4   3   2   1   0
    // |---|   |-----------------------|   |---|   |---|   |---|   |
    // Priv               EXU              LSlen   LSfunc  WBTyp  Debug State (DS)

    val oFeedBackPCChanged = Output(Bool())
    val oFeedBackNewPCVal = Output(UInt(AddrWidth.W))

    //val iHaveWriteBack = Input(Bool())
    //val iWriteBackAddr = Input(UInt(RegIDWidth.W))

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

    //val PipeLine_Instr = ioInternal.PipeLine_IF2ID_MsgBundle(95, 64)
    //val PipeLine_PC = ioInternal.PipeLine_IF2ID_MsgBundle(63, 0)

    // TODO: solve the bug of decoding the next inst when IDU hazard blocked

    val PipeLine_ID2ID_Bundle = new Bundle{
        val Instr = UInt(InstWidth.W)
        val PC = UInt(AddrWidth.W)
    }

    val IF2ID_Msg = ioInternal.PipeLine_IF2ID_MsgBundle.asTypeOf(PipeLine_ID2ID_Bundle)

    val PipeLine_Instr = IF2ID_Msg.Instr
    val PipeLine_PC = IF2ID_Msg.PC

    val IDU_NotBusy = RegInit(true.B)
    //val RegStateTable = Mem(RegSum, Bool())

    val IDU_StateOK = (ioInternal.iSlaveValid.asBool && ioInternal.iMasterReady.asBool)

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
        ), 0.U(iDecEXUValLen.W)
    )

    val LSU_OP_LEN = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, 0.U(2.W), Array(
                LB -> LS_B, LBU -> LS_B, SB -> LS_B,
                LH -> LS_H, LHU -> LS_H, SH -> LS_H,
                LW -> LS_W, LWU -> LS_W, SW -> LS_W,
                LD -> LS_D,              SD -> LS_D
            )
        ), 0.U(iDecLSlenValLen.W)
    )

    val LSU_OP_FUNC = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, LS_NOP, Array(
                LB  -> LS_LD ,  LH -> LS_LD ,  LW -> LS_LD ,  LD -> LS_LD ,
                SB  -> LS_ST ,  SH -> LS_ST ,  SW -> LS_ST ,  SD -> LS_ST ,
                LBU -> LS_LDU, LHU -> LS_LDU, LWU -> LS_LDU
            )
        ), 0.U(iDecLSfuncValLen.W)
    )

    val WBU_OP = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, WB_EXU, Array(
                SB -> WB_NOP, SH -> WB_NOP, SW -> WB_NOP, SD -> WB_NOP,
                ECALL -> WB_NOP, EBREAK -> WB_NOP, MRET -> WB_NOP,
                BEQ -> WB_NOP, BNE -> WB_NOP, BGE -> WB_NOP, BGEU -> WB_NOP, BLT -> WB_NOP, BLTU -> WB_NOP,

                LB -> WB_LSU, LBU -> WB_LSU, LH -> WB_LSU, LHU -> WB_LSU, LW -> WB_LSU, LWU -> WB_LSU, LD -> WB_LSU,

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

    val IDU_InstructionType = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, instR, Array(
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
        ), instR
    )

    // Connect imm-generator
    val ImmGenerator = Module(new immGen)
    ImmGenerator.ioSubmodule.iInst := PipeLine_Instr
    ImmGenerator.ioSubmodule.iType := IDU_InstructionType
    val IDU_Imm = Mux(IDU_StateOK, ImmGenerator.ioSubmodule.oImm, 0.U(DataWidth.W))

    val IDU_RS1 = Mux(IDU_StateOK, PipeLine_Instr(RS1Hi, RS1Lo).asUInt, 0.U(RegIDWidth.W))
    val IDU_RS2 = Mux(IDU_StateOK, PipeLine_Instr(RS2Hi, RS2Lo).asUInt, 0.U(RegIDWidth.W))
    val IDU_RD  = Mux(IDU_StateOK, PipeLine_Instr(RDHi, RDLo).asUInt, 0.U(RegIDWidth.W))

    val IDU_SRC1 = Mux(IDU_StateOK, ioInternal.iSRC1, 0.U(DataWidth.W))
    val IDU_SRC2 = Mux(IDU_StateOK, ioInternal.iSRC2, 0.U(DataWidth.W))

    val IDU_SRC1_Dirty = Mux(IDU_StateOK, ioInternal.iSRC1Dirty, false.B)
    val IDU_SRC2_Dirty = Mux(IDU_StateOK, ioInternal.iSRC2Dirty, false.B)

    /*RegStateTable(IDU_RD.asUInt) := Mux(IDU_RD.asUInt === 0.U, 0.U(1.W), 1.U(1.W))
    RegStateTable(ioInternal.iWriteBackAddr.asUInt) := Mux(ioInternal.iHaveWriteBack.asBool, false.B, RegStateTable(ioInternal.iWriteBackAddr.asUInt))
    IDU_NotBusy := ((!RegStateTable(IDU_RS1.asUInt).asBool) && (!RegStateTable(IDU_RS2.asUInt).asBool))*/

    IDU_NotBusy := ((!IDU_SRC1_Dirty) && (!IDU_SRC2_Dirty))

    /*printf("[RTL] Get WBU's Did RD = %d, RD ADDR = %d\n", ioInternal.iHaveWriteBack, ioInternal.iWriteBackAddr)
    printf("[RTL] New RD is %d, Check Register State: RS1 = %d, RS2 = %d, RS1 state: %d, RS2 state: %d\n",IDU_RD, IDU_RS1, IDU_RS2, RegStateTable(IDU_RS1.asUInt), RegStateTable(IDU_RS2.asUInt))
    printf("[RTL] Register state, 1 is for not ready (dirty) ")
    for(i <- 0 to 31){
        printf("%d", RegStateTable(i))
    }
    printf("\n")
    printf("[RTL] IDU_NotBusy = %d\n", IDU_NotBusy)*/

    val IDU_SNPC = Mux(IDU_StateOK, PipeLine_PC + InstSize.U, 0.U(AddrWidth.W))
    val IDU_DNPC = Mux(IDU_StateOK, Lookup(
            PipeLine_Instr, PipeLine_PC + InstSize.U, Array(
                JAL  -> (PipeLine_PC.asUInt + IDU_Imm.asUInt),
                JALR -> ((IDU_SRC1.asUInt + IDU_Imm.asUInt) & Cat(Fill(63, 1.U(1.W)), Fill(1, 0.U(1.W)))),

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



    val PrePare_PipeLine_ID2EX_Bundle = Mux(IDU_StateOK, Cat(Seq(PipeLine_Instr, PipeLine_PC, IDU_DecodeBundle, IDU_RD, IDU_EXU_SRC1, IDU_EXU_SRC2, IDU_LSU_SRC2)), 0.U(PipeLine_ID2EX_Width.W))
    ioInternal.PipeLine_ID2EX_MsgBundle := PrePare_PipeLine_ID2EX_Bundle
    ioInternal.PipeLine_ID2EX_ChangeReg := (IDU_StateOK && IDU_NotBusy)
     
    ioInternal.oRS1 := IDU_RS1
    ioInternal.oRS2 := IDU_RS2
    ioInternal.oRD := IDU_RD

    ioInternal.oFeedBackPCChanged := IDU_JudgePCJump && ((!IDU_SRC1_Dirty) && (!IDU_SRC2_Dirty))
    ioInternal.oFeedBackNewPCVal := IDU_JumpPCAddr

    ioInternal.oCSR_ZicsrWSCIdx := IDU_ZicsrWSCIdx
    ioInternal.oCSR_ZicsrNewVal := IDU_ZicsrNewVal

    // Connect Pipline Signals
    ioInternal.oMasterValid := (IDU_NotBusy.asBool && ioInternal.iSlaveValid) && ((!IDU_SRC1_Dirty) && (!IDU_SRC2_Dirty))
    ioInternal.oSlaveReady := (IDU_NotBusy.asBool && ioInternal.iMasterReady) && ((!IDU_SRC1_Dirty) && (!IDU_SRC2_Dirty))
}