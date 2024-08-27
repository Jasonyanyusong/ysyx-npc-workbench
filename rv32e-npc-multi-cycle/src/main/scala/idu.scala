package npc

import chisel3._
import chisel3.util._

object NPC_IDU_Insts {
    // Non-Priv
    def LUI    = BitPat("b???????_?????_?????_???_?????_01101_11")
    def AUIPC  = BitPat("b???????_?????_?????_???_?????_00101_11")
    def JAL    = BitPat("b???????_?????_?????_???_?????_11011_11")
    def JALR   = BitPat("b???????_?????_?????_000_?????_11001_11")
    def BEQ    = BitPat("b???????_?????_?????_000_?????_11000_11")
    def BNE    = BitPat("b???????_?????_?????_001_?????_11000_11")
    def BLT    = BitPat("b???????_?????_?????_100_?????_11000_11")
    def BGE    = BitPat("b???????_?????_?????_101_?????_11000_11")
    def BLTU   = BitPat("b???????_?????_?????_110_?????_11000_11")
    def BGEU   = BitPat("b???????_?????_?????_111_?????_11000_11")
    def LB     = BitPat("b???????_?????_?????_000_?????_00000_11")
    def LH     = BitPat("b???????_?????_?????_001_?????_00000_11")
    def LW     = BitPat("b???????_?????_?????_010_?????_00000_11")
    def LBU    = BitPat("b???????_?????_?????_100_?????_00000_11")
    def LHU    = BitPat("b???????_?????_?????_101_?????_00000_11")
    def SB     = BitPat("b???????_?????_?????_000_?????_01000_11")
    def SH     = BitPat("b???????_?????_?????_001_?????_01000_11")
    def SW     = BitPat("b???????_?????_?????_010_?????_01000_11")
    def ADDI   = BitPat("b???????_?????_?????_000_?????_00100_11")
    def SLTI   = BitPat("b???????_?????_?????_010_?????_00100_11")
    def SLTIU  = BitPat("b???????_?????_?????_011_?????_00100_11")
    def XORI   = BitPat("b???????_?????_?????_100_?????_00100_11")
    def ORI    = BitPat("b???????_?????_?????_110_?????_00100_11")
    def ANDI   = BitPat("b???????_?????_?????_111_?????_00100_11")
    def SLLI   = BitPat("b000000?_?????_?????_001_?????_00100_11")
    def SRLI   = BitPat("b000000?_?????_?????_101_?????_00100_11")
    def SRAI   = BitPat("b010000?_?????_?????_101_?????_00100_11")
    def ADD    = BitPat("b0000000_?????_?????_000_?????_01100_11")
    def SUB    = BitPat("b0100000_?????_?????_000_?????_01100_11")
    def SLL    = BitPat("b0000000_?????_?????_001_?????_01100_11")
    def SLT    = BitPat("b0000000_?????_?????_010_?????_01100_11")
    def SLTU   = BitPat("b0000000_?????_?????_011_?????_01100_11")
    def XOR    = BitPat("b0000000_?????_?????_100_?????_01100_11")
    def SRL    = BitPat("b0000000_?????_?????_101_?????_01100_11")
    def SRA    = BitPat("b0100000_?????_?????_101_?????_01100_11")
    def OR     = BitPat("b0000000_?????_?????_110_?????_01100_11")
    def AND    = BitPat("b0000000_?????_?????_111_?????_01100_11")

    // Priv
    def ECALL  = BitPat("b0000000_00000_00000_000_00000_11100_11")
    def EBREAK = BitPat("b0000000_00001_00000_000_00000_11100_11")
    def MRET   = BitPat("b0011000_00010_00000_000_00000_11100_11")

    // Zicsr
    def CSRRW  = BitPat("b???????_?????_?????_001_?????_11100_11")
    def CSRRS  = BitPat("b???????_?????_?????_010_?????_11100_11")
    def CSRRC  = BitPat("b???????_?????_?????_011_?????_11100_11")
    def CSRRWI = BitPat("b???????_?????_?????_101_?????_11100_11")
    def CSRRSI = BitPat("b???????_?????_?????_110_?????_11100_11")
    def CSRRCI = BitPat("b???????_?????_?????_111_?????_11100_11")
}

object NPC_IDU_InstTypes {
    val R = 0.U(3.W)
    val I = 1.U(3.W)
    val S = 2.U(3.W)
    val B = 3.U(3.W)
    val U = 4.U(3.W)
    val J = 5.U(3.W)
}

