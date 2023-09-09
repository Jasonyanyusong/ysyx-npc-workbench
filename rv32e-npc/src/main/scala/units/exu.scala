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

package npc.units.iExecute

import chisel3._
import chisel3.util._

import npc.helper.defs.Base._
import npc.helper.defs.PipeLineDefs._
import npc.helper.defs.PipeLine_Bundle._

import npc.helper.opcode.OpEXU._

class iExecuteInternal extends Bundle{
    // ON-PIPELINE VALUES
    val oSlaveReady = Output(Bool())
    val iSlaveValid = Input(Bool())
    
    val iMasterReady = Input(Bool())
    val oMasterValid = Output(Bool())

    val PipeLine_ID2EX_MsgBundle = Input(UInt(PipeLine_ID2EX_Width.W))
    val PipeLine_EX2LS_MsgBundle = Output(UInt(PipeLine_EX2LS_Width.W))
    val PipeLine_EX2LS_ChangeReg = Output(Bool())
}

class EXU extends Module{
    val ioInternal = IO(new iExecuteInternal)

    val EXU_NotBusy = RegInit(true.B)

    val ID2EX_Msg = ioInternal.PipeLine_ID2EX_MsgBundle.asTypeOf(PipeLine_ID2EX_Bundle)

    printf("[RTL : EXU] DecodeBundle = %d\n", ID2EX_Msg.DecodeVal)

    val iExecuteOPcode = ID2EX_Msg.DecodeVal(13, 7)

    val SRC1 = ID2EX_Msg.EXU_SRC1
    val SRC2 = ID2EX_Msg.EXU_SRC2

    val EXU_StateOK = (ioInternal.iSlaveValid.asBool && ioInternal.iMasterReady.asBool)

    val EXU_Compute_Result = Mux(EXU_StateOK, MuxCase(0.U(DataWidth.W), Array(
            (iExecuteOPcode === EX_NOP) -> (0.U(DataWidth.W)).asUInt,
            (iExecuteOPcode === EX_PS1) -> (SRC1).asUInt,
            (iExecuteOPcode === EX_ADD) -> (SRC1 + SRC2).asUInt,
            (iExecuteOPcode === EX_SLT) -> Mux(SRC1.asSInt < SRC2.asSInt, 1.U(DataWidth.W), 0.U(DataWidth.W)).asUInt,
            (iExecuteOPcode === EX_SLTU) -> Mux(SRC1.asUInt < SRC2.asUInt, 1.U(DataWidth.W), 0.U(DataWidth.W)).asUInt,
            (iExecuteOPcode === EX_XOR) -> (SRC1 ^ SRC2).asUInt,
            (iExecuteOPcode === EX_OR) -> (SRC1 | SRC2).asUInt,
            (iExecuteOPcode === EX_AND) -> (SRC1 & SRC2).asUInt,
            (iExecuteOPcode === EX_SLL) -> (SRC1.asUInt << SRC2(4, 0)).asUInt,
            (iExecuteOPcode === EX_SRL) -> (SRC1.asUInt >> SRC2(4, 0)).asUInt,
            (iExecuteOPcode === EX_SRA) -> (SRC1.asSInt >> SRC2(4, 0)).asUInt,
            (iExecuteOPcode === EX_SUB) -> (SRC1 - SRC2).asUInt,
        )), 0.U(DataWidth.W))

    val PrePare_PipeLine_EX2LS_Bundle = Mux(EXU_StateOK, Cat(Seq(
        ID2EX_Msg.Instr.asUInt, ID2EX_Msg.PC.asUInt, ID2EX_Msg.DNPC, ID2EX_Msg.DecodeVal.asUInt, ID2EX_Msg.RD.asUInt, EXU_Compute_Result(DataWidth - 1, 0).asUInt, ID2EX_Msg.LSU_SRC2.asUInt
    )), 0.U(PipeLine_EX2LS_Width.W))

    ioInternal.PipeLine_EX2LS_MsgBundle := PrePare_PipeLine_EX2LS_Bundle
    ioInternal.PipeLine_EX2LS_ChangeReg := EXU_StateOK && EXU_NotBusy

    ioInternal.oSlaveReady := (EXU_NotBusy.asBool && ioInternal.iMasterReady.asBool)
    ioInternal.oMasterValid := (EXU_NotBusy.asBool && ioInternal.iSlaveValid.asBool)
}