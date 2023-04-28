package npcb
import chisel3._
import chisel3.util._

object inst_types{
    def inst_R     = BitPat("b000")
    def inst_I     = BitPat("b001")
    def inst_S     = BitPat("b010")
    def inst_B     = BitPat("b011")
    def inst_U     = BitPat("b100")
    def inst_J     = BitPat("b101")
    def inst_N     = BitPat("b110")
    def inst_error = BitPat("b111")
}

object EXU_opcode{
    // EXU needs to do something for every instruction except EBREAK
    def EXU_DoNothing = BitPat("b000000")
    def EXU_LUI       = BitPat("b000001")
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
    def LSU_LWU       = BitPat("b001011")
    def LSU_LD        = BitPat("b000110")
    def LSU_SB        = BitPat("b000111")
    def LSU_SH        = BitPat("b001000")
    def LSU_SW        = BitPat("b001001")
    def LSU_SD        = BitPat("b001010")
}

object RV_Inst{
    // All Insts except EBREAK requrie EXU
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
        IDU_O_rs1 = Output(UInt(5.W)) // Same for all
        IDU_I_src1 = Input(UInt(64.W))
        IDU_O_src1 = Input(UInt(64.W)) // Same for all
        IDU_O_rs2 = Output(UInt(5.W)) // Same for all
        IDU_I_src2 = Input(UInt(64.W))
        IDU_O_src2 = Input(UInt(64.W)) // Same for all
        IDU_O_rd = Output(UInt(5.W)) // Same for all
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
    val SignExtend_immR = Cat(Fill(63, immR(0)), immJ) // When we found error in decoding, we will automatically return this imm value since it is 0, this will reduce the cause of bugs

    // We use a List called IDU_opcodes to record operats, the list have structure:
    // List(inst_type(15, 13), ModifyMem(12, 12), EXUopcode(11, 6), LSUopcode(9, 4), snpcISdnpc(3, 3), GPRneedWriteBack(2, 2), error(1, 1), halt(0, 0))
    // Here snpcISdnpc does not contain B type instructions even though they may be equal

