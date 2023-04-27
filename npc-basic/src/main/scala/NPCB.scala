package npcb
import chisel3._
import chisel3.util._

object inst_types{
    def inst_R = BitPat("b000")
    def inst_I = BitPat("b001")
    def inst_S = BitPat("b010")
    def inst_B = BitPat("b011")
    def inst_U = BitPat("b100")
    def inst_J = BitPat("b101")
}

object EXU_opcode{
    // EXU needs to do something for every instruction except EBREAK
    def EXU_DoNothing = BitPat("b000000")
    // def EXU_LUI       = BitPat("b000001")
    def EXU_AUIPC     = BitPat("b000010")
    def EXU_JAL       = BitPat("b000011")
    def EXU_JALR      = BitPat("b000100")
    def EXU_BEQ       = BitPat("b000101")
    def EXU_BNE       = BitPat("b000110")
    def EXU_BGE       = BitPat("b000111")
    def EXU_BLTU      = BitPat("b001000")
    def EXU_BGEU      = BitPat("b001001")
    def EXU_LB        = BitPat("b001010")
    def EXU_LH        = BitPat("b001011")
    def EXU_LW        = BitPat("b001100")
    def EXU_LBU       = BitPat("b001101")
    def EXU_LHU       = BitPat("b001110")
    def EXU_SB        = BitPat("b001111")
    def EXU_SH        = BitPat("b010000")
    def EXU_SW        = BitPat("b010001")
    def EXU_ADDI      = BitPat("b010010")
    def EXU_SLTI      = BitPat("b010011")
    def EXU_SLTIU     = BitPat("b010100")
    def EXU_XORI      = BitPat("b010101")
    def EXU_ORI       = BitPat("b010110")
    def EXU_ANDI      = BitPat("b010111")
    def EXU_SLLI      = BitPat("b011000")
    def EXU_SRLI      = BitPat("b011001")
    def EXU_SRAI      = BitPat("b011010")
    def EXU_ADD       = BitPat("b011011")
    def EXU_SUB       = BitPat("b011100")
    def EXU_SLL       = BitPat("b011101")
    def EXU_SLT       = BitPat("b011110")
    def EXU_SLTU      = BitPat("b011111")
    def EXU_XOR       = BitPat("b100000")
    def EXU_SRL       = BitPat("b100001")
    def EXU_SRA       = BitPat("b100010")
    def EXU_OR        = BitPat("b100011")
    def EXU_AND       = BitPat("b100100")
    def EXU_LWU       = BitPat("b100101")
    def EXU_LD        = BitPat("b100110")
    def EXU_SD        = BitPat("b100111")
    def EXU_ADDIW     = BitPat("b101000")
    def EXU_SLLIW     = BitPat("b101001")
    def EXU_SRLIW     = BitPat("b101010")
    def EXU_SRAIW     = BitPat("b101011")
    def EXU_ADDW      = BitPat("b101100")
    def EXU_SUBW      = BitPat("b101101")
    def EXU_SLLW      = BitPat("b101110")
    def EXU_SRLW      = BitPat("b101111")
    def EXU_SRAW      = BitPat("b110000")
    def EXU_MUL       = BitPat("b110001")
    def EXU_MULH      = BitPat("b110010")
    def EXU_MULHSU    = BitPat("b110011")
    def EXU_MULHU     = BitPat("b110100")
    def EXU_DIV       = BitPat("b110101")
    def EXU_DIVU      = BitPat("b110110")
    def EXU_REM       = BitPat("b110111")
    def EXU_REMU      = BitPat("b111000")
    def EXU_MULW      = BitPat("b111001")
    def EXU_DIVW      = BitPat("b111010")
    def EXU_DIVUW     = BitPat("b111011")
    def EXU_REMW      = BitPat("b111100")
    def EXU_REMUW     = BitPat("b111101")
}

object LSU_opcode{
    def LSU_DoNothing = BitPat("b000000")
    def LSU_LB        = BitPat("b000001")
    def LSU_LBU       = BitPat("b000010")
    def LSU_LH        = BitPat("b000011")
    def LSU_LHU       = BitPat("b000100")
    def LSU_LW        = BitPat("b000101")
    def LSU_LD        = BitPat("b000110")
    def LSU_SB        = BitPat("b000111")
    def LSU_SH        = BitPat("b001000")
    def LSU_SW        = BitPat("b001001")
    def LSU_SD        = BitPat("b001010")
}

