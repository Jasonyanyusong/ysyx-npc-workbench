package npcb
import chisel3._
import chisel3.util._

class EXU extends Module{
    val io = IO(new Bundle{
        val EXU_getRS1 = Output(UInt(5.W))
        val EXU_getRS2 = Output(UInt(5.W))
        val EXU_getRD = Output(UInt(5.W))
        val EXU_ALUEnable = Output(Bool())
        val EXU_LSUEnable = Output(Bool())
        val EXU_LSUNeedWriteBack = Output(Bool())
        val EXU_EXUOperateCode = Output(UInt(4.W))
        val EXU_EXUTreatUnsigned = Output(Bool())
        val EXU_EXUTreatSigned = Output(Bool())
        val EXU_ALUWriteBackData = Input(UInt(64.W))
        val EXU_LSUWriteBackData = Input(UInt(64.W))
    })

    val EXU_ALUModule = Module(new ALU)
    val EXU_LSUModule = Module(new LSU)

    EXU_ALUModule.io.ALU_getRS1 := io.EXU_getRS1
    EXU_ALUModule.io.ALU_getRS2 := io.EXU_getRS2
    EXU_ALUModule.io.ALU_getRD := io.EXU_getRD
    EXU_ALUModule.io.ALU_ALUEnable := io.EXU_ALUEnable
    EXU_ALUModule.io.ALU_EXUOperateCode := io.EXU_EXUOperateCode
    EXU_ALUModule.io.ALU_EXUTreatUnsigned := io.EXU_EXUTreatUnsigned
    EXU_ALUModule.io.ALU_EXUTreatSigned := io.EXU_EXUTreatSigned
    io.EXU_ALUWriteBackData := EXU_ALUModule.io.ALU_WriteBackData

    EXU_LSUModule.io.LSU_getRS1 := io.EXU_getRS1
    EXU_LSUModule.io.LSU_getRS2 := io.EXU_getRS2
    EXU_LSUModule.io.LSU_getRD := io.EXU_getRD
    EXU_LSUModule.io.LSU_LSUEnable := io.EXU_LSUEnable
    EXU_LSUModule.io.LSU_EXUOperateCode := io.EXU_EXUOperateCode
    EXU_LSUModule.io.LSU_EXUTreatUnsigned := io.EXU_EXUTreatUnsigned
    EXU_LSUModule.io.LSU_EXUTreatSigned := io.EXU_EXUTreatSigned
    io.EXU_LSUWriteBackData := EXU_LSUModule.io.LSU_WriteBackData
}