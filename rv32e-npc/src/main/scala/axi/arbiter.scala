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

import npc.axi.master._
import npc.axi.slave._
import npc.axi.params.ArbiterDefs._

class AXIArbiter extends Module{
    // Rule: LSU Read > IFU iFetch
    // This is the interface between the "module parts external" to "SoC/simulation external"

    /*
    val IFU_AR = IO(Flipped(new AXIMasterAR))
    val IFU_R  = IO(Flipped(new AXIMasterR))
    val LSU_AR = IO(Flipped(new AXIMasterAR))
    val LSU_R  = IO(Flipped(new AXIMasterR))

    val ArbitAR = IO(new AXIMasterAR)
    val ArbitR  = IO(new AXIMasterR)

    // TODO: not sequential/cycle accurate

    if(IFU_AR.oMasterARvalid.litToBoolean && LSU_AR.oMasterARvalid.litToBoolean){
        // Both LSU and IFU send read request, satisfy LSU first
        LSU_AR <> ArbitAR
        LSU_R  <> ArbitR
    }else if(IFU_AR.oMasterARvalid.litToBoolean && (!LSU_AR.oMasterARvalid.litToBoolean)){
        // Only IFU
        IFU_AR <> ArbitAR
        IFU_R  <> ArbitR
    }else if((!IFU_AR.oMasterARvalid.litToBoolean) && LSU_AR.oMasterARvalid.litToBoolean){
        // Only LSU
        LSU_AR <> ArbitAR
        LSU_R  <> ArbitR
    }else{
        // No read request
        ArbitAR.oMasterARvalid := false.B
        ArbitR.oMasterRready   := false.B
    }
    */

    val IFU_AR = IO(new AXISlaveAR)
    val IFU_R = IO(new AXISlaveR)

    val LSU_AR = IO(new AXISlaveAR)
    val LSU_R = IO(new AXISlaveR)

    val ArbiterState = RegInit(0.U(2.W))
    val ArbiterBusy = RegInit(false.B)

    // I: Update State if Arbiter's Job has finished
    ArbiterState := Mux(!ArbiterBusy, MuxCase(Arbiter_NO_REQUEST, Array(
        ((!IFU_AR.iSlaveARvalid) && (!LSU_AR.iSlaveARvalid)) -> (Arbiter_NO_REQUEST),
        (  IFU_AR.iSlaveARvalid  && (!LSU_AR.iSlaveARvalid)) -> (Arbiter_IFU_ONLY),
        ((!IFU_AR.iSlaveARvalid) &&   LSU_AR.iSlaveARvalid ) -> (Arbiter_LSU_ONLY),
        (  IFU_AR.iSlaveARvalid  &&   LSU_AR.iSlaveARvalid ) -> (Arbiter_BOTH_REQUEST)
    )), ArbiterState)

    // II: According to ArbiterState, forward signals to IFU and LSU

}