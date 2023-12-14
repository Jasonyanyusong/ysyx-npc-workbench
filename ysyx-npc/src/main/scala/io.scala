package ysyx.io

import chisel3._
import chisel3.util._

class NonAXI_IF extends Bundle {
    val pc_i = Input(UInt(32.W))
    
    val pc_o = Output(UInt(32.W))
    val ren_o = Output(Bool())

    val instr_i = Input(UInt(32.W))
}

class NonAXI_LS extends Bundle {
    val addr = Output(UInt(32.W))

    val op = Output(UInt(2.W))
    val sz = Output(UInt(2.W))

    val rdata = Input(UInt(32.W))
    val wdata = Output(UInt(32.W))
}