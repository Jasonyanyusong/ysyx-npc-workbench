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

package npc.units

import chisel3._
import chisel3.util._

import npc.helper.defs.Base._
import npc.helper.opcode.OpEXU._

object iExecuteInternal extends Bundle{
    // ON-PIPELINE VALUES
    val oSlaveReady = Output(Bool())
    val iSlaveValid = Input(Bool())
    
    val iMasterReady = Input(Bool())
    val oMasterValid = Output(Bool())

    val iDecodeBundle = Input(UInt(DecodeWidth.W))
    val oDecodeBundle = Output(UInt(DecodeWidth.W))

    val iEXU_SRC1 = Input(UInt(DataWidth.W))
    val iEXU_SRC2 = Input(UInt(DataWidth.W))
    val iLSU_SRC2 = Input(UInt(DataWidth.W))
    val oLSU_SRC2 = Output(UInt(DataWidth.W))

    val oEXU_RET = Output(UInt(DataWidth.W))
    val iRD = Input(UInt(RegIDWidth.W))
    val oRD = Output(UInt(RegIDWidth.W))
}

class EXU extends Module{
    val ioInternal = IO(new iExecuteInternal)

    val EXU_NotBusy = RegInit(true.B)

    // TODO: Add EXU's functional
    val ExecuteResult = 0.U(DataWidth.W)
    val iExecuteEnable = true.B

    // Only can execute arthmetic if Master (IDU) 's output is valid
    Mux(ioInternal.iSlaveValid.asBool, iExecuteEnable := true.B, iExecuteEnable := false.B)

    when(iExecuteEnable.asBool){
        ExecuteResult := MuxCase(0.U(DataWidth.W), Array(
            // TODO: Add more EXU operation types
        ))
    }

    // TODO: Connect Passthroughs for LSU and WBU
    ioInternal.oDecodeBundle := ioInternal.iDecodeBundle
    ioInternal.oLSU_SRC2 := ioInternal.iLSU_SRC2
    ioInternal.oRD := ioInternal.iRD

    // TODO: Pipeline Shake-Hand-Rule implement
}