package npcb
import chisel3._
import chisel3.util._

class NPCB extends Module{
    val io = IO(new Bundle{
        val halt = Output((Bool()))
        val currentPC = Output(UInt(64.W))
        val instIn = Input(UInt(32.W))

        val memRW = Output(Bool())
        val memAddr = Output(UInt(64.W))
        val memMem2LSU = Input(UInt(64.W))
        val memLSU2Mem = Output(UInt(64.W))
    })
    val GPR = Mem(32, UInt(64.W))
    val PC = RegInit(0.U(64.W))
    val Memory = Mem(1024/4, UInt(32.W))
    def GPRRead(GPR_Index : UInt) = Mux(GPR_Index === 0.U, 0.U(64.W), GPR(GPR_Index))

    val NPCB_IFU = Module(new IFU)
    NPCB_IFU.io.IFU_PC := PC
    NPCB_IFU.io.IFU_InstIn := io.instIn

    val NPCB_IDU = Module(new IDU)
    NPCB_IDU.io.IDU_InstIn := NPCB_IFU.io.IFU_InstOut

    val NPCB_EXU = Module(new EXU)

    val NPCB_TNI = Module(new ToNextInst)

}