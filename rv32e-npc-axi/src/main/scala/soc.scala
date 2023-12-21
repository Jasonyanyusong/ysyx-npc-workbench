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

package soc

import chisel3._
import chisel3.util._

import npc._

class SOCIO extends Bundle {
    // Memory Write Channel
    val sram_waddr_o = Output(UInt(32.W))
    val sram_wdata_o = Output(UInt(32.W))
    val sram_wsize_o = Output(UInt(2.W))

    // Memory Read Channel
    val sram_raddr_o = Output(UInt(32.W))
    val sram_rdata_i = Input (UInt(32.W))
    val sram_rsize_o = Output(UInt(2.W))
}

class SOC extends Module {
    // SOC level IO
    val io = IO(new SOCIO)

    // Processor core
    val core = Module(new NPC)

    
}