object NPC_IDU_EX_Ops {
    def EX_NOP    =  0.U(4.W)
    def EX_PS1    =  1.U(4.W)
    def EX_ADD    =  2.U(4.W)
    def EX_SLT    =  3.U(4.W)
    def EX_SLTU   =  4.U(4.W)
    def EX_XOR    =  5.U(4.W)
    def EX_OR     =  6.U(4.W)
    def EX_AND    =  7.U(4.W)
    def EX_SLL    =  8.U(4.W)
    def EX_SRL    =  9.U(4.W)
    def EX_SRA    = 10.U(4.W)
    def EX_SUB    = 11.U(4.W)
}

class NPC_IDU_Internal_IO extends Bundle {
    val idu_internal_inst_i = Input(UInt(32.W))
    val idu_internal_work_i = Input(Bool())

    val idu_internal_rs1_o = Output(UInt(4.W)) // rv32e only 16 registers
    val idu_internal_rs1_val_i = Input(UInt(32.W))

    val idu_internal_rs2_o = Output(UInt(4.W)) // rv32e only 16 registers
    val idu_internal_rs2_val_i = Input(UInt(32.W))

    val idu_internal_exu_opcode_o = Output(UInt(4.W))
    val idu_internal_exu_src1_o = Output(UInt(32.W))
    val idu_internal_exu_src2_o = Output(UInt(32.W))

    val idu_internal_lsu_opcode_o = Output(UInt(4.W))
    //val idu_internal_lsu_addr_o = Output(UInt(32.W))
    val idu_internal_lsu_wdata_o = Output(UInt(32.W))

    val idu_internal_wbu_opcode_o = Output(UInt(2.W))
    val idu_internal_wbu_rd_o = Output(UInt(4.W))

    val idu_internal_pc_i = Input(UInt(32.W))
    val idu_internal_dnpc_o = Output(UInt(32.W))

    val idu_internal_valid_o = Output(Bool())
    val idu_internal_ebreak_o = Output(Bool())
}

class NPC_IDU extends Module {
    // IO definitions
    val idu_internal_io = IO(new NPC_IDU_Internal_IO)

    // CSRs
    val CSR_mcause  = RegInit(0.U(32.W))
    val CSR_mepc    = RegInit(0.U(32.W))
    val CSR_mstatus = RegInit(0.U(32.W))
    val CSR_mtvec   = RegInit(0.U(32.W))

    // decode - instruction type
    val InstType = Lookup(idu_internal_io.idu_internal_inst_i, NPC_IDU_InstTypes.R, Array(
        NPC_IDU_Insts.LUI    -> NPC_IDU_InstTypes.U,
        NPC_IDU_Insts.AUIPC  -> NPC_IDU_InstTypes.U,

        NPC_IDU_Insts.JAL    -> NPC_IDU_InstTypes.J,

        NPC_IDU_Insts.JALR   -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.LB     -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.LH     -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.LW     -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.LBU    -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.LHU    -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.ADDI   -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.SLTI   -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.SLTIU  -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.XORI   -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.ORI    -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.ANDI   -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.SLLI   -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.SRLI   -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.SRAI   -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.CSRRW  -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.CSRRS  -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.CSRRC  -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.CSRRWI -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.CSRRSI -> NPC_IDU_InstTypes.I,
        NPC_IDU_Insts.CSRRCI -> NPC_IDU_InstTypes.I,

        NPC_IDU_Insts.ADD    -> NPC_IDU_InstTypes.R,
        NPC_IDU_Insts.SUB    -> NPC_IDU_InstTypes.R,
        NPC_IDU_Insts.SLL    -> NPC_IDU_InstTypes.R,
        NPC_IDU_Insts.SLT    -> NPC_IDU_InstTypes.R,
        NPC_IDU_Insts.SLTU   -> NPC_IDU_InstTypes.R,
        NPC_IDU_Insts.XOR    -> NPC_IDU_InstTypes.R,
        NPC_IDU_Insts.SRL    -> NPC_IDU_InstTypes.R,
        NPC_IDU_Insts.SRA    -> NPC_IDU_InstTypes.R,
        NPC_IDU_Insts.OR     -> NPC_IDU_InstTypes.R,
        NPC_IDU_Insts.AND    -> NPC_IDU_InstTypes.R,

        NPC_IDU_Insts.SB     -> NPC_IDU_InstTypes.S,
        NPC_IDU_Insts.SH     -> NPC_IDU_InstTypes.S,
        NPC_IDU_Insts.SW     -> NPC_IDU_InstTypes.S,

        NPC_IDU_Insts.BEQ    -> NPC_IDU_InstTypes.B,
        NPC_IDU_Insts.BNE    -> NPC_IDU_InstTypes.B,
        NPC_IDU_Insts.BLT    -> NPC_IDU_InstTypes.B,
        NPC_IDU_Insts.BGE    -> NPC_IDU_InstTypes.B,
        NPC_IDU_Insts.BLTU   -> NPC_IDU_InstTypes.B,
        NPC_IDU_Insts.BGEU   -> NPC_IDU_InstTypes.B,
    ))

