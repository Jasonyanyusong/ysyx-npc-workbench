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

import npc.helper.defs.Base._

class SRAM_Internal_IO extends Bundle{
    // TODO: add internal IO (connect LSU for AW W B and Arbiter for LSU/IFU AR and R)
}

class SRAM_External_IO extends Bundle{
    // TODO: add external IO (connect NPC or Debug Environment)
}

class SRAM extends Module{
    // TODO: add functional
    val InternalIO = IO(new SRAM_Internal_IO)
    val ExternalIO = IO(new SRAM_External_IO)
}