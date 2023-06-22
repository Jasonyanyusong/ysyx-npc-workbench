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

    // all types of immediates are bit-manipulated, concated
    val immI = io.IDU_I_inst(31, 20)
    val immS = Cat(io.IDU_I_inst(31, 25), io.IDU_I_inst(11, 7))
    val immB = Cat(io.IDU_I_inst(31, 31), io.IDU_I_inst(7, 7), io.IDU_I_inst(30, 25), io.IDU_I_inst(11, 8), 0.U)
    val immU = Cat(io.IDU_I_inst(31, 12), Fill(12, 0.U))
    val immJ = Cat(io.IDU_I_inst(31, 31), io.IDU_I_inst(19, 12), io.IDU_I_inst(20, 20), io.IDU_I_inst(30, 21), 0.U)

    // sign-extend these immediates for later calculation
    val SignExtend_immI = Cat(Fill(52, immI(11)), immI)
    val SignExtend_immS = Cat(Fill(52, immS(11)), immS)
    val SignExtend_immB = Cat(Fill(51, immB(12)), immB)
    val SignExtend_immU = Cat(Fill(32, immU(31)), immU)
    val SignExtend_immJ = Cat(Fill(43, immJ(20)), immJ)
    val NoImmediateNum  = Fill(64, 0.U) // if an instruction's type is not I, S, B, U, J
}