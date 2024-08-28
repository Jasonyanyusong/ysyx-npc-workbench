package npc

import chisel3._
import chisel3.util._

object NPC_State { // 3 bit register save current cpu state
    val NPC_State_Idle = 0.U(3.W)
    val NPC_State_IF   = 1.U(3.W)
    val NPC_State_ID   = 2.U(3.W)
    val NPC_State_EX   = 3.U(3.W)
    val NPC_State_LS   = 4.U(3.W)
    val NPC_State_WB   = 5.U(3.W)
    val NPC_State_Stop = 6.U(3.W)
}

class NPC_AXI_IO extends Bundle {
    // Address Read (AR)
    val axi_ar_ready_i = Input(Bool())
    val axi_ar_valid_o = Output(Bool())
    val axi_ar_addr_o = Output(UInt(32.W))
    val axi_ar_id_o = Output(UInt(4.W))
    val axi_ar_len_o = Output(UInt(8.W))
    val axi_ar_size_o = Output(UInt(3.W))
    val axi_ar_burst_o = Output(UInt(2.W))

    // Read (R)
    val axi_r_ready_o = Output(Bool())
    val axi_r_valid_i = Input(Bool())
    val axi_r_resp_i = Input(UInt(2.W))
    val axi_r_data_i = Input(UInt(64.W))
    val axi_r_last_i = Input(Bool())
    val axi_r_id_i = Input(UInt(4.W))

    // Address Write (AW)
    val axi_aw_ready_i = Input(Bool())
    val axi_aw_valid_o = Output(Bool())
    val axi_aw_addr_o = Output(UInt(32.W))
    val axi_aw_id_o = Output(UInt(4.W))
    val axi_aw_len_o = Output(UInt(8.W))
    val axi_aw_size_o = Output(UInt(3.W))
    val axi_aw_burst_o = Output(UInt(2.W))

    // Write (W)
    val axi_w_ready_i = Input(Bool())
    val axi_w_valid_o = Output(Bool())
    val axi_w_data_o = Output(UInt(64.W))
    val axi_w_strb_o = Output(UInt(8.W))
    val axi_w_last_o = Output(Bool())

    // Write Response (B)
    val axi_b_ready_o = Output(Bool())
    val axi_b_valid_i = Input(Bool())
    val axi_b_resp_i = Input(UInt(2.W))
    val axi_b_id_i = Input(UInt(4.W))
}

class NPC extends Module {
    // IO (outer AXI 4)
    val axi_io = IO(new NPC_AXI_IO)

    // State register
    NPC_State_Register = RegInit(NPC_State.NPC_State_Idle) // start with idle state

    // GPR and PC
    NPC_GPR_Registers = Mem(16, UInt(32.W)) // A total of 16 RV32 32 bit registers
    NPC_PC_Register   = RegInit("h80000000".U(32.W)) // start with PC 0x80000000

    // Read GPR function, ensure x0 is always 0
    def NPC_GPR_Read(index : UInt) = Mux(index === 0.U, 0.U(32.W), NPC_GPR_Registers(index))

    // Initiate modules: IFU, IDU, EXU, LSU, WBU, AXI_Arbiter
    val IFU = Module (new NPC_IFU)
    val IDU = Module (new NPC_IDU)

    // Stage connection - IF -> ID
    IF2ID_Inst = RegInit(0.U(32.W))
    IF2ID_Inst := Mux(
        IFU.ifu_internal_io.ifu_internal_instValid_o,
        IFU.ifu_internal_io.ifu_internal_inst_o,
        IF2ID_Inst
    )
    IDU.idu_internal_io.idu_internal_inst_i := IF2ID_Inst

    // Stage connection - ID -> EX

    // Stage connection - EX -> LS

    // Stage connection - LS -> WB

    // Internal connection - IFU (transfer PC and valid information)
    IFU.ifu_internal_io.ifu_internal_work_i := (NPC_State_Register === NPC_State.NPC_State_IF)
    IFU.ifu_internal_io.ifu_internal_pc_i := NPC_PC_Register

    // Internal connection - IDU (work signal, pc input, register read)
    IDU.idu_internal_io.idu_internal_work_i := (NPC_State_Register === NPC_State.NPC_State_ID)

    val NPC_Reg1 = NPC_GPR_Read(IDU.idu_internal_io.idu_internal_rs1_o)
    val NPC_Reg2 = NPC_GPR_Read(IDU.idu_internal_io.idu_internal_rs2_o)
    IDU.idu_internal_io.idu_internal_rs1_val_i := NPC_Reg1
    IDU.idu_internal_io.idu_internal_rs2_val_i := NPC_Reg2

    IDU.idu_internal_io.idu_internal_pc_i := NPC_PC_Register
    NPC_PC_Register := Mux(
        IDU.idu_internal_io.idu_internal_valid_o,
        IDU.idu_internal_io.idu_internal_dnpc_o,
        NPC_PC_Register
    )

    // Internal connection - LSU (transfer valid information)

    // Connect AXI arbiter

    // Stage update
    NPC_State_Register := MuxCase(NPC_State_Register,
        // TODO: update NPC state according to each state

        // when state is idle, we can fetch a new instruction
        (NPC_State_Register === NPC_State.NPC_State_Idle) -> NPC_State.NPC_State_IF,

        // when IF is finish, continue to ID
        (NPC_State_Register === NPC_State.NPC_State_IF) -> Mux(
            IFU.ifu_internal_io.ifu_internal_instValid_o,
            NPC_State.NPC_State_ID,
            NPC_State.NPC_State_IF
        ),

        // when ID is finish, continue to EX if is not ebreak, else just stop(ID is one-cycle only)
        (NPC_State_Register === NPC_State.NPC_State_ID) -> Mux(
            IDU.idu_internal_io.idu_internal_valid_o,
            Mux(
                IDU.idu_internal_io.idu_internal_ebreak_o,
                NPC_State.NPC_State_Stop, // is ebreak, stop
                NPC_State.NPC_State_EX // not ebreak
            ),
            NPC_State.NPC_State_ID
        ),

        // when EX is finish, continue to LS (EX is one-cycle only)

        // when LS is finish, continue to WB

        // when WB is finish, set to idle, so next cycle will start (WB is one-cycle only)

        // If the state is stop (ebreak), continue to ebreak and do nothing else
        (NPC_State_Register === NPC_State.NPC_State_Stop) -> (NPC_State.NPC_State_Stop)
    )
}