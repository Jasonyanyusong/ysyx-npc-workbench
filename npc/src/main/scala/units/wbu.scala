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
import npc.helper.opcode.OpWBU._

object iWriteBackInternal extends Bundle{
    // ON-PIPELEINE VALUES
    val oSlaveReady = Output(Bool())
    val iSlaveValid = Input(Bool())

    val iDecodeBundle = Input(UInt(DecodeWidth.W))

    val iEXU_RET = Input(UInt(DataWidth.W))
    val iLSU_RET = Input(UInt(DataWidth.W))

    val iRD = Input(UInt(RegIDWidth.W))

    // OFF-PIPELINE VALUES
    val oWriteGPREnable = Output(Bool())
    val oWriteGPRAddr = Output(UInt(RegIDWidth.W))
    val oWriteGPRVal = Output(UInt(DataWidth.W))
}