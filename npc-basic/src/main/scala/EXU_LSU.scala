package npcb
import chisel3._
import chisel3.util._

class LSU extends Module{
    val io = IO(new Bundle{
        val LSU_getRS1 = Input(UInt(5.W))
        val LSU_getRS2 = Input(UInt(5.W))
        val LSU_getRD = Input(UInt(5.W))
        val LSU_LSUEnable = Input(Bool())
        val LSU_EXUOperateCode = Input(UInt(4.W))
        val LSU_EXUTreatUnsigned = Input(Bool())
        val LSU_EXUTreatSigned = Input(Bool())
        val LSU_NeedWriteBack = Output(Bool())
        val LSU_WriteBackData = Output(UInt(64.W))
    })
}