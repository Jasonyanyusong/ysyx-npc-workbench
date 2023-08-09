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
import npc.helper.opcode.OpWBU._

class iWriteBackInternal extends Bundle{
    // ON-PIPELEINE VALUES
    val oSlaveReady = Output(Bool())
    val iSlaveValid = Input(Bool())

    val iDecodeBundle = Input(UInt(DecodeWidth.W))

    val iEXU_RET = Input(UInt(DataWidth.W))
    val iLSU_RET = Input(UInt(DataWidth.W))

    val iRD = Input(UInt(RegIDWidth.W))
    val iPC = Input(UInt(AddrWidth.W))

    val oPC = Output(UInt(AddrWidth.W))

    // OFF-PIPELINE VALUES
    val oWriteGPREnable = Output(Bool())
    val oWriteGPRAddr = Output(UInt(RegIDWidth.W))
    val oWriteGPRVal = Output(UInt(DataWidth.W))
}

class WBU extends Module{
    val ioInternal = IO(new iWriteBackInternal)

    val WBU_NotBusy = RegInit(true.B)
    val iWriteBackEnable = RegInit(true.B)

    val WriteGPREnable = RegInit(false.B)
    val WriteGPRAddr = RegInit(0.U(RegIDWidth.W))
    val WriteGPRVal = RegInit(0.U(DataWidth.W))

    val EX_RETVal = ioInternal.iEXU_RET
    val LS_RETVal = ioInternal.iLSU_RET
    val SNPC = ioInternal.iPC + 4.U

    val DecodeBundle = ioInternal.iDecodeBundle
    val WBDecode = DecodeBundle(2, 1)

    iWriteBackEnable := Mux(ioInternal.iSlaveValid.asBool, true.B, false.B)

    WriteGPRAddr := Mux(iWriteBackEnable.asBool, ioInternal.iRD, WriteGPRAddr)

    WriteGPREnable := Mux(iWriteBackEnable.asBool, MuxCase(false.B, Array(
        (WBDecode === WB_NOP) -> false.B,
        (WBDecode === WB_EXU) -> true.B,
        (WBDecode === WB_LSU) -> true.B,
        (WBDecode === WB_SNPC) -> true.B
    )), WriteGPREnable)

    WriteGPRVal := Mux(iWriteBackEnable.asBool, MuxCase(0.U(DataWidth.W), Array(
        (WBDecode === WB_NOP) -> 0.U(DataWidth.W),
        (WBDecode === WB_EXU) -> EX_RETVal,
        (WBDecode === WB_LSU) -> LS_RETVal,
        (WBDecode === WB_SNPC) -> SNPC
    )), WriteGPRVal)

    // Connect IO Internal
    ioInternal.oWriteGPREnable := WriteGPREnable
    ioInternal.oWriteGPRAddr := WriteGPRAddr
    ioInternal.oWriteGPRVal := WriteGPRVal
    ioInternal.oPC := ioInternal.iPC

    // Pipeline shake hand implementation
    ioInternal.oSlaveReady := WBU_NotBusy.asBool
}