    // decode - assign immediate number
    val immI = ioSubmodule.iInst(31, 20).asUInt
    val immS = Cat(
        Seq(
            ioSubmodule.iInst(31, 25).asUInt,
            ioSubmodule.iInst(11,  7).asUInt
        )
    )
    val immB = Cat(
        Seq(
            ioSubmodule.iInst(31, 31).asUInt,
            ioSubmodule.iInst( 7,  7).asUInt,
            ioSubmodule.iInst(30, 25).asUInt,
            ioSubmodule.iInst(11,  8).asUInt,
            Fill(1, 0.U(1.W)).asUInt
        )
    )
    val immU = Cat(
        Seq(
            ioSubmodule.iInst(31, 12).asUInt,
            Fill(12, 0.U(1.W)).asUInt
        )
    )
    val immJ = Cat(
        Seq(
            ioSubmodule.iInst(31, 31).asUInt,
            ioSubmodule.iInst(19, 12).asUInt,
            ioSubmodule.iInst(20, 20).asUInt,
            ioSubmodule.iInst(30, 21).asUInt,
            Fill(1, 0.U(1.W)).asUInt
        )
    )

    val imm = MuxCase(0.U(32.W),
        Array(
            (InstType === NPC_IDU_InstTypes.I) -> Cat(Fill(20, immI(11)), immI),
            (InstType === NPC_IDU_InstTypes.S) -> Cat(Fill(20, immS(11)), immS),
            (InstType === NPC_IDU_InstTypes.B) -> Cat(Fill(19, immB(12)), immB),
            (InstType === NPC_IDU_InstTypes.U) -> immU,
            (InstType === NPC_IDU_InstTypes.J) -> Cat(Fill(11, immJ(20)), immJ),
        )
    )

    // decode - access registers
    idu_internal_io.idu_internal_rs1_o := idu_internal_io.idu_internal_inst_i(18, 15)
    idu_internal_io.idu_internal_rs2_o := idu_internal_io.idu_internal_inst_i(23, 20)
    val IDU_RS1_Val = idu_internal_io.idu_internal_rs1_val_i
    val IDU_RS2_Val = idu_internal_io.idu_internal_rs2_val_i

