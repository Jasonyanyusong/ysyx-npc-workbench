package ysyx.message

import chisel3._
import chisel3.util._

class IF2ID_Msg extends Bundle {
    val instr = Output(UInt(32.W))
    val pc = Output(UInt(32.W))
    
    val valid = Output(Bool())
    val ready = Input(Bool())
}

class ID2EX_Msg extends Bundle {
    val instr = Output(UInt(32.W))
    val pc = Output(UInt(32.W))

    val src1 = Output(UInt(32.W))
    val src2 = Output(UInt(32.W))
    val imm  = Output(UInt(32.W))

    val ex_op = Output(UInt(6.W))
    val ls_op = Output(UInt(4.W))
    val wb_op = Output(UInt(4.W))

    val dnpc = Output(UInt(32.W))
}

class EX2LS_Msg extends Bundle {
    val instr = Output(UInt(32.W))
    val pc = Output(UInt(32.W))

    val ex_ret = Output(UInt(32.W))
    val ls_src = Output(UInt(32.W))
    
    val ls_op = Output(UInt(4.W))
    val wb_op = Output(UInt(4.W))

    val dnpc = Output(UInt(32.W))
}

class LS2WB_Msg extends Bundle {
    val instr = Output(UInt(32.W))
    val pc = Output(UInt(32.W))

    val ex_ret = Output(UInt(32.W))
    val ls_ret = Output(UInt(32.W))

    val wb_op = Output(UInt(4.W))

    val dnpc = Output(UInt(32.W))
}