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

import chisel3._
import chisel3.util._

import npc.axi.master
import npc.axi.slave
import npc.axi.params.Base._
import npc.axi.params.AXI2Lable._

import npc.axi.slave.AXISlaveAW
import npc.axi.slave.AXISlaveW
import npc.axi.slave.AXISlaveB
import npc.axi.slave.AXISlaveAR
import npc.axi.slave.AXISlaveR

import npc.helper.defs.Base._

class SRAM_External_IO extends Bundle{
    val SRAM_R_Addr = Output(UInt(AddrWidth.W))
    val SRAM_W_Addr = Output(UInt(AddrWidth.W))
    val SRAM_R_Data = Input(UInt(AddrWidth.W))
    val SRAM_W_Data = Output(UInt(AddrWidth.W))
    val SRAM_R_Enable = Output(Bool())
    val SRAM_W_Enable = Output(Bool())
    val SRAM_R_Mask = Output(UInt(DataWidth.W))
    val SRAM_W_Mask = Output(UInt(DataWidth.W))
    val SRAM_R_Valid = Input(Bool())
    val SRAM_W_Valid = Input(Bool())
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

    // Configurable paramater to set how many cycle delay after memory manipulation is OK
    val LoadStoreDelay = 5

    // I: maintain SRAM_R signals
    ExternalIO.SRAM_R_Addr := Mux(
        (Slave_AR.iSlaveARvalid && Slave_R.iSlaveRready),
        (Slave_AR.iSlaveARaddr),
        (0.U(AddrWidth.W)) // To trigger error if AXI went wrong
    )
    ExternalIO.SRAM_R_Enable := (Slave_AR.iSlaveARvalid && Slave_R.iSlaveRready)
    ExternalIO.SRAM_R_Mask := MuxCase(0.U(DataWidth.W), Seq(
        (Slave_AR.iSlaveARsize === AxSIZE_1B) -> Cat(Fill(DataWidth - ByteWidth, 0.U(1.W)), Fill(ByteWidth, 1.U(1.W))),
        (Slave_AR.iSlaveARsize === AxSIZE_2B) -> Cat(Fill(DataWidth - HalfWidth, 0.U(1.W)), Fill(HalfWidth, 1.U(1.W))),
        (Slave_AR.iSlaveARsize === AxSIZE_4B) -> Cat(Fill(WordWidth, 1.U(1.W))),
    ))

    // II: maintain SRAM_W signals
    ExternalIO.SRAM_W_Addr := Mux(
        (Slave_AW.iSlaveAWvalid && Slave_W.iSlaveWvalid && Slave_B.iSlaveBready),
        (Slave_AW.iSlaveAWaddr),
        (0.U(AddrWidth.W))
    )
    ExternalIO.SRAM_W_Enable := (Slave_AW.iSlaveAWvalid && Slave_W.iSlaveWvalid && Slave_B.iSlaveBready)
    ExternalIO.SRAM_W_Data := Slave_W.iSlaveWdata
    ExternalIO.SRAM_W_Mask := MuxCase(0.U(DataWidth.W), Seq(
        (Slave_AW.iSlaveAWsize === AxSIZE_1B) -> Cat(Fill(DataWidth - ByteWidth, 0.U(1.W)), Fill(ByteWidth, 1.U(1.W))),
        (Slave_AW.iSlaveAWsize === AxSIZE_2B) -> Cat(Fill(DataWidth - HalfWidth, 0.U(1.W)), Fill(HalfWidth, 1.U(1.W))),
        (Slave_AW.iSlaveAWsize === AxSIZE_4B) -> Cat(Fill(WordWidth, 1.U(1.W))),
    ))

    // III: issue memory read and write
    val ReadResponse = ExternalIO.SRAM_R_Data

    // IV: delay data writeback, using ShiftRegister API to create register delay
    // ready: Depends on state of SRAM
    // valid: ShiftRegister()
    // other: ShiftRegister()

    Slave_AW.oSlaveAWready := true.B
    
    Slave_W.oSlaveWready := true.B

    Slave_B.oSlaveBvalid := ShiftRegister(ExternalIO.SRAM_W_Valid, LoadStoreDelay)
    Slave_B.oSlaveBresp := ShiftRegister(BRESP_OKAY, LoadStoreDelay)
    Slave_B.oSlaveBid := ShiftRegister(Slave_AW.iSlaveAWid, LoadStoreDelay)

    Slave_AR.oSlaveARready := true.B

    Slave_R.oSlaveRvalid := ShiftRegister(ExternalIO.SRAM_R_Valid, LoadStoreDelay)
    Slave_R.oSlaveRresp := ShiftRegister(RRESP_OKAY, LoadStoreDelay)
    Slave_R.oSlaveRdata := ShiftRegister(ReadResponse, LoadStoreDelay)
    Slave_R.oSlaveRlast := ShiftRegister(true.B, LoadStoreDelay)
    Slave_R.oSlaveRid := ShiftRegister(Slave_AR.iSlaveARid, LoadStoreDelay)
}