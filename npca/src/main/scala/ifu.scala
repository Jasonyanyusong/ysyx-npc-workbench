/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing 101 High School
* Copyright (c) 2023 Yusong Yan, University of Washington - Seattle
*
* YSYX-NPCA is licensed under Mulan PSL v2.
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

package npca
import chisel3._
import chisel3-util._

class IFU extends Module{
    // Instruction Fetch Unit fetch an instruction from memory by giving the PC's val
    val io = IO(new Bundle{
        val IFU_I_PC = Input(UInt(64.W))
        val IFU_O_PC = Output(UInt(64.W))
        val IFU_I_inst = Input(UInt(32.W))
        val IFU_O_inst = Output(UInt(32.W))
    })
    io.IFU_O_inst := io.IFU_I_inst
    io.IFU_O_PC := io.IFU_I_PC
}