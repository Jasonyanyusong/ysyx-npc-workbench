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

package npc.axi.params

import chisel3._
import chisel3.util._

object Base{
    val AXI_DataWidth = 64

    val AxSIZE_1B = 0.U(3.W)
    val AxSIZE_2B = 1.U(3.W)
    val AxSIZE_4B = 2.U(3.W)

    val AXI_ADDR_LEN = 32
    val AXI_ID_LEN = 4
    val AXI_LEN_LEN = 8
    val AXI_SIZE_LEN = 3
    val AXI_BURST_LEN = 2
    val AXI_RESP_LEN = 2
    val AXI_DATA_LEN = 64
}

object AXI2Lable{
    // B
    val BRESP_OKAY = 0.U(3.W)

    // R
    val RRESP_OKAY = 0.U(3.W)
}

object ArbiterDefs{
    val Arbiter_NO_REQUEST = 0.U(2.W)
    val Arbiter_IFU_ONLY = 1.U(2.W)
    val Arbiter_LSU_ONLY = 2.U(2.W)
    val Arbiter_BOTH_REQUEST = 3.U(2.W)
}