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
import npc.axi.params.Base._

class AXIArbiter extends Module{
    // Rule: LSU Read > IFU iFetch
    // This is the interface between the "module parts external" to "SoC/simulation external"

    val IFU_AR = IO(Filpped(new AXIMasterAR))
    val IFU_R = IO(Filpped(new AXIMasterR))

    val LSU_AR = IO(Filpped(new AXIMasterAR))
    val LSU_R = IO(Filpped(new AXIMasterR))

    val Arbiter_AR = IO(new AXIMasterAR)
    val Arbiter_R = IO(new AXIMasterR)

    val ArbiterState = RegInit(0.U(2.W))
    val ArbiterBusy = RegInit(false.B)

    // I: Update State if Arbiter's Job has finished
    ArbiterState := Mux(!ArbiterBusy, MuxCase(Arbiter_NO_REQUEST, Array(
        ((!IFU_AR.iMasterARready) && (!LSU_AR.iMasterARready)) -> (Arbiter_NO_REQUEST),
        (  IFU_AR.iMasterARready  && (!LSU_AR.iMasterARready)) -> (Arbiter_IFU_ONLY),
        ((!IFU_AR.iMasterARready) &&   LSU_AR.iMasterARready ) -> (Arbiter_LSU_ONLY),
        (  IFU_AR.iMasterARready  &&   LSU_AR.iMasterARready ) -> (Arbiter_BOTH_REQUEST)
    )), ArbiterState)

    // ArbiterBusy indicating it is working
    ArbiterBusy := !(IFU_AR.iMasterARready && LSU_AR.iMasterARready )

    // II: According to ArbiterState, forward signals from IFU / LSU to SRAM / SoC
    Arbiter_AR.oMasterARvalid := Mux(ArbiterBusy, MuxCase(false.B, Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (false.B),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (IFU_AR.oMasterARvalid),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (LSU_AR.oMasterARvalid),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (LSU_AR.oMasterARvalid)
    )), false.B)

    Arbiter_AR.oMasterARaddr := Mux(ArbiterBusy, MuxCase(0.U(AddrWidth.W), Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (0.U(AddrWidth.W)),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (IFU_AR.oMasterARaddr),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (LSU_AR.oMasterARaddr),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (LSU_AR.oMasterARaddr)
    )), 0.U(AddrWidth.W))

    Arbiter_AR.oMasterARid := Mux(ArbiterBusy, MuxCase(0.U(AXI_ID_LEN.W), Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (0.U(AXI_ID_LEN.W)),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (IFU_AR.oMasterARid),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (LSU_AR.oMasterARid),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (LSU_AR.oMasterARid)
    )), 0.U(AXI_ID_LEN.W))

    Arbiter_AR.oMasterARlen := Mux(ArbiterBusy, MuxCase(0.U(AXI_LEN_LEN.W), Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (0.U(AXI_LEN_LEN.W)),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (IFU_AR.oMasterARlen),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (LSU_AR.oMasterARlen),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (LSU_AR.oMasterARlen)
    )), 0.U(AXI_LEN_LEN.W))

    Arbiter_AR.oMasterARsize := Mux(ArbiterBusy, MuxCase(0.U(AXI_SIZE_LEN.W), Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (0.U(AXI_SIZE_LEN.W)),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (IFU_AR.oMasterARsize),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (LSU_AR.oMasterARsize),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (LSU_AR.oMasterARsize)
    )), 0.U(AXI_SIZE_LEN.W))

    Arbiter_AR.oMasterARburst := Mux(ArbiterBusy, MuxCase(0.U(AXI_BURST_LEN.W), Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (0.U(AXI_BURST_LEN.W)),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (IFU_AR.oMasterARburst),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (LSU_AR.oMasterARburst),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (LSU_AR.oMasterARburst)
    )), 0.U(AXI_BURST_LEN.W))

    Arbiter_R.oMasterRready := Mux(ArbiterBusy, MuxCase(false.B, Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (false.B),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (IFU_R.oMasterRready),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (LSU_R.oMasterRready),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (LSU_R.oMasterRready)
    )), false.B)

    // III: According to ArbiterState, forward signals from SRAM / SoC to IFU / LSU
    IFU_AR.iMasterARready := Mux(ArbiterBusy, false.B, true.B)
    LSU_AR.iMasterARready := Mux(ArbiterBusy, false.B, true.B)

    IFU_R.iMasterRvalid := MuxCase(false.B, Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (false.B),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (Arbiter_R.iMasterRvalid),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (false.B),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (false.B)
    ))
    LSU_R.iMasterRvalid := MuxCase(false.B, Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (false.B),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (false.B),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (Arbiter_R.iMasterRvalid),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (Arbiter_R.iMasterRvalid)
    ))

    IFU_R.iMasterRresp := MuxCase(0.U(AXI_RESP_LEN), Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (0.U(AXI_RESP_LEN)),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (Arbiter_R.iMasterRresp),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (0.U(AXI_RESP_LEN)),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (0.U(AXI_RESP_LEN))
    ))
    LSU_R.iMasterRresp := MuxCase(0.U(AXI_RESP_LEN), Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (0.U(AXI_RESP_LEN)),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (0.U(AXI_RESP_LEN)),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (Arbiter_R.iMasterRresp),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (Arbiter_R.iMasterRresp)
    ))

    IFU_R.iMasterRdata := MuxCase(0.U(AXI_DATA_LEN), Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (0.U(AXI_DATA_LEN)),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (Arbiter_R.iMasterRdata),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (0.U(AXI_DATA_LEN)),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (0.U(AXI_DATA_LEN))
    ))
    LSU_R.iMasterRdata := MuxCase(0.U(AXI_DATA_LEN), Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (0.U(AXI_DATA_LEN)),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (0.U(AXI_DATA_LEN)),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (Arbiter_R.iMasterRdata),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (Arbiter_R.iMasterRdata)
    ))

    IFU_R.iMasterRlast := MuxCase(false.B, Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (false.B),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (Arbiter_R.iMasterRlast),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (false.B),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (false.B)
    ))
    LSU_R.iMasterRlast := MuxCase(false.B, Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (false.B),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (false.B),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (Arbiter_R.iMasterRlast),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (Arbiter_R.iMasterRlast)
    ))

    IFU_R.iMasterRid := MuxCase(0.U(AXI_ID_LEN), Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (0.U(AXI_ID_LEN)),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (Arbiter_R.iMasterRid),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (0.U(AXI_ID_LEN)),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (0.U(AXI_ID_LEN))
    ))
    LSU_R.iMasterRid := MuxCase(0.U(AXI_ID_LEN), Array(
        (ArbiterState === Arbiter_NO_REQUEST)   -> (0.U(AXI_ID_LEN)),
        (ArbiterState === Arbiter_IFU_ONLY)     -> (0.U(AXI_ID_LEN)),
        (ArbiterState === Arbiter_LSU_ONLY)     -> (Arbiter_R.iMasterRid),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (Arbiter_R.iMasterRid)
    ))

    // IV: Update ArbiterBusy's state
    ArbiterBusy := Mux(ArbiterBusy, MuxCase(ArbiterBusy, Array(
        (ArbiterState === Arbiter_NO_REQUEST  ) -> (false.B),
        (ArbiterState === Arbiter_IFU_ONLY    ) -> (IFU_R.iMasterRvalid),
        (ArbiterState === Arbiter_LSU_ONLY    ) -> (LSU_R.iMasterRvalid),
        (ArbiterState === Arbiter_BOTH_REQUEST) -> (LSU_R.iMasterRvalid)
    )), ArbiterBusy)
}