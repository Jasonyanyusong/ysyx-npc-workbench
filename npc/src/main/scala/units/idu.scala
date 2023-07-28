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
import npc.helper.rv64im.Inst._
import npc.helper.opcode.OpEXU._
import npc.helper.opcode.OpLSU._
import npc.helper.opcode.OpWBU._

object iDecodeInternal extends Bundle{

    // ON-PIPELINE VALUES
    val oSlaveReady = Input(Bool())
    val iSlaveValid = Output(Bool())

    val iMasterReady = Input(Bool())
    val oMasterValid = Output(Bool())

    val iInst = Input(UInt(InstWidth.W))
    val oDynamicNextPC = Output(UInt(AddrWidth.W))
    val oStaticNextPC = Output(UInt(AddrWidth.W))
    
    // Combine EXU, LSU, WBU 's decode information together
    // 1   1   1   1   1   1   0   0   0   0   0   0   0   0   0   0
    // 5   4   3   2   1   0   9   8   7   6   5   4   3   2   1   0
    // |---|   |-----------------------|   |---|   |---|   |---|   |
    // Priv               EXU              LSlen   LSfunc  WBTyp  Debug State
    val oDecodeBundle = Output(UInt(16.W))

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
}

class IDU extends Module{
    ioInternal = IO(new iDecodeInternal)
    // TODO: Write IDU Logic
}