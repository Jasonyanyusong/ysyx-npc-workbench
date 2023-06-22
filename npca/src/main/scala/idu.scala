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

class IDU extends Module{
    val io = IO(new Bundle{
        val IDU_I_inst = Input(UInt(32.W))

        val IDU_O_rs1 = Output(UInt(5.W))
        val IDU_O_rs2 = Output(UInt(5.W))
        val IDU_O_rd  = Output(UInt(5.W))

        val IDU_O_InstIndicator = Output(UInt(10.W)) // debug only, do not participate in logic

        val IDU_O_EXU_Int_opcode = Output(UInt(4.W))
        val IDU_O_EXU_Int_sign = Output(Bool())
        val IDU_O_EXU_Int_computeLength = Output(UInt(2.W))
        val IDU_O_EXU_Int_resultPart = Output(Bool())
        val IDU_O_EXU_Int_operand = Output(UInt(4.W))

        val IDU_O_LSU_optype = Output(UInt(4.W))
        val IDU_O_LSU_sign = Output(Bool())
        val IDU_O_LSU_len = Output(UInt(2.W))
        val IDU_O_LSU_memOperationType = Output(UInt(2.W))

        val IDU_O_writeBackGPRType = Output(UInt(4.W))
        val IDU_O_isHalt = Output(Bool())

        val IDU_O_imm = Output(UInt(64.W))
    })

    io.IDU_O_rs1 := io.IDU_I_inst(19, 15)
    io.IDU_O_rs2 := io.IDU_I_inst(24, 20)
    io.IDU_O_rd  := io.IDU_I_inst(11,  7)

    io.IDU_O_isHalt := opcode_IDU_isHalt.IDU_NOTHALT
}