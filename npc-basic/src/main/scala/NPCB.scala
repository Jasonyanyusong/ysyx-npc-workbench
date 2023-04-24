package npcb
import chisel3._
import chisel3.util._

class IFU extends Module{
    // Now we make IFU just a passthrough because we do not fetch instruction from memory directly
    val io = IO(new Bundle{
        IFU_I_PC = Input(UInt(64.W))
        IFU_O_PC = Output(UInt(64.W))
        IFU_I_inst = Input(UInt(32.W))
        IFU_O_inst = Output(UInt(32.W))
    })
    io.IFU_O_inst := io.IFU_I_inst
    io.IFU_O_PC := io.IFU_I_PC
}

class IDU extends Module{
    val io = IO(new Bundle{
        IDU_I_inst = Input(UInt(32.W))
        IDU_O_OperateMem = Output(Bool())
        IDU_O_rs1 = Output(UInt(5.W))
        IDU_I_src1 = Input(UInt(64.W))
        IDU_O_src1 = Input(UInt(64.W))
        IDU_O_rs2 = Output(UInt(5.W))
        IDU_I_src2 = Input(UInt(64.W))
        IDU_O_src2 = Input(UInt(64.W))
        IDU_O_rd = Output(UInt(5.W))
        IDU_O_EXUopcode = Output(UInt(4.W))
        IDU_O_LSUopcode = Output(UInt(4.W))
        IDU_O_snpcISdnpc = Output(Bool())
        IDU_O_enableEXU = Output(Bool())
        IDU_O_enableLSU = Output(Bool())
    })
    io.IDU_O_src1 := io.IDU_I_src1
    io.IDU_O_src2 := io.IDU_I_src2
}

class EXU extends Module{
    val io = IO(new Bundle{
        EXU_I_src1 = Input(UInt(64.W))
        EXU_I_src2 = Input(UInt(64.W))
        EXU_I_opcode = Input(UInt(4.W))
        EXU_I_enableEXU = Input(Bool())
        EXU_O_result = Output(UInt(64.W))
        EXU_O_carry = Output(UInt(1.W))
        EXU_O_overflow = Output(UInt(1.W))
        EXU_O_snpcNEQdnpc = Output(Bool())
    })
}

class LSU extends Module{
    val io = IO(new Bundle{
        LSU_I_src1 = Input(UInt(64.W))
        LSU_I_src2 = Input(UInt(64.W))
        LSU_I_EXUresult = Input(UInt(64.W))
        LSU_I_opcode = Input(UInt(4.W))
        LSU_I_enableLSU = Input(Bool())
        // Following signals are pulled to top
        LSU_O_memAddr = Output(UInt(64.W))
        LSU_O_memRW = Output(Bool()) // Low: Read, High: Write
        LSU_I_memR = Input(UInt(64.W))
        LSU_I_memW = Output(UInt(64.W))
    })
}

class WBU extends Module{
    val io = IO(new Bundle{
        WBU_I_EXUresult = Input(UInt(64.W))
        WBU_I_EXUsnpcNEQdnpc = Input(Bool())
        WBU_I_IDUsnpcISdnpc = Input(Bool())
        WBU_I_rd = Input(Bool())
        WBU_O_nextPC = Output(UInt(64.W))
        WBU_O_GPRidx = Output(UInt(5.W))
        WBU_O_GPRWriteBack = Output(UInt(64.W))
    })
}

class NPCB extends Module{
    val io = IO(new Bundle{
        NPC_sendCurrentPC = Output(UInt(64.W))
        NPC_getInst = Input(UInt(32.W))
        NPC_LSU_O_memAddr = Output(UInt(64.W))
        NPC_LSU_O_memRW = Output(Bool()) // Low: Read, High: Write
        NPC_LSU_I_memR = Input(UInt(64.W))
        NPC_LSU_I_memW = Output(UInt(64.W))
    })

    val PC = RegInit(0.U(64.W))
    val GPR = Mem(32, UInt(64.W))
    def GPR_read (index: UInt) = Mux(index === 0.U, 0.U(64.W), GPR(index)) // So that we make sure R(0) is always 0

    val npcb_IFU = Module(new IFU)
    npcb_IFU.io.IFU_I_inst := io.NPC_getInst
    npcb_IFU.io.IFU_I_PC := PC

    val npcb_IDU = Module(new IDU)
    npcb_IDU.io.IDU_I_inst := npcb_IFU.io.IFU_I_inst
}