    var IDU_opcodes = ListLookup(
        /*Compare Item: */io.IDU_I_inst,
        /*Default: */       List(inst_types.inst_error, 0.U, EXU_opcode.EXU_DoNothing, LSU_opcode.LSU_DoNothing, 1.U, 0.U, 1.U, 1.U),
        RV_Inst.LUI      -> List(inst_types.inst_U    , 0.U, EXU_opcode.EXU_LUI      , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.AUIPC    -> List(inst_types.inst_U    , 0.U, EXU_opcode.EXU_AUIPC    , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.JAL      -> List(inst_types.inst_J    , 0.U, EXU_opcode.EXU_JAL      , LSU_opcode.LSU_DoNothing, 0.U, 1.U, 0.U, 0.U),
        RV_Inst.JALR     -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_JALR     , LSU_opcode.LSU_DoNothing, 0.U, 1.U, 0.U, 0.U),
        RV_Inst.BEQ      -> List(inst_types.inst_B    , 0.U, EXU_opcode.EXU_BEQ      , LSU_opcode.LSU_DoNothing, 0.U, 0.U, 0.U, 0.U),
        RV_Inst.BNE      -> List(inst_types.inst_B    , 0.U, EXU_opcode.EXU_BNE      , LSU_opcode.LSU_DoNothing, 0.U, 0.U, 0.U, 0.U),
        RV_Inst.BLT      -> List(inst_types.inst_B    , 0.U, EXU_opcode.EXU_BLT      , LSU_opcode.LSU_DoNothing, 0.U, 0.U, 0.U, 0.U),
        RV_Inst.BGE      -> List(inst_types.inst_B    , 0.U, EXU_opcode.EXU_BGE      , LSU_opcode.LSU_DoNothing, 0.U, 0.U, 0.U, 0.U),
        RV_Inst.BLTU     -> List(inst_types.inst_B    , 0.U, EXU_opcode.EXU_BLTU     , LSU_opcode.LSU_DoNothing, 0.U, 0.U, 0.U, 0.U),
        RV_Inst.BGEU     -> List(inst_types.inst_B    , 0.U, EXU_opcode.EXU_BGEU     , LSU_opcode.LSU_DoNothing, 0.U, 0.U, 0.U, 0.U),
        RV_Inst.LB       -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_LB       , LSU_opcode.LSU_LB       , 1.U, 1.U, 0.U, 0.U),
        RV_Inst.LH       -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_LH       , LSU_opcode.LSU_LH       , 1.U, 1.U, 0.U, 0.U),
        RV_Inst.LW       -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_LW       , LSU_opcode.LSU_LW       , 1.U, 1.U, 0.U, 0.U),
        RV_Inst.LBU      -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_LBU      , LSU_opcode.LSU_LBU      , 1.U, 1.U, 0.U, 0.U),
        RV_Inst.LHU      -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_LHU      , LSU_opcode.LSU_LHU      , 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SB       -> List(inst_types.inst_S    , 1.U, EXU_opcode.EXU_SB       , LSU_opcode.LSU_SB       , 1.U, 0.U, 0.U, 0.U),
        RV_Inst.SH       -> List(inst_types.inst_S    , 1.U, EXU_opcode.EXU_SH       , LSU_opcode.LSU_SH       , 1.U, 0.U, 0.U, 0.U),
        RV_Inst.SW       -> List(inst_types.inst_S    , 1.U, EXU_opcode.EXU_SW       , LSU_opcode.LSU_SW       , 1.U, 0.U, 0.U, 0.U),
        RV_Inst.ADDI     -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_ADDI     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLTI     -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_SLTI     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLTIU    -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_SLTIU    , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.XORI     -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_XORI     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.ORI      -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_ORI      , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.ANDI     -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_ANDI     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLLI     -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_SLLI     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SRLI     -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_SRLI     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SRAI     -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_SRAI     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.ADD      -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_ADD      , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SUB      -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_SUB      , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLL      -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_SLL      , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLT      -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_SLT      , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLTU     -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_SLTU     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.XOR      -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_XOR      , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SRL      -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_SRL      , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SRA      -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_SRA      , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.OR       -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_OR       , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.AND      -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_AND      , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.EBREAK   -> List(inst_types.inst_N    , 0.U, EXU_opcode.EXU_DoNothing, LSU_opcode.LSU_DoNothing, 1.U, 0.U, 0.U, 1.U), // We set snpc === dnpc for EBREAK
        RV_Inst.LWU      -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_LWU      , LSU_opcode.LSU_LWU      , 1.U, 1.U, 0.U, 0.U),
        RV_Inst.LD       -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_LD       , LSU_opcode.LSU_LD       , 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SD       -> List(inst_types.inst_S    , 1.U, EXU_opcode.EXU_SD       , LSU_opcode.LSU_SD       , 1.U, 0.U, 0.U, 0.U),
        RV_Inst.ADDIW    -> List(inst_types.inst_I    , 0.U, EXU_opcode.EXU_ADDIW    , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLLIW    -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_SLLIW    , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SRLIW    -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_SRLIW    , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SRAIW    -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_SRAIW    , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.ADDW     -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_ADDW     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.ADDIW    -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_ADDIW    , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SUBW     -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_SUBW     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SLLW     -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_SLLW     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.SRAW     -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_SRAW     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.MUL      -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_MUL      , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.MULH     -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_MULH     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.MULHSU   -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_MULHSU   , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.MULHU    -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_MULHU    , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.DIV      -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_DIV      , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.DIVU     -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_DIVU     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.REM      -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_REM      , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.REMU     -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_REMU     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.MULW     -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_MULW     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.DIVW     -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_DIVW     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.DIVUW    -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_DIVUW    , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.REMW     -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_REMW     , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U),
        RV_Inst.REMUW    -> List(inst_types.inst_R    , 0.U, EXU_opcode.EXU_REMUW    , LSU_opcode.LSU_DoNothing, 1.U, 1.U, 0.U, 0.U)
        )

    val IDU_inst_type          = IDU_opcodes(0)
    io.IDU_O_ModifyMem        := IDU_opcodes(1)
    io.IDU_O_EXUopcode        := IDU_opcodes(2)
    io.IDU_O_LSUopcode        := IDU_opcodes(3)
    io.IDU_O_snpcISdnpc       := IDU_opcodes(4)
    io.IDU_O_GPRneedWriteBack := IDU_opcodes(5)
    io.IDU_O_error            := IDU_opcodes(6)
    io.IDU_O_halt             := IDU_opcodes(7)

    io.IDU_O_imm := MuxCase(SignExtend_immR, Array(
        IDU_inst_type === inst_types.inst_error -> SignExtend_immR,
        IDU_inst_type === inst_types.inst_N     -> SignExtend_immR,
        IDU_inst_type === inst_types.inst_R     -> SignExtend_immR,
        IDU_inst_type === inst_types.inst_I     -> SignExtend_immI,
        IDU_inst_type === inst_types.inst_S     -> SignExtend_immS,
        IDU_inst_type === inst_types.inst_B     -> SignExtend_immB,
        IDU_inst_type === inst_types.inst_U     -> SignExtend_immU,
        IDU_inst_type === inst_types.inst_J     -> SignExtend_immJ
    ))
}

