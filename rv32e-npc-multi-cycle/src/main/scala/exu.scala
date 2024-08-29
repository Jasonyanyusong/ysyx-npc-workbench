package npc

import chisel3._
import chisel3.util._

object NPC_EXU_EX_Ops {
    def EX_NOP    =  0.U(4.W)
    def EX_PS1    =  1.U(4.W)
    def EX_ADD    =  2.U(4.W)
    def EX_SLT    =  3.U(4.W)
    def EX_SLTU   =  4.U(4.W)
    def EX_XOR    =  5.U(4.W)
    def EX_OR     =  6.U(4.W)
    def EX_AND    =  7.U(4.W)
    def EX_SLL    =  8.U(4.W)
    def EX_SRL    =  9.U(4.W)
    def EX_SRA    = 10.U(4.W)
    def EX_SUB    = 11.U(4.W)
}

class NPC_EXU_Internal_IO extends Bundle {
    val exu_internal_work_i = Input(Bool())
    val exu_internal_valid_o = Output(Bool())

    val exu_internal_ex_opcode_i = Input(UInt(4.W))
    val exu_internal_ex_src1_i = Input(UInt(32.W))
    val exu_internal_ex_src2_i = Input(UInt(32.W))

    val exu_internal_result_o = Output(UInt(32.W))

    val exu_internal_ls_opcode_i = Input(UInt(4.W))
    val exu_internal_ls_wdata_i = Input(UInt(32.W))
    val exu_internal_ls_opcode_o = Output(UInt(4.W))
    val exu_internal_ls_wdata_o = Output(UInt(32.W))

    val exu_internal_wb_opcode_i = Input(UInt(2.W))
    val exu_internal_wb_rd_i = Input(UInt(4.W))
    val exu_internal_wb_opcode_o = Output(UInt(2.W))
    val exu_internal_wb_rd_o = Output(UInt(4.W))
}

class NPC_EXU extends Module {
    // IO definitions
    val exu_internal_io = IO(new NPC_EXU_Internal_IO)

    // compute
    val EXU_OpCode = exu_internal_io.exu_internal_ex_opcode_i
    val EXU_SRC1 = exu_internal_io.exu_internal_ex_src1_i
    val EXU_SRC2 = exu_internal_io.exu_internal_ex_src2_i

    val EXU_Result = MuxCase(0.U(32.W), Array(
        (EXU_OpCode === NPC_EXU_EX_Ops.EX_NOP) -> (0.U(32.W)).asUInt,
        (EXU_OpCode === NPC_EXU_EX_Ops.EX_PS1) -> (EXU_SRC1).asUInt,
        (EXU_OpCode === NPC_EXU_EX_Ops.EX_ADD) -> (EXU_SRC1 + EXU_SRC2).asUInt,
        (EXU_OpCode === NPC_EXU_EX_Ops.EX_SLT) -> Mux(EXU_SRC1.asSInt < EXU_SRC2.asSInt, 1.U(32.W), 0.U(32.W)).asUInt,
        (EXU_OpCode === NPC_EXU_EX_Ops.EX_SLTU) -> Mux(EXU_SRC1.asUInt < EXU_SRC2.asUInt, 1.U(32.W), 0.U(32.W)).asUInt,
        (EXU_OpCode === NPC_EXU_EX_Ops.EX_XOR) -> (EXU_SRC1 ^ EXU_SRC2).asUInt,
        (EXU_OpCode === NPC_EXU_EX_Ops.EX_OR) -> (EXU_SRC1 | EXU_SRC2).asUInt,
        (EXU_OpCode === NPC_EXU_EX_Ops.EX_AND) -> (EXU_SRC1 & EXU_SRC2).asUInt,
        (EXU_OpCode === NPC_EXU_EX_Ops.EX_SLL) -> (EXU_SRC1.asUInt << EXU_SRC2(4, 0)).asUInt,
        (EXU_OpCode === NPC_EXU_EX_Ops.EX_SRL) -> (EXU_SRC1.asUInt >> EXU_SRC2(4, 0)).asUInt,
        (EXU_OpCode === NPC_EXU_EX_Ops.EX_SRA) -> (EXU_SRC1.asSInt >> EXU_SRC2(4, 0)).asUInt,
        (EXU_OpCode === NPC_EXU_EX_Ops.EX_SUB) -> (EXU_SRC1 - EXU_SRC2).asUInt,
    ))

    // connect
    exu_internal_io.exu_internal_valid_o := exu_internal_io.exu_internal_work_i

    exu_internal_io.exu_internal_result_o := EXU_Result

    exu_internal_io.exu_internal_ls_opcode_o := exu_internal_io.exu_internal_ls_opcode_i
    exu_internal_io.exu_internal_ls_wdata_o := exu_internal_io.exu_internal_ls_wdata_i

    exu_internal_io.exu_internal_wb_opcode_o := exu_internal_io.exu_internal_wb_opcode_i
    exu_internal_io.exu_internal_wb_rd_o := exu_internal_io.exu_internal_wb_rd_i
}