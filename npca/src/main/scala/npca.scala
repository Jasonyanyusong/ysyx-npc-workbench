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
import chisel3.util._

class npca extends Module{
    // TODO
    val io = IO(new Bundle{
        // IFU
        val NPC_sendCurrentPC   = Output(UInt(64.W))
        val NPC_getCurrentInst  = Input(UInt(64.W))

        // IDU (debug/simulation)
        val NPC_InstIndicator   = Output(UInt(10.W))
        val NPC_InstType        = Output(UInt(4.W))
        val NPC_PrivStatus      = Output(UInt(4.W))

        // LSU
        val NPC_memReadEnable   = Output(Bool())
        val NPC_memReadAddress  = Output(UInt(64.W))
        val NPC_memReadLength   = Output(UInt(2.W))
        val NPC_memReadResult   = Input(UInt(64.W))

        val NPC_memWriteEnable  = Output(Bool())
        val NPC_memWriteAddress = Output(UInt(64.W))
        val NPC_memWriteLength  = Output(UInt(2.W))
        val NPC_memWriteData    = Output(UInt(64.W))
    })

    val PC = RegInit("h80000000".asUInt)
    val GPR = Mem(32, UInt(64.W))
    val CSR = Mem(4096, UInt(64.W))
    def GPR_Read (index : UInt) = Mux(index === 0.U, 0.U(64.W), GPR(index))
    def CSR_Read (index : UInt) = CSR(index)

    // IFU
    val npca_IFU = Module(new IFU)
    npca_IFU.io.IFU_I_PC                := PC
    io.NPC_sendCurrentPC                := npca_IFU.io.IFU_O_PC
    npca_IFU.io.IFU_I_inst              := io.NPC_getCurrentInst

    // IDU
    val npca_IDU = Module(new IDU)
    npca_IDU.io.IDU_I_inst              := npca_IFU.io.IFU_O_inst
    val SRC1                             = GPR_Read(npca_IDU.io.IDU_O_rs1)
    val SRC2                             = GPR_Read(npca_IDU.io.IDU_O_rs2)
    val IMM                              = npca_IDU.io.IDU_O_imm
    val RD                               = npca_IDU.io.IDU_O_rd

    io.NPC_InstIndicator                := npca_IDU.io.IDU_O_InstIndicator
    io.NPC_InstType                     := npca_IDU.io.IDU_O_InstType
    io.NPC_PrivStatus                   := npca_IDU.io.IDU_O_privState

    // EXU
    val npca_EXU = Module(new EXU)
    npca_EXU.io.EXU_I_src1              := SRC1
    npca_EXU.io.EXU_I_src2              := SRC2
    npca_EXU.io.EXU_I_imm               := IMM
    npca_EXU.io.EXU_I_cPC               := PC

    npca_EXU.io.EXU_I_Int_opcode        := npca_IDU.io.IDU_O_EXU_Int_opcode
    npca_EXU.io.EXU_I_Int_sign          := npca_IDU.io.IDU_O_EXU_Int_sign
    npca_EXU.io.EXU_I_Int_computeLength := npca_IDU.io.IDU_O_EXU_Int_computeLength
    npca_EXU.io.EXU_I_Int_resultPart    := npca_IDU.io.IDU_O_EXU_Int_resultPart
    npca_EXU.io.EXU_I_Int_operand       := npca_IDU.io.IDU_O_EXU_Int_operand

    val EXU_ValuResult                   = npca_EXU.io.EXU_O_ValuResult
    val EXU_BoolResult                   = npca_EXU.io.EXU_O_BoolResult

    // LSU
    val npca_LSU = Module(new LSU)
    npca_LSU.io.LSU_I_optype            := npca_IDU.io.IDU_O_LSU_optype
    npca_LSU.io.LSU_I_sign              := npca_IDU.io.IDU_O_LSU_sign
    npca_LSU.io.LSU_I_len               := npca_IDU.io.IDU_O_LSU_len
    npca_LSU.io.LSU_I_memOperationType  := npca_IDU.io.IDU_O_LSU_memOperationType
    npca_LSU.io.LSU_I_EXUresult         := EXU_ValuResult
    npca_LSU.io.LSU_I_src1              := SRC1
    npca_LSU.io.LSU_I_src2              := SRC2

    io.NPC_memReadEnable                := npca_LSU.io.LSU_O_memReadEnable
    io.NPC_memReadAddress               := npca_LSU.io.LSU_O_memReadAddress
    io.NPC_memReadLength                := npca_LSU.io.LSU_O_memReadLength
    npca_LSU.io.LSU_I_memReadResult     := io.NPC_memReadResult

    io.NPC_memWriteEnable               := npca_LSU.io.LSU_O_memWriteEnable
    io.NPC_memWriteAddress              := npca_LSU.io.LSU_O_memWriteAddress
    io.NPC_memWriteLength               := npca_LSU.io.LSU_O_memWriteLength
    io.NPC_memWriteData                 := npca_LSU.io.LSU_O_memWriteData

    val LSU_ValuResult                   = npca_LSU.io.LSU_O_resultWriteBack

    // WBU
    val npca_WBU = Module(new WBU)
    npca_WBU.io.WBU_I_EXU_ValuResult    := EXU_ValuResult
    npca_WBU.io.WBU_I_EXU_CompResult    := EXU_BoolResult
    npca_WBU.io.WBU_I_LSU_Result        := LSU_ValuResult
    npca_WBU.io.WBU_I_PC                := PC

    npca_WBU.io.WBU_I_WriteBackType     := npca_IDU.io.IDU_O_writeBackRegType
    npca_WBU.io.WBU_I_nextPCType        := npca_IDU.io.IDU_O_nextPCStatic
    npca_WBU.io.WBU_I_PCJumpReason      := npca_IDU.io.IDU_O_PCJumpReadson

    npca_WBU.io.WBU_I_GPR_RD            := RD
    npca_WBU.io.WBU_I_CSR_RD            := 0.U(12.W) // do not toatlly support CSR now
    GPR(npca_WBU.io.WBU_O_GPR_RD)       := Mux(npca_WBU.io.WBU_O_GPR_WBenable, npca_WBU.io.WBU_O_GPR_WBdata, GPR_Read(npca_WBU.io.WBU_O_GPR_RD))
    CSR(npca_WBU.io.WBU_O_CSR_RD)       := Mux(npca_WBU.io.WBU_O_CSR_WBenable, npca_WBU.io.WBU_O_CSR_WBdata, CSR_Read(npca_WBU.io.WBU_O_CSR_RD))

    PC := npca_WBU.io.WBU_O_nextPC
}