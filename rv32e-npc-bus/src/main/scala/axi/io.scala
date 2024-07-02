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

package npc.axi.io

import chisel3._
import chisel3.util._

object Master{
    val AXIMasterAW = new Bundle{
        val iMasterAWready = Input(Bool())
        val oMasterAWvalid = Output(Bool())
        val oMasterAWaddr = Output(UInt(32.W))
        val oMasterAWid = Output(UInt(4.W))
        val oMasterAWlen = Output(UInt(8.W))
        val oMasterAWsize = Output(UInt(3.W))
        val oMasterAWburst = Output(UInt(2.W))
    }
}