    // decode - exu
    idu_internal_io.idu_internal_exu_opcode_o := Lookup(idu_internal_io.idu_internal_inst_i, NPC_IDU_EX_Ops.EX_NOP, Array(
        NPC_IDU_Insts.LUI    -> NPC_IDU_EX_Ops.EX_PS1,
        NPC_IDU_Insts.AUIPC  -> NPC_IDU_EX_Ops.EX_ADD,
        NPC_IDU_Insts.JAL    -> NPC_IDU_EX_Ops.EX_ADD,
        NPC_IDU_Insts.JALR   -> NPC_IDU_EX_Ops.EX_ADD,
        NPC_IDU_Insts.BEQ    -> NPC_IDU_EX_Ops.EX_NOP,
        NPC_IDU_Insts.BNE    -> NPC_IDU_EX_Ops.EX_NOP,
        NPC_IDU_Insts.BLT    -> NPC_IDU_EX_Ops.EX_NOP,
        NPC_IDU_Insts.BGE    -> NPC_IDU_EX_Ops.EX_NOP,
        NPC_IDU_Insts.BLTU   -> NPC_IDU_EX_Ops.EX_NOP,
        NPC_IDU_Insts.BGEU   -> NPC_IDU_EX_Ops.EX_NOP,
        NPC_IDU_Insts.LB     -> NPC_IDU_EX_Ops.EX_ADD,
        NPC_IDU_Insts.LH     -> NPC_IDU_EX_Ops.EX_ADD,
        NPC_IDU_Insts.LW     -> NPC_IDU_EX_Ops.EX_ADD,
        NPC_IDU_Insts.LBU    -> NPC_IDU_EX_Ops.EX_ADD,
        NPC_IDU_Insts.LHU    -> NPC_IDU_EX_Ops.EX_ADD,
        NPC_IDU_Insts.SB     -> NPC_IDU_EX_Ops.EX_ADD,
        NPC_IDU_Insts.SH     -> NPC_IDU_EX_Ops.EX_ADD,
        NPC_IDU_Insts.SW     -> NPC_IDU_EX_Ops.EX_ADD,
        NPC_IDU_Insts.ADDI   -> NPC_IDU_EX_Ops.EX_ADD,
        NPC_IDU_Insts.SLTI   -> NPC_IDU_EX_Ops.EX_SLT,
        NPC_IDU_Insts.SLTIU  -> NPC_IDU_EX_Ops.EX_SLTU,
        NPC_IDU_Insts.XORI   -> NPC_IDU_EX_Ops.EX_XOR,
        NPC_IDU_Insts.ORI    -> NPC_IDU_EX_Ops.EX_OR,
        NPC_IDU_Insts.ANDI   -> NPC_IDU_EX_Ops.EX_AND,
        NPC_IDU_Insts.SLLI   -> NPC_IDU_EX_Ops.EX_SLL,
        NPC_IDU_Insts.SRLI   -> NPC_IDU_EX_Ops.EX_SRL,
        NPC_IDU_Insts.SRAI   -> NPC_IDU_EX_Ops.EX_SRA,

        NPC_IDU_Insts.ADD    -> NPC_IDU_EX_Ops.EX_ADD,
        NPC_IDU_Insts.SUB    -> NPC_IDU_EX_Ops.EX_SUB,
    ))

    idu_internal_io.idu_internal_exu_src1_o := Lookup(idu_internal_io.idu_internal_inst_i, 0.U(32.W), Array(
        NPC_IDU_Insts.LUI    -> imm,
        NPC_IDU_Insts.AUIPC  -> idu_internal_io.idu_internal_pc_i,
        NPC_IDU_Insts.JAL    -> idu_internal_io.idu_internal_pc_i,
        NPC_IDU_Insts.JALR   -> idu_internal_io.idu_internal_pc_i,
        NPC_IDU_Insts.BEQ    -> 0.U(32.W),
        NPC_IDU_Insts.BNE    -> 0.U(32.W),
        NPC_IDU_Insts.BLT    -> 0.U(32.W),
        NPC_IDU_Insts.BGE    -> 0.U(32.W),
        NPC_IDU_Insts.BLTU   -> 0.U(32.W),
        NPC_IDU_Insts.BGEU   -> 0.U(32.W),
        NPC_IDU_Insts.LB     -> IDU_RS1_Val,
        NPC_IDU_Insts.LH     -> IDU_RS1_Val,
        NPC_IDU_Insts.LW     -> IDU_RS1_Val,
        NPC_IDU_Insts.LBU    -> IDU_RS1_Val,
        NPC_IDU_Insts.LHU    -> IDU_RS1_Val,
        NPC_IDU_Insts.SB     -> IDU_RS1_Val,
        NPC_IDU_Insts.SH     -> IDU_RS1_Val,
        NPC_IDU_Insts.SW     -> IDU_RS1_Val,
        NPC_IDU_Insts.ADDI   -> IDU_RS1_Val,
        NPC_IDU_Insts.SLTI   -> IDU_RS1_Val,
        NPC_IDU_Insts.SLTIU  -> IDU_RS1_Val,
        NPC_IDU_Insts.XORI   -> IDU_RS1_Val,
        NPC_IDU_Insts.ORI    -> IDU_RS1_Val,
        NPC_IDU_Insts.ANDI   -> IDU_RS1_Val,
        NPC_IDU_Insts.SLLI   -> IDU_RS1_Val,
        NPC_IDU_Insts.SRLI   -> IDU_RS1_Val,
        NPC_IDU_Insts.SRAI   -> IDU_RS1_Val,
    ))

