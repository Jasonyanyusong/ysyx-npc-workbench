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

package npc.units.iWriteBack

import chisel3._
import chisel3.util._

import npc.helper.defs.Base._
import npc.helper.defs.PipeLineDefs._

import npc.helper.opcode.OpWBU._

class iWriteBackInternal extends Bundle{
    // ON-PIPELEINE VALUES
    val oSlaveReady = Output(Bool())
    val iSlaveValid = Input(Bool())

    val PipeLine_LS2WB_MsgBundle = Input(UInt(PipeLine_LS2WB_Width.W))

    val oPC = Output(UInt(AddrWidth.W))

    val oWorked = Output(Bool()) // Debug signal, used for difftest
    val oStopped = Output(Bool())
    val oDecodeBundleDebug = Output(UInt(DecodeWidth.W))

    // OFF-PIPELINE VALUES
    val oWriteGPREnable = Output(Bool())
    val oWriteGPRAddr = Output(UInt(RegIDWidth.W))
    val oWriteGPRVal = Output(UInt(DataWidth.W))
}

class WBU extends Module{
    val ioInternal = IO(new iWriteBackInternal)

    //val WBU_NotBusy = RegInit(true.B)

    val PipeLine_LS2WB_Bundle = new Bundle{
        val Instr = UInt(InstWidth.W)
        val PC = UInt(AddrWidth.W)
        val DecodeVal = UInt(DecodeWidth.W)
        val RD = UInt(RegIDWidth.W)
        val EX_RET = UInt(DataWidth.W)
        val LS_RET = UInt(DataWidth.W)
    }

    val LS2WB_Msg = ioInternal.PipeLine_LS2WB_MsgBundle.asTypeOf(PipeLine_LS2WB_Bundle)

    val EX_RETVal = LS2WB_Msg.EX_RET
    val LS_RETVal = LS2WB_Msg.LS_RET
    val SNPC = LS2WB_Msg.PC + 4.U

    val DecodeBundle = LS2WB_Msg.DecodeVal
    val WBDecode = DecodeBundle(2, 1)

    val WBU_StateOK = (ioInternal.iSlaveValid.asBool)

    val WBU_GPR_ADDR = Mux(WBU_StateOK, LS2WB_Msg.RD, 0.U(RegIDWidth.W))
    val WBU_GPR_WRITE_ENABLE = Mux(WBU_StateOK, MuxCase(false.B, Array(
            (WBDecode === WB_NOP) -> false.B,
            (WBDecode === WB_EXU) -> true.B,
            (WBDecode === WB_LSU) -> true.B,
            (WBDecode === WB_SNPC) -> true.B
        )), false.B
    )
    val WBU_GPR_WRITE_DATA = Mux(WBU_StateOK, MuxCase(0.U(DataWidth.W), Array(
            (WBDecode === WB_NOP) -> 0.U(DataWidth.W),
            (WBDecode === WB_EXU) -> EX_RETVal,
            (WBDecode === WB_LSU) -> LS_RETVal,
            (WBDecode === WB_SNPC) -> SNPC
        )), 0.U(DataWidth.W)
    )

    // Connect IO Internal
    ioInternal.oWriteGPREnable := WBU_GPR_WRITE_ENABLE
    ioInternal.oWriteGPRAddr := WBU_GPR_ADDR
    ioInternal.oWriteGPRVal := WBU_GPR_WRITE_DATA
    ioInternal.oPC := LS2WB_Msg.PC

    // Connect IO for debug
    ioInternal.oWorked := WBU_StateOK
    ioInternal.oDecodeBundleDebug := LS2WB_Msg.DecodeVal
    ioInternal.oStopped := (LS2WB_Msg.Instr === "h100073".U)

    // Pipeline shake hand implementation
    ioInternal.oSlaveReady := true.B
}