object RV_Inst{
    // All Insts except EBREAK, LUI requrie EXU
    // RV64I Instructions
    def LUI       = BitPat("b???????_?????_?????_???_?????_01101_11") // U
    def AUIPC     = BitPat("b???????_?????_?????_???_?????_00101_11") // U
    def JAL       = BitPat("b???????_?????_?????_???_?????_11011_11") // J
    def JALR      = BitPat("b???????_?????_?????_000_?????_11001_11") // I
    def BEQ       = BitPat("b???????_?????_?????_000_?????_11000_11") // B
    def BNE       = BitPat("b???????_?????_?????_001_?????_11000_11") // B
    def BLT       = BitPat("b???????_?????_?????_100_?????_11000_11") // B
    def BGE       = BitPat("b???????_?????_?????_101_?????_11000_11") // B
    def BLTU      = BitPat("b???????_?????_?????_110_?????_11000_11") // B
    def BGEU      = BitPat("b???????_?????_?????_111_?????_11000_11") // B
    def LB        = BitPat("b???????_?????_?????_000_?????_00000_11") // I LSU
    def LH        = BitPat("b???????_?????_?????_001_?????_00000_11") // I LSU
    def LW        = BitPat("b???????_?????_?????_010_?????_00000_11") // I LSU
    def LBU       = BitPat("b???????_?????_?????_100_?????_00000_11") // I LSU
    def LHU       = BitPat("b???????_?????_?????_101_?????_00000_11") // I LSU
    def SB        = BitPat("b???????_?????_?????_000_?????_01000_11") // S LSU
    def SH        = BitPat("b???????_?????_?????_001_?????_01000_11") // S LSU
    def SW        = BitPat("b???????_?????_?????_010_?????_01000_11") // S LSU
    def ADDI      = BitPat("b???????_?????_?????_000_?????_00100_11") // I
    def SLTI      = BitPat("b???????_?????_?????_010_?????_00100_11") // I
    def SLTIU     = BitPat("b???????_?????_?????_011_?????_00100_11") // I
    def XORI      = BitPat("b???????_?????_?????_100_?????_00100_11") // I
    def ORI       = BitPat("b???????_?????_?????_110_?????_00100_11") // I
    def ANDI      = BitPat("b???????_?????_?????_111_?????_00100_11") // I
    def SLLI      = BitPat("b000000?_?????_?????_001_?????_00100_11") // I
    def SRLI      = BitPat("b000000?_?????_?????_101_?????_00100_11") // I
    def SRAI      = BitPat("b010000?_?????_?????_101_?????_00100_11") // I
    def ADD       = BitPat("b0000000_?????_?????_000_?????_01100_11") // R
    def SUB       = BitPat("b0100000_?????_?????_000_?????_01100_11") // R
    def SLL       = BitPat("b0000000_?????_?????_001_?????_01100_11") // R
    def SLT       = BitPat("b0000000_?????_?????_010_?????_01100_11") // R
    def SLTU      = BitPat("b0000000_?????_?????_011_?????_01100_11") // R
    def XOR       = BitPat("b0000000_?????_?????_100_?????_01100_11") // R
    def SRL       = BitPat("b0000000_?????_?????_101_?????_01100_11") // R
    def SRA       = BitPat("b0100000_?????_?????_101_?????_01100_11") // R
    def OR        = BitPat("b0000000_?????_?????_110_?????_01100_11") // R
    def AND       = BitPat("b0000000_?????_?????_111_?????_01100_11") // R
    def EBREAK    = BitPat("b0000000_00001_00000_000_00000_11100_11") // N
    def LWU       = BitPat("b???????_?????_?????_110_?????_00000_11") // I
    def LD        = BitPat("b???????_?????_?????_011_?????_00000_11") // I LSU
    def SD        = BitPat("b???????_?????_?????_011_?????_01000_11") // S LSU
    def ADDIW     = BitPat("b???????_?????_?????_000_?????_00110_11") // I
    def SLLIW     = BitPat("b0000000_?????_?????_001_?????_00110_11") // R
    def SRLIW     = BitPat("b0000000_?????_?????_101_?????_00110_11") // R
    def SRAIW     = BitPat("b0100000_?????_?????_101_?????_00110_11") // R
    def ADDW      = BitPat("b0000000_?????_?????_000_?????_01110_11") // R
    def SUBW      = BitPat("b0100000_?????_?????_000_?????_01110_11") // R
    def SLLW      = BitPat("b0000000_?????_?????_001_?????_01110_11") // R
    def SRLW      = BitPat("b0000000_?????_?????_101_?????_01110_11") // R
    def SRAW      = BitPat("b0100000_?????_?????_101_?????_01110_11") // R
    // RV64M Instructions
    def MUL       = BitPat("b0000001 ????? ????? 000 ????? 01100 11") // R
    def MULH      = BitPat("b0000001 ????? ????? 001 ????? 01100 11") // R
    def MULHSU    = BitPat("b0000001 ????? ????? 010 ????? 01100 11") // R
    def MULHU     = BitPat("b0000001 ????? ????? 011 ????? 01100 11") // R
    def DIV       = BitPat("b0000001 ????? ????? 100 ????? 01100 11") // R
    def DIVU      = BitPat("b0000001 ????? ????? 101 ????? 01100 11") // R
    def REM       = BitPat("b0000001 ????? ????? 110 ????? 01100 11") // R
    def REMU      = BitPat("b0000001 ????? ????? 111 ????? 01100 11") // R
    def MULW      = BitPat("b0000001 ????? ????? 000 ????? 01110 11") // R
    def DIVW      = BitPat("b0000001 ????? ????? 100 ????? 01110 11") // R
    def DIVUW     = BitPat("b0000001 ????? ????? 101 ????? 01110 11") // R
    def REMW      = BitPat("b0000001 ????? ????? 110 ????? 01110 11") // R
    def REMUW     = BitPat("b0000001 ????? ????? 111 ????? 01110 11") // R

}

