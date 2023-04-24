package npcb
import chisel3._
import chisel3.util._

class ALU extends Module{
    val io = IO(new Bundle{
        val ALU_getRS1 = Input(UInt(5.W))
        val ALU_getRS2 = Input(UInt(5.W))
        val ALU_getRD = Input(UInt(5.W))
        val ALU_ALUEnable = Input(Bool())
        val ALU_EXUOperateCode = Input(UInt(4.W))
        val ALU_EXUTreatUnsigned = Input(Bool())
        val ALU_EXUTreatSigned = Input(Bool())
        val ALU_WriteBackData = Output(UInt(64.W))
    })
}