class EXU extends Module{
    val io = IO(new Bundle{
        EXU_I_src1 = Input(UInt(64.W))
        EXU_I_src2 = Input(UInt(64.W))
        EXU_I_imm = Input(UInt(64.W))
        EXU_I_opcode = Input(UInt(6.W))
        EXU_I_currentPC = Input(UInt(64.W))
        EXU_O_result = Output(UInt(64.W))
        EXU_O_snpcNEQdnpc = Output(Bool())
        EXU_O_error = Output(Bool())
    })
    io.EXU_O_error := false.B
    val EXU_src1_signed = io.EXU_I_src1.asSInt
    val EXU_src1_unsigned = io.EXU_I_src1.asUInt
    val EXU_src2_signed = io.EXU_I_src2.asSInt
    val EXU_src2_unsigned = io.EXU_I_src2.asUInt
    val EXU_imm_signed = io.EXU_I_imm.asSInt
    val EXU_imm_unsigned = io.EXU_I_imm.asUInt

    // We use a list to manage the output of EXU
    // List(result(65, 2), snpcNEQdnpc(1), error(0))

    var EXU_output = ListLookup(
        /*Compare Item: */ io.EXU_I_opcode,
        // If none of the opcodes are matched, we set result to 0, with static next pc and raise error
        /*Default: */               List(0.U(64.W)                                                                                                                 , 1.U                                                  , 1.U),
        EXU_opcode.EXU_DoNothing -> List(0.U(64.W)                                                                                                                 , 1.U                                                  , 0.U),
        EXU_opcode.LUI           -> List(EXU_imm_unsigned                                                                                                          , 1.U                                                  , 0.U),
        EXU_opcode.AUIPC         -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , 1.U                                                  , 0.U),
        EXU_opcode.JAL           -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , 0.U                                                  , 0.U),
        EXU_opcode.JALR          -> List((EXU_src1_unsigned + EXU_src2_unsigned) & ~1                                                                              , 0.U                                                  , 0.U),
        EXU_opcode.BEQ           -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , (!(EXU_src1_unsigned === EXU_src2_unsigned)).asUInt  , 0.U),
        EXU_opcode.BNE           -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , (!(EXU_src1_unsigned =/= EXU_src2_unsigned)).asUInt  , 0.U),
        EXU_opcode.BLT           -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , (!(EXU_src1_signed < EXU_src2_signed)).asUInt        , 0.U),
        EXU_opcode.BGE           -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , (!(EXU_src1_signed >= EXU_src2_signed)).asUInt       , 0.U),
        EXU_opcode.BLTU          -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , (!(EXU_src1_unsigned < EXU_src2_unsigned)).asUInt    , 0.U),
        EXU_opcode.BGEU          -> List(io.EXU_I_currentPC + EXU_imm_unsigned                                                                                     , (!(EXU_src1_unsigned >= EXU_src2_unsigned)).asUInt   , 0.U),
        EXU_opcode.LB            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.LH            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.LW            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.LBU           -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.LHU           -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.SB            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.SH            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.SW            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.ADDI          -> List(EXU_src1_unsigned + EXU+EXU_imm_unsigned                                                                                  , 0.U                                                  , 0.U),
        EXU_opcode.SLTI          -> List(Mux(EXU_src1_signed < EXU_imm_signed, 1.U(64.W), 0.U(64.W))                                                               , 0.U                                                  , 0.U),
        EXU_opcode.SLTIU         -> List(Mux(EXU_src1_unsigned < EXU_imm_unsigned, 1.U(64.W), 0.U(64.W))                                                           , 0.U                                                  , 0.U),
        EXU_opcode.XORI          -> List(EXU_src1_unsigned ^ EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.ORI           -> List(EXU_src1_unsigned | EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.ANDI          -> List(EXU_src1_unsigned & EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.SLLI          -> List(EXU_src1_unsigned << EXU_imm_unsigned(5, 0)                                                                               , 0.U                                                  , 0.U),
        EXU_opcode.SRLI          -> List(EXU_src1_unsigned >> EXU_imm_unsigned(5, 0)                                                                               , 0.U                                                  , 0.U),
        EXU_opcode.SRAI          -> List(EXU_src1_signed >> EXU_imm_unsigned(5, 0)                                                                                 , 0.U                                                  , 0.U),
        EXU_opcode.ADD           -> List(EXU_src1_unsigned + EXU_src2_unsigned                                                                                     , 0.U                                                  , 0.U),
        EXU_opcode.SUB           -> List(EXU_src1_unsigned - EXU_src2_unsigned                                                                                     , 0.U                                                  , 0.U),
        EXU_opcode.SLL           -> List(EXU_src1_unsigned << EXU_src2_unsigned(5, 0)                                                                              , 0.U                                                  , 0.U),
        EXU_opcode.SLT           -> List(Mux(EXU_src1_signed < EXU_src2_signed, 1.U(64.W), 0.U(64.W))                                                              , 0.U                                                  , 0.U),
        EXU_opcode.SLTU          -> List(Mux(EXU_src1_unsigned < EXU_src2_unsigned, 1.U(64.W), 0.U(64.W))                                                          , 0.U                                                  , 0.U),
        EXU_opcode.XOR           -> List(EXU_src1_unsigned ^ EXU_src2_unsigned                                                                                     , 0.U                                                  , 0.U),
        EXU_opcode.SRL           -> List(EXU_src1_unsigned >> EXU_src2_unsigned(5, 0)                                                                              , 0.U                                                  , 0.U),
        EXU_opcode.SRA           -> List(EXU_src1_signed >> EXU_src2_signed(5, 0)                                                                                  , 0.U                                                  , 0.U),
        EXU_opcode.OR            -> List(EXU_src1_unsigned | EXU_src2_unsigned                                                                                     , 0.U                                                  , 0.U),
        EXU_opcode.AND           -> List(EXU_src1_unsigned & EXU_src2_unsigned                                                                                     , 0.U                                                  , 0.U),
        EXU_opcode.LWU           -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.LD            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.SD            -> List(EXU_src1_unsigned + EXU_imm_unsigned                                                                                      , 0.U                                                  , 0.U),
        EXU_opcode.ADDIW         -> List(Cat(Fill(32, (EXU_src1_unsigned + EXU_imm_unsigned)(31)), (EXU_src1_unsigned + EXU_imm_unsigned)(31, 0))                  , 0.U                                                  , 0.U),
        EXU_opcode.SLLIW         -> List(Cat(Fill(32, (EXU_src1_unsigned << EXU_imm_unsigned(4, 0))(31)), (EXU_src1_unsigned << EXU_imm_unsigned(4, 0))(31, 0))    , 0.U                                                  , 0.U),
        EXU_opcode.SRLIW         -> List(Cat(Fill(32, (EXU_src1_unsigned >> EXU_imm_unsigned(4, 0))(31)), (EXU_src1_unsigned >> EXU_imm_unsigned(4, 0))(31, 0))    , 0.U                                                  , 0.U),
        EXU_opcode.SRAIW         -> List(Cat(Fill(32, (EXU_src1_signed >> EXU_imm_unsigned(4, 0))(31)), (EXU_src1_signed >> EXU_imm_signed(4, 0))(31, 0))          , 0.U                                                  , 0.U),
        EXU_opcode.ADDW          -> List(Cat(Fill(32, (EXU_src1_unsigned + EXU_src2_unsigned)(31)), (EXU_src1_unsigned + EXU_src2_unsigned)(31, 0))                , 0.U                                                  , 0.U),
        EXU_opcode.SUBW          -> List(Cat(Fill(32, (EXU_src1_unsigned - EXU_src2_unsigned)(31)), (EXU_src1_unsigned + EXU_src2_unsigned)(31, 0))                , 0.U                                                  , 0.U),
        EXU_opcode.SLLW          -> List(Cat(Fill(32, (EXU_src1_unsigned << EXU_src2_unsigned(4, 0))(31)), (EXU_src1_unsigned << EXU_imm_unsigned(4, 0))(31, 0))   , 0.U                                                  , 0.U),
        EXU_opcode.SRLW          -> List(Cat(Fill(32, (EXU_src1_unsigned >> EXU_src2_unsigned(4, 0))(31)), (EXU_src1_unsigned >> EXU_imm_unsigned(4, 0))(31, 0))   , 0.U                                                  , 0.U),
        EXU_opcode.SRAW          -> List(Cat(Fill(32, (EXU_src1_signed >> EXU_src2_unsigned(4, 0))(31)), (EXU_src1_signed >> EXU_imm_signed(4, 0))(31, 0))         , 0.U                                                  , 0.U),
        EXU_opcode.MUL           -> List((EXU_src1_unsigned * EXU_src2_unsigned)(63, 0)                                                                            , 0.U                                                  , 0.U),
        EXU_opcode.MULH          -> List((EXU_src1_signed * EXU_src2_signed)(127, 64)                                                                              , 0.U                                                  , 0.U),
        EXU_opcode.MULHSU        -> List((EXU_src1_signed * EXU_src2_unsigned)(127, 64)                                                                            , 0.U                                                  , 0.U),
        EXU_opcode.MULHU         -> List((EXU_src1_unsigned * EXU_src2_unsigned)(127, 64)                                                                          , 0.U                                                  , 0.U),
        EXU_opcode.DIV           -> List((EXU_src1_signed / EXU_src2_signed)                                                                                       , 0.U                                                  , 0.U),
        EXU_opcode.DIVU          -> List((EXU_src1_unsigned / EXU_src2_unsigned)                                                                                   , 0.U                                                  , 0.U),
        EXU_opcode.REM           -> List((EXU_src1_signed % EXU_src2_signed)                                                                                       , 0.U                                                  , 0.U),
        EXU_opcode.REMU          -> List((EXU_src1_unsigned % EXU_src2_unsigned)                                                                                   , 0.U                                                  , 0.U),
        EXU_opcode.MULW          -> List(Cat(Fill(32, (EXU_src1_unsigned * EXU_src2_unsigned)(31)), (EXU_src1_unsigned * EXU_src2_unsigned)(31, 0))                , 0.U                                                  , 0.U),
        EXU_opcode.DIVW          -> List(Cat(Fill(32, (EXU_src1_signed / EXU_src2_signed)(31)), (EXU_src1_signed / EXU_src2_signed)(31, 0))                        , 0.U                                                  , 0.U),
        EXU_opcode.DIVUW         -> List(Cat(Fill(32, (EXU_src1_unsigned / EXU_src2_unsigned)(31)), (EXU_src1_unsigned / EXU_src2_unsigned)(31, 0))                , 0.U                                                  , 0.U),
        EXU_opcode.REMW          -> List(Cat(Fill(32, (EXU_src1_signed % EXU_src2_signed)(31)), (EXU_src1_signed % EXU_src2_signed)(31, 0))                        , 0.U                                                  , 0.U),
        EXU_opcode.REMUW         -> List(Cat(Fill(32, (EXU_src1_unsigned % EXU_src2_unsigned)(31)), (EXU_src1_unsigned % EXU_src2_unsigned)(31, 0))                , 0.U                                                  , 0.U)
    )

    io.EXU_O_result := EXU_output(0)
    io.EXU_O_snpcNEQdnpc := EXU_output(1)
    io.EXU_O_error := EXU_output(2)
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
        //LSU_I_EXUresult = Input(UInt(64.W))
        LSU_I_opcode = Input(UInt(6.W))
        LSU_I_ModifyMem = Input(Bool())
        LSU_O_result = Output(UInt(64.W))
        // Following signals are pulled to top
        LSU_O_memAddr = Output(UInt(64.W))
        LSU_O_memRW = Output(Bool()) // Low: Read, High: Write
        LSU_I_memR = Input(UInt(64.W))
        LSU_O_memW = Output(UInt(64.W))
        LSU_O_error = Output(Bool())
        LSU_O_len = Output(UInt(2.W)) // 00: Bit 01: Half 10: Word 11: Double Word
    })
    io.LSU_O_error := false.B

    // We use a list to manage the output of LSU
    // List(result, memAddr, memRW, memW, error, len)

    var LSU_output = ListLookup(
        /*Compare Item: */ io.LSU_I_opcode,
        /*Default: */               List(0.U(64.W)                                        , 0.U(64.W)     , 0.U, 0.U(64.W)     , 1.U , "b00".U),
        LSU_opcode.LSU_DoNothing -> List(0.U(64.W)                                        , 0.U(64.W)     , 0.U, 0.U(64.W)     , 0.U , "b00".U),
        LSU_opcode.LSU_LB        -> List(Cat(Fill(56, io.LSU_I_memR(7)) , io.LSU_I_memR)  , io.LSU_I_src1 , 0.U, 0.U(64.W)     , 0.U , "b00".U),
        LSU_opcode.LSU_LBU       -> List(Cat(Fill(56, 0) , io.LSU_I_memR)                 , io.LSU_I_src1 , 0.U, 0.U(64.W)     , 0.U , "b00".U),
        LSU_opcode.LSU_LH        -> List(Cat(Fill(48, io.LSU_I_memR(15)) , io.LSU_I_memR) , io.LSU_I_src1 , 0.U, 0.U(64.W)     , 0.U , "b01".U),
        LSU_opcode.LSU_LHU       -> List(Cat(Fill(48, 0) , io.LSU_I_memR)                 , io.LSU_I_src1 , 0.U, 0.U(64.W)     , 0.U , "b01".U),
        LSU_opcode.LSU_LW        -> List(Cat(Fill(32, io.LSU_I_memR(31)) , io.LSU_I_memR) , io.LSU_I_src1 , 0.U, 0.U(64.W)     , 0.U , "b10".U),
        LSU_opcode.LSU_LWU       -> List(Cat(Fill(32, 0) , io.LSU_I_memR)                 , io.LSU_I_src1 , 0.U, 0.U(64.W)     , 0.U , "b10".U),
        LSU_opcode.LSU_LD        -> List(Cat(Fill(32, io.LSU_I_memR(31)) , io.LSU_I_memR) , io.LSU_I_src1 , 0.U, 0.U(64.W)     , 0.U , "b11".U),
        LSU_opcode.LSU_SB        -> List(0.U(64.W)                                        , io.LSU_I_src1 , 1.U, io.LSU_I_src2 , 0.U , "b00".U),
        LSU_opcode.LSU_SH        -> List(0.U(64.W)                                        , io.LSU_I_src1 , 1.U, io.LSU_I_src2 , 0.U , "b01".U),
        LSU_opcode.LSU_SW        -> List(0.U(64.W)                                        , io.LSU_I_src1 , 1.U, io.LSU_I_src2 , 0.U , "b10".U),
        LSU_opcode.LSU_SD        -> List(0.U(64.W)                                        , io.LSU_I_src1 , 1.U, io.LSU_I_src2 , 0.U , "b11".U),
    )

    io.LSU_O_result := LSU_output(0)
    io.LSU_O_memAddr := LSU_output(1)
    io.LSU_O_memRW := LSU_output(2)
    io.LSU_O_memW := LSU_output(3)
    io.LSU_O_error := LSU_output(4)
    io.LSU_O_len := LSU_output(5)
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
        NPC_LSU_O_memW = Output(UInt(64.W))
        NPC_LSU_O_lem = Output(UInt(2.W))

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
    npcb_EXU.io.EXU_I_imm := npcb_IDU.io.IDU_O_imm
    npcb_EXU.io.EXU_I_opcode := npcb_IDU.io.IDU_O_EXUopcode
    npcb_EXU.io.EXU_I_currentPC := PC

    // Step IV: LSU execution (we access memory using Verilator, NEMU and AM's debug interfaces)
    val npcb_LSU = Module(new LSU)
    npcb_LSU.io.LSU_I_src1 := npcb_EXU.io.EXU_O_result // LSU will get src1 + imm from EXU's result
    npcb_LSU.io.LSU_I_src2 := npcb_IDU.io.IDU_O_src2
    //npcb_LSU.io.LSU_I_EXUresult := npcb_EXU.io.EXU_O_result
    npcb_LSU.io.LSU_I_ModifyMem := npcb_IDU.io.IDU_O_ModifyMem
    npcb_LSU.io.LSU_I_opcode := npcb_IDU.io.IDU_O_LSUopcode

    // Step V: Write back data to a GPR and PC
    val npcb_WBU = Module(new WBU)
    val npcb_PCU = Module(new PCU)
    npcb_WBU.io.WBU_I_EXUresult := npcb_EXU.io.EXU_O_result
    npcb_WBU.io.WBU_I_LSUresult := npcb_LSU.io.LSU_O_result
    npcb_WBU.io.WBU_I_EXUsnpcNEQdnpc := npcb_EXU.io.EXU_O_snpcNEQdnpc
    npcb_PCU.io.PCU_I_CurrentPC := PC
    npcb_PCU.io.PCU_I_willJump := (! npcb_IDU.io.IDU_O_snpcISdnpc) && npcb_EXU.io.EXU_O_snpcNEQdnpc
    npcb_PCU.io.PCU_I_imm := npcb_IDU.io.IDU_O_imm
    npcb_WBU.io.WBU_I_rd := npcb_IDU.io.IDU_O_rd
    npcb_WBU.io.WBU_I_IDUsnpcISdnpc := npcb_IDU.io.IDU_O_snpcISdnpc
    NPC_GPRchanged := true.B
    Mux(npcb_IDU.io.IDU_O_GPRneedWriteBack, GPR(npcb_WBU.io.WBU_O_GPRidx) := npcb_WBU.io.WBU_O_GPRWriteBack, NPC_GPRchanged := false.B)
    PC := npcb_PCU.io.PCU_O_DynamicNextPC
}