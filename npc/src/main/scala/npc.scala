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

package npc

import chisel3._
import chisel3.util._

import npc.helper.defs.Base._
import npc.helper.defs.CSR_LUT._

object NPCIO extends Bundle{
    // TODO: Define NPC's IO
}

object NPCIODebug extends Bundle{
    // TODO: Define NPC's debug IO
}

class NPC extends Module{
    val ioNPC = IO(new NPCIO)
    val ioNPCDebug = IO(new NPCIODebug)

    // Pipeline Components
    val NPC_IFU = Module(new IFU)
    val NPC_IDU = Module(new IDU)
    val NPC_EXU = Module(new EXU)
    val NPC_LSU = Module(new LSU)
    val NPC_WBU = Module(new WBU)

    // GPR Maintain and Manipulation
    val GPR = Mem(RegSum, 0.U(DataWidth.W))
    def GPR_Read(index : UInt) = Mux(index === 0.U, 0.U(DataWidth.W), GPR(index))

    // CSR Maintain and Manipulation
    val mstatus = RegInit(0.U(DataWidth.W))
    val mepc = RegInit(0.U(DataWidth.W))
    val mtvec = RegInit(0.U(DataWidth.W))
    val mcause = RegInit(0.U(DataWidth.W))

    def CSR_Read(index : UInt) = MuxCase(0.U(DataWidth.W), Array(
        (index === CSR_MSTATUS) -> (mstatus.asUInt), // mstatus
        (index === CSR_MTVEC) -> (mtvec.asUInt  ), // mtvec
        (index === CSR_MEPC) -> (mepc.asUInt   ), // mepc
        (index === CSR_MCAUSE) -> (mcause.asUInt )  // mcause
    ))

    val isZicsr = false.B
    val ZicsrIndex = 0.U(CSRIDWidth.W)
    val ZicsrNewVal = 0.U(DataWidth.W)

    def CSR_Write(CSR_idx, CSR_val : UInt) = {
        switch (ZicsrIndex.asUInt){
            is(CSR_MSTATUS){
                Mux(isZicsr.asBool, mstatus := ZicsrNewVal, mstatus := mstatus)
            }
            is(CSR_MTVEC){
                Mux(isZicsr.asBool, mtvec := ZicsrNewVal, mtvec := mtvec)
            }
            is(CSR_MEPC){
                Mux(isZicsr.asBool, mepc := ZicsrNewVal, mepc := mepc)
            }
            is(CSR_MCAUSE){
                Mux(isZicsr.asBool, mcause := ZicsrNewVal, mcause := mcause)
            }
        }
    }

    // TODO: Write NPC Logic
}