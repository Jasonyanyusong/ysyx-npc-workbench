package npcb
import chisel3._
import chisel3.util._

class ToNextInst extends Module{
    val io = IO(new Bundle{
        val ToNextInst_isJump = Input(Bool())
        val ToNextInst_currentPC = Input(UInt(32.W))
        val ToNextInst_JumpAddress = Input(UInt(32.W))
        val ToNextInst_nextPC = Output(UInt(32.W))
    })
}