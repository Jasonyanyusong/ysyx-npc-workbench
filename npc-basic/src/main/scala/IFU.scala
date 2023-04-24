package npcb
import chisel3._
import chisel3.util._

class IFU extends Module{
    val io = IO(new Bundle{
        val IFU_PC = Input(UInt(64.W))
        val IFU_InstIn = Input(UInt(32.W))
        val IFU_InstOut = Output(UInt(32.W))
    })

    io.IFU_InstOut := io.IFU_InstIn

    //io.IFU_Inst := Memory(IFU_PC)
}