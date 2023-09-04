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

package npc.axi.sram

import chisel2._
import chisel3.uitl._

import npc.axi.master
import npc.axi.slave

import npc.helper.defs.Base._

class SRAM_External_IO extends Bundle{
    val SRAM_Addr = Output(UInt(AddrWidth.W))
    val SRAM_R_Data = Input(UInt(AddrWidth.W))
    val SRAM_W_Data = Output(UInt(AddrWidth.W))
    val SRAM_R_Enable = Output(Bool())
    val SRAM_W_Enable = Output(Bool())
    val SRAM_Host_Valid = Input(Bool())
}

class SRAM extends Module{
    // Internal IO: connect with Master at arbiter
    val Slave_AW = IO(new AXISlaveAW)
    val Slave_W = IO(new AXISlaveW)
    val Slave_B = IO(new AXISlaveB)
    val Slave_AR = IO(new AXISlaveAR)
    val Slave_R = IO(new AXISlaveR)

    // External IO: connect with NPC's IO for verilating
    val ExternalIO = IO(new SRAM_External_IO)

    // TODO: Logic for Memory manipulations
}