class IFU extends Module{
    // Now we make IFU just a passthrough because we do not fetch instruction from memory directly
    val io = IO(new Bundle{
        IFU_I_PC = Input(UInt(64.W))
        IFU_O_PC = Output(UInt(64.W))
        IFU_I_inst = Input(UInt(32.W))
        IFU_O_inst = Output(UInt(32.W))
        IFU_O_error = Output(Bool())
    })
    io.IFU_O_inst := io.IFU_I_inst
    io.IFU_O_PC := io.IFU_I_PC
    io.IFU_O_error := false.B
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
        IDU_O_EXUopcode = Output(UInt(6.W))
        IDU_O_LSUopcode = Output(UInt(6.W))
        IDU_O_snpcISdnpc = Output(Bool())
        IDU_O_GPRneedWriteBack = Output(Bool())
        IDU_O_imm = Output(UInt(64.W))
        IDU_O_halt = Output(Bool())
        IDU_O_error = Output(Bool())
    })

    io.IDU_O_rs1 := io.IDU_I_inst(19, 15) // Cut rs1 from BITS(i, 19, 15)
    io.IDU_O_rs2 := io.IDU_I_inst(24, 20) // Cut rs2 from BITS(i, 24, 20)
    io.IDU_O_rd := io.IDU_I_inst(11, 7) // Cut rd from BITS(i, 11, 7)
    io.IDU_O_src1 := io.IDU_I_src1
    io.IDU_O_src2 := io.IDU_I_src2

    io.IDU_O_halt := false.B
    io.IDU_O_error := false.B

    val immI = io.IDU_I_inst(31, 20)
    val SignExtend_immI = Cat(Fill(52, immI(11)), immI)
    val immS = Cat(io.IDU_I_inst(31, 25), io.IDU_I_inst(11, 7))
    val SignExtend_immS = Cat(Fill(52, immS(11)), immS)
    val immB = Cat(io.IDU_I_inst(31, 31), io.IDU_I_inst(7, 7), io.IDU_I_inst(30, 25), io.IDU_I_inst(11, 8), 0.U)
    val SignExtend_immB = Cat(Fill(51, immB(12)), immB)
    val immU = Cat(io.IDU_I_inst(31, 12), Fill(12, 0.U))
    val SignExtend_immU = Cat(Fill(32, immU(31)), immU)
    val immJ = Cat(io.IDU_I_inst(31, 31), io.IDU_I_inst(19, 12), io.IDU_I_inst(20, 20), io.IDU_I_inst(30, 21), 0.U)
    val SignExtend_immJ = Cat(Fill(43, immJ(20)), immJ)
    val immR = 0.U
    val SignExtend_immR = Cat(Fill(63, immR(0)), immJ)
}

