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
        IDU_O_ModifyMem = Output(Bool())
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
        IDU_O_GPRneedWriteBack = Output(Bool())
        IDU_O_imm = Output(UInt(64.W))
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

class PCU extends Module{
    val io = IO(new Bundle{
        PCU_I_CurrentPC = Input(UInt(64.W))
        PCU_I_imm = Input(UInt(64.W))
        PCU_I_willJump = Input(Bool())
        PCU_O_NextPC = Output(UInt(64.W))
    })
    io.PCU_O_NextPC := Mux(io.PCU_I_willJump, io.PCU_I_CurrentPC + io.PCU_I_imm, io.PCU_I_CurrentPC + 4.U)
}

class LSU extends Module{
    val io = IO(new Bundle{
        LSU_I_src1 = Input(UInt(64.W))
        LSU_I_src2 = Input(UInt(64.W))
        LSU_I_EXUresult = Input(UInt(64.W))
        LSU_I_opcode = Input(UInt(4.W))
        LSU_I_enableLSU = Input(Bool())
        LSU_I_ModifyMem = Input(Bool())
        LSU_O_result = Output(UInt(64.W))
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
        WBU_I_LSUresult = Input(UInt(64.W))
        WBU_I_LSUenable = Input(Bool())
        WBU_I_rd = Input(UInt(5.W))
        WBU_O_GPRidx = Output(UInt(5.W))
        WBU_O_GPRWriteBack = Output(UInt(64.W))
    })
    io.WBU_O_GPRidx := io.WBU_I_rd
    Mux(io.WBU_I_LSUenable, io.WBU_O_GPRWriteBack := io.WBU_I_LSUresult, io.WBU_O_GPRWriteBack := io.WBU_I_EXUresult)
    // Mux(io.WBU_I_IDUsnpcISdnpc, io.WBU_O_nextPC := io.WBU_I_currentPC + 4.U, Mux(io.WBU_I_EXUsnpcNEQdnpc, io.WBU_O_nextPC := io.WBU_I_nextPC))
}

class NPCB extends Module{
    val io = IO(new Bundle{
        NPC_sendCurrentPC = Output(UInt(64.W))
        NPC_getInst = Input(UInt(32.W))
        NPC_LSU_O_memAddr = Output(UInt(64.W))
        NPC_LSU_O_memRW = Output(Bool()) // Low: Read, High: Write
        NPC_LSU_I_memR = Input(UInt(64.W))
        NPC_LSU_I_memW = Output(UInt(64.W))

        NPC_GPRchanged = Output(Bool())
    })

    val PC = RegInit(0.U(64.W))
    val GPR = Mem(32, UInt(64.W))
    def GPR_read (index: UInt) = Mux(index === 0.U, 0.U(64.W), GPR(index)) // So that we make sure R(0) is always 0

    // Step I: Fetch one piece of instruction from Mem
    val npcb_IFU = Module(new IFU)
    npcb_IFU.io.IFU_I_inst := io.NPC_getInst
    npcb_IFU.io.IFU_I_PC := PC

    // Step II: Decode the instruction: 1) generate OPCODE for LSU and EXU 2) cut RS1, RS2 and RD 3) get SRC1 and SRC2 from GPR 4) Decide if this instruction may cause PC jump
    val npcb_IDU = Module(new IDU)
    npcb_IDU.io.IDU_I_inst := npcb_IFU.io.IFU_I_inst
    npcb_IDU.io.IDU_I_src1 := GPR_read(npcb_IDU.io.IDU_O_rs1)
    npcb_IDU.io.IDU_I_src2 := GPR_read(npcb_IDU.io.IDU_O_rs2)

    // Step III: ALU execution
    val npcb_EXU = Module(new EXU)
    npcb_EXU.io.EXU_I_src1 := npcb_IDU.io.IDU_O_src1
    npcb_EXU.io.EXU_I_src2 := npcb_IDU.io.IDU_O_src2
    npcb_EXU.io.EXU_I_opcode := npcb_IDU.io.IDU_O_EXUopcode
    npcb_EXU.io.EXU_I_enableEXU := npcb_IDU.IDU_O_enableEXU

    // Step IV: LSU execution (we access memory using Verilator, NEMU and AM's debug interfaces)
    val npcb_LSU = Module(new LSU)
    npcb_LSU.io.LSU_I_src1 := npcb_IDU.io.IDU_O_src1
    npcb_LSU.io.LSU_I_src2 := npcb_IDU.io.IDU_O_src2
    npcb_LSU.io.LSU_I_EXUresult := npcb_EXU.io.EXU_O_result
    npcb_LSU.io.LSU_I_ModifyMem := npcb_IDU.io.IDU_O_ModifyMem
    npcb_LSU.io.LSU_I_enableLSU := npcb_IDU.io.IDU_O_enableLSU
    npcb_LSU.io.LSU_I_opcode := npcb_IDU.io.IDU_O_LSUopcode

    // Step V: Write back data to a GPR and PC
    val npcb_WBU = Module(new WBU)
    val npcb_PCU = Module(new PCU)
    npcb_WBU.io.WBU_I_EXUresult := npcb_EXU.io.EXU_O_result
    npcb_WBU.io.WBU_I_LSUresult := npcb_LSU.io.LSU_O_result
    npcb_WBU.io.WBU_I_EXUsnpcNEQdnpc := npcb_EXU.io.EXU_O_snpcNEQdnpc
    npcb_PCU.io.PCU_I_CurrentPC := PC
    npcb_PCU.io.PCU_I_willJump := npcb_IDU.io.IDU_O_snpcISdnpc && npcb_EXU.io.EXU_O_snpcNEQdnpc
    npcb_PCU.io.PCU_I_imm := npcb_IDU.io.IDU_O_imm
    npcb_WBU.io.WBU_I_rd := npcb_IDU.io.IDU_O_rd
    npcb_WBU.io.WBU_I_IDUsnpcISdnpc := npcb_IDU.io.IDU_O_snpcISdnpc
    NPC_GPRchanged := true.B
    Mux(npcb_IDU.io.IDU_O_GPRneedWriteBack, GPR(npcb_WBU.io.WBU_O_GPRidx) := npcb_WBU.io.WBU_O_GPRWriteBack, NPC_GPRchanged := false.B)
    PC := npcb_PCU.io.PCU_O_NextPC
}