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

package npc.axi

import chisel3._
import chisel3.util._

object AXISlaveAW extends Bundle{
    val oSlaveAWready = Output(Bool())
    val iSlaveAWvalid = Input(Bool())
    val iSlaveAWaddr = Input(UInt(32.W))
    val iSlaveAWid = Input(UInt(4.W))
    val iSlaveAWlen = Input(UInt(8.W))
    val iSlaveAWsize = Input(UInt(3.W))
    val iSlaveAWburst = Input(UInt(2.W))
}

object AXISlaveW extends Bundle{
    val oSlaveWready = Output(Bool())
    val iSlaveWvalid = Input(Bool())
    val iSlaveWdata = Input(UInt(64.W))
    val iSlaveWstrb = Input(UInt(8.W))
    val iSlaveWlast = Input(Bool())
}

object AXISlaveB extends Bundle{
    val iSlaveBready = Input(Bool())
    val oSlaveBvalid = Output(Bool())
    val oSlaveBresp = Output(UInt(2.W))
    val oSlaveBid = Output(UInt(4.W))
}

object AXISlaveAR extends Bundle{
    val oSlaveARready = Output(Bool())
    val iSlaveARvalid = Input(Bool())
    val iSlaveARaddr = Input(UInt(32.W))
    val iSlaveARid = Input(UInt(4.W))
    val iSlaveARlen = Input(UInt(8.W))
    val iSlaveARsize = Input(UInt(3.W))
    val iSlaveARburst = Input(UInt(2.W))
}

object AXISlaveR extends Bundle{
    val iSlaveRready = Input(Bool())
    val oSlaveRvalid = Output(Bool())
    val oSlaveRresp = Output(UInt(2.W))
    val oSlaveRdata = Output(UInt(64.W))
    val oSlaveRlast = Output(Bool())
    val oSlaveRid = Output(UInt(4.W))
}