class EXU extends Module{
    val io = IO(new Bundle{
        EXU_I_src1 = Input(UInt(64.W))
        EXU_I_src2 = Input(UInt(64.W))
        EXU_I_opcode = Input(UInt(6.W))
        EXU_O_result = Output(UInt(64.W))
        EXU_O_carry = Output(UInt(1.W))
        EXU_O_overflow = Output(UInt(1.W))
        EXU_O_snpcNEQdnpc = Output(Bool())
        EXU_O_error = Output(Bool())
    })
    io.EXU_O_error := false.B
}

class PCU extends Module{
    val io = IO(new Bundle{
        PCU_I_CurrentPC = Input(UInt(64.W))
        PCU_I_imm = Input(UInt(64.W))
        PCU_I_willJump = Input(Bool())
        PCU_O_DynamicNextPC = Output(UInt(64.W))
        PCU_O_StaticNextPC = Output(UInt(64.W))
        PCU_O_error = Output(Bool())
    })
    io.PCU_O_StaticNextPC := io.PCU_I_CurrentPC + 4.U
    io.PCU_O_DynamicNextPC := Mux(io.PCU_I_willJump, io.PCU_I_CurrentPC + io.PCU_I_imm, io.PCU_I_CurrentPC + 4.U)
    io.PCU_O_error := false.B
}

class LSU extends Module{
    val io = IO(new Bundle{
        LSU_I_src1 = Input(UInt(64.W))
        LSU_I_src2 = Input(UInt(64.W))
        LSU_I_EXUresult = Input(UInt(64.W))
        LSU_I_opcode = Input(UInt(6.W))
        LSU_I_ModifyMem = Input(Bool())
        LSU_O_result = Output(UInt(64.W))
        // Following signals are pulled to top
        LSU_O_memAddr = Output(UInt(64.W))
        LSU_O_memRW = Output(Bool()) // Low: Read, High: Write
        LSU_I_memR = Input(UInt(64.W))
        LSU_I_memW = Output(UInt(64.W))
        LSU_O_error = Output(Bool())
    })
    io.LSU_O_error := false.B
}

class WBU extends Module{
    val io = IO(new Bundle{
        WBU_I_EXUresult = Input(UInt(64.W))
        WBU_I_LSUresult = Input(UInt(64.W))
        WBU_I_LSUenable = Input(Bool())
        WBU_I_rd = Input(UInt(5.W))
        WBU_O_GPRidx = Output(UInt(5.W))
        WBU_O_GPRWriteBack = Output(UInt(64.W))
        WBU_O_error = Output(Bool())
    })
    io.WBU_O_GPRidx := io.WBU_I_rd
    Mux(io.WBU_I_LSUenable, io.WBU_O_GPRWriteBack := io.WBU_I_LSUresult, io.WBU_O_GPRWriteBack := io.WBU_I_EXUresult)
    // Mux(io.WBU_I_IDUsnpcISdnpc, io.WBU_O_nextPC := io.WBU_I_currentPC + 4.U, Mux(io.WBU_I_EXUsnpcNEQdnpc, io.WBU_O_nextPC := io.WBU_I_nextPC))
    io.WBU_O_error := false.B
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
        NPC_halt = Output(Bool())
        NPC_error = Output(Bool())
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
    io.NPC_halt := npcb_IDU.io.IDU_O_halt

    // Step III: ALU execution
    val npcb_EXU = Module(new EXU)
    npcb_EXU.io.EXU_I_src1 := npcb_IDU.io.IDU_O_src1
    npcb_EXU.io.EXU_I_src2 := npcb_IDU.io.IDU_O_src2
    npcb_EXU.io.EXU_I_opcode := npcb_IDU.io.IDU_O_EXUopcode

    // Step IV: LSU execution (we access memory using Verilator, NEMU and AM's debug interfaces)
    val npcb_LSU = Module(new LSU)
    npcb_LSU.io.LSU_I_src1 := npcb_IDU.io.IDU_O_src1
    npcb_LSU.io.LSU_I_src2 := npcb_IDU.io.IDU_O_src2
    npcb_LSU.io.LSU_I_EXUresult := npcb_EXU.io.EXU_O_result
    npcb_LSU.io.LSU_I_ModifyMem := npcb_IDU.io.IDU_O_ModifyMem
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
    PC := npcb_PCU.io.PCU_O_DynamicNextPC
}