    idu_internal_io.idu_internal_exu_src2_o := Lookup(idu_internal_io.idu_internal_inst_i, 0.U(32.W), Array(
        NPC_IDU_Insts.LUI    -> 0.U(32.W),
        NPC_IDU_Insts.AUIPC  -> imm,
        NPC_IDU_Insts.JAL    -> 4.U(32.W),
        NPC_IDU_Insts.JALR   -> 4.U(32.W),
        NPC_IDU_Insts.BEQ    -> 0.U(32.W),
        NPC_IDU_Insts.BNE    -> 0.U(32.W),
        NPC_IDU_Insts.BLT    -> 0.U(32.W),
        NPC_IDU_Insts.BGE    -> 0.U(32.W),
        NPC_IDU_Insts.BLTU   -> 0.U(32.W),
        NPC_IDU_Insts.BGEU   -> 0.U(32.W),
        NPC_IDU_Insts.LB     -> imm,
        NPC_IDU_Insts.LH     -> imm,
        NPC_IDU_Insts.LW     -> imm,
        NPC_IDU_Insts.LBU    -> imm,
        NPC_IDU_Insts.LHU    -> imm,
        NPC_IDU_Insts.SB     -> imm,
        NPC_IDU_Insts.SH     -> imm,
        NPC_IDU_Insts.SW     -> imm,
        NPC_IDU_Insts.ADDI   -> imm,
        NPC_IDU_Insts.SLTI   -> imm,
        NPC_IDU_Insts.SLTIU  -> imm,
        NPC_IDU_Insts.XORI   -> imm,
        NPC_IDU_Insts.ORI    -> imm,
        NPC_IDU_Insts.ANDI   -> imm,
        NPC_IDU_Insts.SLLI   -> (imm & "b11111".U),
        NPC_IDU_Insts.SRLI   -> (imm & "b11111".U),
        NPC_IDU_Insts.SRAI   -> (imm & "b11111".U),
    ))

    // decode - lsu

    // decode - wbu

    // decode - dnpc generation
    idu_internal_io.idu_internal_dnpc_o := Lookup(idu_internal_io.idu_internal_inst_i, idu_internal_io.idu_internal_pc_i + 4.U, Array(
        // unconditional jump
        NPC_IDU_Insts.JAL    -> (idu_internal_io.idu_internal_pc_i + imm),
        NPC_IDU_Insts.JALR   -> ((idu_internal_io.idu_internal_pc_i + IDU_RS1_Val) & Cat(Fill(31, 1.U(1.W)), Fill(1, 0.U(1.W)))),
        
        // conditional jump
        NPC_IDU_Insts.BEQ    -> Mux(IDU_RS1_Val === IDU_RS2_Val,
            idu_internal_io.idu_internal_pc_i + imm,
            idu_internal_io.idu_internal_pc_i + 4.U
        ),
        NPC_IDU_Insts.BNE    -> Mux(IDU_RS1_Val =/= IDU_RS2_Val,
            idu_internal_io.idu_internal_pc_i + imm,
            idu_internal_io.idu_internal_pc_i + 4.U
        ),
        NPC_IDU_Insts.BLT    -> Mux(IDU_RS1_Val.asSInt < IDU_RS2_Val.asSInt,
            idu_internal_io.idu_internal_pc_i + imm,
            idu_internal_io.idu_internal_pc_i + 4.U
        ),
        NPC_IDU_Insts.BGE    -> Mux(IDU_RS1_Val.asSInt >= IDU_RS2_Val.asSInt,
            idu_internal_io.idu_internal_pc_i + imm,
            idu_internal_io.idu_internal_pc_i + 4.U
        ),
        NPC_IDU_Insts.BLTU   -> Mux(IDU_RS1_Val.asUInt < IDU_RS2_Val.asUInt,
            idu_internal_io.idu_internal_pc_i + imm,
            idu_internal_io.idu_internal_pc_i + 4.U
        ),
        NPC_IDU_Insts.BGEU   -> Mux(IDU_RS1_Val.asUInt >= IDU_RS2_Val.asUInt,
            idu_internal_io.idu_internal_pc_i + imm,
            idu_internal_io.idu_internal_pc_i + 4.U
        ),

        // priv jumps
        NPC_IDU_Insts.ECALL  -> CSR_mtvec,
        NPC_IDU_Insts.MRET   -> CSR_mepc,
    ))

    // decode - csr updates - deal with zicsr instructions, not ecall or mret

    // connect
    idu_internal_io.idu_internal_valid_o := idu_internal_io.idu_internal_work_i // idu is comb logic only
    idu_internal_io.idu_internal_ebreak_o := Lookup(idu_internal_io.idu_internal_inst_i, false.B, Array(
        NPC_IDU_Insts.EBREAK    -> true.B
    )) // if ebreak, stop everything in cpu
}