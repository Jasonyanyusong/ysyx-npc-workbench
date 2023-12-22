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

package npc.arbiter

import chisel3._
import chisel3.util._

import npc.helper.axi._

object ArbiterMode {
    val ArbiterFree = 0.U(2.W)
    val ArbiterIFOn = 1.U(2.W)
    val ArbiterLSOn = 2.U(2.W)
}

class Arbiter extends Module {
    // IFU read channels
    val ifu_ar_b = IO(Flipped(new AXIMasterAR))
    val ifu_r_b  = IO(Flipped(new AXIMasterR))

    // LSU read channels
    val lsu_ar_b = IO(Flipped(new AXIMasterAR))
    val lsu_r_b  = IO(Flipped(new AXIMasterR))

    // Output channels
    val out_ar_b = IO(new AXIMasterAR)
    val out_r_b  = IO(new AXIMasterR)

    // Arbiter mode
    val mode = RegInit(ArbiterMode.ArbiterFree)

    // Master requesting signal?
    val ifu_request = ifu_ar_b.oMasterARvalid
    val lsu_request = lsu_ar_b.oMasterARvalid

    // Check and update status when arbiter is
    // in free mode, otherwise don't change
    mode := Mux(mode === ArbiterMode.ArbiterFree, MuxCase(
        mode, Array(
            (ifu_request === false.B && lsu_request === false.B) -> (ArbiterMode.ArbiterFree),
            (ifu_request === false.B && lsu_request === true.B ) -> (ArbiterMode.ArbiterLSOn),
            (ifu_request === true.B  && lsu_request === false.B) -> (ArbiterMode.ArbiterIFOn),
            (ifu_request === true.B  && lsu_request === true.B ) -> (ArbiterMode.ArbiterLSOn)
        )
    ), mode) // don't change the mode if it's not free

    // TODO: connect datapath according to arbiter's mode

    // TODO: update mode according to read response
}