package ysyx.ifu

import chisel3._
import chisel3.util._

import ysyx.message._
import ysyx.io._

class IFU extends Module {
    val IF2ID = IO(new Bundle {val msg = Decoupled(new IF2ID_Msg) })
    val IF_IO = IO(new Bundle {val msg = Decoupled(new NonAXI_IF) })

    val state_idle :: state_wait_ready :: Nil = Enum(2)
    val state = RegInit(state_idle)

    state := MuxLookUp(state, state_idle, List(
        state_idle       -> Mux(IF2ID.msg.valid, state_wait_ready, state_idle),
        state_wait_ready -> Mux(IF2ID.msg.ready, state_idle, state_wait_ready)
    ))

    /*IF_IO.msg.pc_o := MuxLookUp(state, 0.U(32.W), List(
        state_idle -> 
    ))*/
}