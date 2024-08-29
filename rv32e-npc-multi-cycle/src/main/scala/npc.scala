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

class NPC_AXI_IO_Slave extends Bundle {
    // Address Read (AR)
    val axi_slave_ar_ready_o = Output(Bool())
    val axi_slave_ar_valid_i = Input(Bool())
    val axi_slave_ar_addr_i = Input(UInt(32.W))
    val axi_slave_ar_id_i = Input(UInt(4.W))
    val axi_slave_ar_len_i = Input(UInt(8.W))
    val axi_slave_ar_size_i = Input(UInt(3.W))
    val axi_slave_ar_burst_i = Input(UInt(2.W))

    // Read (R)
    val axi_slave_r_ready_i = Input(Bool())
    val axi_slave_r_valid_o = Output(Bool())
    val axi_slave_r_resp_o = Output(UInt(2.W))
    val axi_slave_r_data_o = Output(UInt(64.W))
    val axi_slave_r_last_o = Output(Bool())
    val axi_slave_r_id_o = Output(UInt(4.W))
}

class NPC_AXI_IO_Master extends Bundle {
    // Address Read (AR)
    val axi_master_ar_ready_i = Input(Bool())
    val axi_master_ar_valid_o = Output(Bool())
    val axi_master_ar_addr_o = Output(UInt(32.W))
    val axi_master_ar_id_o = Output(UInt(4.W))
    val axi_master_ar_len_o = Output(UInt(8.W))
    val axi_master_ar_size_o = Output(UInt(3.W))
    val axi_master_ar_burst_o = Output(UInt(2.W))

    // Read (R)
    val axi_master_r_ready_o = Output(Bool())
    val axi_master_r_valid_i = Input(Bool())
    val axi_master_r_resp_i = Input(UInt(2.W))
    val axi_master_r_data_i = Input(UInt(64.W))
    val axi_master_r_last_i = Input(Bool())
    val axi_master_r_id_i = Input(UInt(4.W))
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
    val EXU = Module (new NPC_EXU)
    val LSU = Module (new NPC_LSU)
    val WBU = Module (new NPC_WBU)
    
    val Arbiter = Module (new NPC_Arbiter)

    // Stage connection - IF -> ID
    IF2ID_Inst = RegInit(0.U(32.W))
    IF2ID_Inst := Mux(
        IFU.ifu_internal_io.ifu_internal_instValid_o,
        IFU.ifu_internal_io.ifu_internal_inst_o,
        IF2ID_Inst
    )
    IDU.idu_internal_io.idu_internal_inst_i := IF2ID_Inst

    // Stage connection - ID -> EX
    ID2EX_EXU_OPCODE = RegInit(0.U(4.W))
    ID2EX_EXU_SRC1 = RegInit(0.U(32.W))
    ID2EX_EXU_SRC2 = RegInit(0.U(32.W))
    ID2EX_LSU_OPCODE = RegInit(0.U(4.W))
    ID2EX_LSU_WDATA = RegInit(0.U(32.W))
    ID2EX_WBU_OPCODE = RegInit(0.U(2.W))
    ID2EX_WBU_RD = RegInit(0.U(4.W))

    ID2EX_EXU_OPCODE := Mux(
        IDU.idu_internal_io.idu_internal_valid_o,
        IDU.idu_internal_io.idu_internal_exu_opcode_o,
        ID2EX_EXU_OPCODE
    )
    ID2EX_EXU_SRC1 := Mux(
        IDU.idu_internal_io.idu_internal_valid_o,
        IDU.idu_internal_io.idu_internal_exu_src1_o,
        ID2EX_EXU_SRC1
    )
    ID2EX_EXU_SRC2 := Mux(
        IDU.idu_internal_io.idu_internal_valid_o,
        IDU.idu_internal_io.idu_internal_exu_src2_o,
        ID2EX_EXU_SRC2
    )
    ID2EX_LSU_OPCODE := Mux(
        IDU.idu_internal_io.idu_internal_valid_o,
        IDU.idu_internal_io.idu_internal_lsu_opcode_o,
        ID2EX_LSU_OPCODE
    )
    ID2EX_LSU_WDATA := Mux(
        IDU.idu_internal_io.idu_internal_valid_o,
        IDU.idu_internal_io.idu_internal_lsu_wdata_o,
        ID2EX_LSU_WDATA
    )
    ID2EX_WBU_OPCODE := Mux(
        IDU.idu_internal_io.idu_internal_valid_o,
        IDU.idu_internal_io.idu_internal_wbu_opcode_o,
        ID2EX_WBU_OPCODE
    )
    ID2EX_WBU_RD := Mux(
        IDU.idu_internal_io.idu_internal_valid_o,
        IDU.idu_internal_io.idu_internal_wbu_rd_o,
        ID2EX_WBU_RD
    )

    EXU.exu_internal_io.exu_internal_ex_opcode_i := ID2EX_EXU_OPCODE
    EXU.exu_internal_io.exu_internal_ex_src1_i := ID2EX_EXU_SRC1
    EXU.exu_internal_io.exu_internal_ex_src2_i := ID2EX_EXU_SRC2
    EXU.exu_internal_io.exu_internal_ls_opcode_i := ID2EX_LSU_OPCODE
    EXU.exu_internal_io.exu_internal_ls_wdata_i := ID2EX_LSU_WDATA
    EXU.exu_internal_io.exu_internal_wb_opcode_i := ID2EX_WBU_OPCODE
    EXU.exu_internal_io.exu_internal_wb_rd_i := ID2EX_WBU_RD

    // Stage connection - EX -> LS
    EX2LS_RESULT = RegInit(0.U(32.W))
    EX2LS_LSOPCODE = RegInit(0.U(4.W))
    EX2LS_LSWDATA = RegInit(0.U(32.W))
    EX2LS_WBOPCODE = RegInit(0.U(2.W))
    EX2LS_WBRD = RegInit(0.U(4.W))

    EX2LS_RESULT := Mux(
        EXU.exu_internal_io.exu_internal_valid_o,
        EXU.exu_internal_io.exu_internal_result_o,
        EX2LS_RESULT
    )
    EX2LS_LSOPCODE := Mux(
        EXU.exu_internal_io.exu_internal_valid_o,
        EXU.exu_internal_io.exu_internal_ls_opcode_o,
        EX2LS_LSOPCODE
    )
    EX2LS_LSWDATA := Mux(
        EXU.exu_internal_io.exu_internal_valid_o,
        EXU.exu_internal_io.exu_internal_ls_wdata_o,
        EX2LS_LSWDATA
    )
    EX2LS_WBOPCODE := Mux(
        EXU.exu_internal_io.exu_internal_valid_o,
        EXU.exu_internal_io.exu_internal_wb_opcode_o,
        EX2LS_WBOPCODE
    )
    EX2LS_WBRD := Mux(
        EXU.exu_internal_io.exu_internal_valid_o,
        EXU.exu_internal_io.exu_internal_wb_rd_o,
        EX2LS_WBRD
    )

    LSU.lsu_internal_io.lsu_internal_ls_op_i := EX2LS_LSOPCODE
    LSU.lsu_internal_io.lsu_internal_ls_wdata_i := EX2LS_LSWDATA
    LSU.lsu_internal_io.lsu_internal_ex_result_i := EX2LS_RESULT
    LSU.lsu_internal_io.lsu_internal_wb_opcode_i := EX2LS_WBOPCODE
    LSU.lsu_internal_io.lsu_internal_wb_rd_i := EX2LS_WBRD

    // Stage connection - LS -> WB
    LS2WB_LSRESULT = RegInit(0.U(32.W))
    LS2WB_EXRESULT = RegInit(0.U(32.W))
    LS2WB_WBOPCODE = RegInit(0.U(2.W))
    LS2WB_WBRD = RegInit(0.U(4.W))

    LS2WB_LSRESULT := Mux(
        LSU.lsu_internal_io.lsu_internal_valid_o,
        LSU.lsu_internal_io.lsu_internal_ls_result_o,
        LS2WB_LSRESULT
    )
    LS2WB_EXRESULT := Mux(
        LSU.lsu_internal_io.lsu_internal_valid_o,
        LSU.lsu_internal_io.lsu_internal_ex_result_o,
        LS2WB_EXRESULT
    )
    LS2WB_WBOPCODE := Mux(
        LSU.lsu_internal_io.lsu_internal_valid_o,
        LSU.lsu_internal_io.lsu_internal_wb_opcode_o,
        LS2WB_WBOPCODE
    )
    LS2WB_WBRD := Mux(
        LSU.lsu_internal_io.lsu_internal_valid_o,
        LSU.lsu_internal_io.lsu_internal_wb_rd_o,
        LS2WB_WBRD
    )

    WBU.wbu_internal_io.wbu_internal_ls_ret_i := LS2WB_LSRESULT
    WBU.wbu_internal_io.wbu_internal_ex_ret_i := LS2WB_EXRESULT
    WBU.wbu_internal_io.wbu_internal_opcode_i := LS2WB_WBOPCODE
    WBU.wbu_internal_io.wbu_internal_rd_i := LS2WB_WBRD

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

    // Internal connection - EXU (work signal)
    EXU.exu_internal_io.exu_internal_work_i := (NPC_State_Register === NPC_State.NPC_State_EX)

    // Internal connection - LSU (transfer valid information)
    LSU.lsu_internal_io.lsu_internal_work_i := (NPC_State_Register === NPC_State.NPC_State_LS)

    // Internal connection - WBU (update registers)
    WBU.wbu_internal_io.wbu_internal_work_i := (NPC_State_Register === NPC_State.NPC_State_WB)
    val WBU_WB_RD = Mux(
        WBU.wbu_internal_io.wbu_internal_valid_o,
        WBU.wbu_internal_io.wbu_internal_rd_o,
        0.U(4.W)
    )
    NPC_GPR_Registers(WBU_WB_RD) := Mux(
        WBU.wbu_internal_io.wbu_internal_valid_o,
        WBU.wbu_internal_io.wbu_internal_wdata_o,
        NPC_GPR_Registers(WBU_WB_RD)
    )

    // Connect AXI arbiter
    IFU.ifu_axi_io.ifu_axi_ar_ready_i := Arbiter.arbiter_ifu_axi_io.axi_slave_ar_ready_o
    Arbiter.arbiter_ifu_axi_io.axi_slave_ar_valid_i := IFU.ifu_axi_io.ifu_axi_ar_valid_o
    Arbiter.arbiter_ifu_axi_io.axi_slave_ar_addr_i := IFU.ifu_axi_io.ifu_axi_ar_addr_o
    Arbiter.arbiter_ifu_axi_io.axi_slave_ar_id_i := IFU.ifu_axi_io.ifu_axi_ar_id_o
    Arbiter.arbiter_ifu_axi_io.axi_slave_ar_len_i := IFU.ifu_axi_io.ifu_axi_ar_len_o
    Arbiter.arbiter_ifu_axi_io.axi_slave_ar_size_i := IFU.ifu_axi_io.ifu_axi_ar_size_o
    Arbiter.arbiter_ifu_axi_io.axi_slave_ar_burst_i := IFU.ifu_axi_io.ifu_axi_ar_burst_o

    Arbiter.arbiter_ifu_axi_io.axi_slave_r_ready_i := IFU.ifu_axi_io.ifu_axi_r_ready_o
    IFU.ifu_axi_io.ifu_axi_r_valid_i := Arbiter.arbiter_ifu_axi_io.axi_slave_r_valid_o
    IFU.ifu_axi_io.ifu_axi_r_resp_i := Arbiter.arbiter_ifu_axi_io.axi_slave_r_resp_o
    IFU.ifu_axi_io.ifu_axi_r_data_i := Arbiter.arbiter_ifu_axi_io.axi_slave_r_data_o
    IFU.ifu_axi_io.ifu_axi_r_last_i := Arbiter.arbiter_ifu_axi_io.axi_slave_r_last_o
    IFU.ifu_axi_io.ifu_axi_r_id_i := Arbiter.arbiter_ifu_axi_io.axi_slave_r_id_o

    LSU.lsu_axi_io.axi_ar_ready_i := Arbiter.arbiter_lsu_axi_io.axi_slave_ar_ready_o
    Arbiter.arbiter_lsu_axi_io.axi_slave_ar_valid_i := LSU.lsu_axi_io.axi_ar_valid_o
    Arbiter.arbiter_lsu_axi_io.axi_slave_ar_addr_i := LSU.lsu_axi_io.axi_ar_addr_o
    Arbiter.arbiter_lsu_axi_io.axi_slave_ar_id_i := LSU.lsu_axi_io.axi_ar_id_o
    Arbiter.arbiter_lsu_axi_io.axi_slave_ar_len_i := LSU.lsu_axi_io.axi_ar_len_o
    Arbiter.arbiter_lsu_axi_io.axi_slave_ar_size_i := LSU.lsu_axi_io.axi_ar_size_o
    Arbiter.arbiter_lsu_axi_io.axi_slave_ar_burst_i := LSU.lsu_axi_io.axi_ar_burst_o

    Arbiter.arbiter_lsu_axi_io.axi_slave_r_ready_i := LSU.lsu_axi_io.axi_r_ready_o
    LSU.lsu_axi_io.axi_r_valid_i := Arbiter.arbiter_lsu_axi_io.axi_slave_r_valid_o
    LSU.lsu_axi_io.axi_r_resp_i := Arbiter.arbiter_lsu_axi_io.axi_slave_r_resp_o
    LSU.lsu_axi_io.axi_r_data_i := Arbiter.arbiter_lsu_axi_io.axi_slave_r_data_o
    LSU.lsu_axi_io.axi_r_last_i := Arbiter.arbiter_lsu_axi_io.axi_slave_r_last_o
    LSU.lsu_axi_io.axi_r_id_i := Arbiter.arbiter_lsu_axi_io.axi_slave_r_id_o

    // Connect AXI channels - AR
    Arbiter.arbiter_top_axi_io.axi_master_ar_ready_i := axi_io.axi_ar_ready_i
    axi_io.axi_ar_valid_o := Arbiter.arbiter_top_axi_io.axi_master_ar_valid_o
    axi_io.axi_ar_addr_o := Arbiter.arbiter_top_axi_io.axi_master_ar_addr_o
    axi_io.axi_ar_id_o := Arbiter.arbiter_top_axi_io.axi_master_ar_id_o
    axi_io.axi_ar_len_o := Arbiter.arbiter_top_axi_io.axi_master_ar_len_o
    axi_io.axi_ar_size_o := Arbiter.arbiter_top_axi_io.axi_master_ar_size_o
    axi_io.axi_ar_burst_o := Arbiter.arbiter_top_axi_io.axi_master_ar_burst_o

    // Connect AXI channels - R
    axi_io.axi_r_ready_o := Arbiter.arbiter_top_axi_io.axi_master_r_ready_o
    Arbiter.arbiter_top_axi_io.axi_master_r_valid_i := axi_io.axi_r_valid_i
    Arbiter.arbiter_top_axi_io.axi_master_r_resp_i := axi_io.axi_r_resp_i
    Arbiter.arbiter_top_axi_io.axi_master_r_data_i := axi_io.axi_r_data_i
    Arbiter.arbiter_top_axi_io.axi_master_r_last_i := axi_io.axi_r_last_i
    Arbiter.arbiter_top_axi_io.axi_master_r_id_i := axi_io.axi_r_id_i

    // Connect AXI channels - AW
    LSU.lsu_axi_io.axi_aw_ready_i := axi_io.axi_aw_ready_i
    axi_io.axi_aw_valid_o := LSU.lsu_axi_io.axi_aw_valid_o
    axi_io.axi_aw_addr_o := LSU.lsu_axi_io.axi_aw_addr_o
    axi_io.axi_aw_id_o := LSU.lsu_axi_io.axi_aw_id_o
    axi_io.axi_aw_len_o := LSU.lsu_axi_io.axi_aw_len_o
    axi_io.axi_aw_size_o := LSU.lsu_axi_io.axi_aw_size_o
    axi_io.axi_aw_burst_o := LSU.lsu_axi_io.axi_aw_burst_o

    // Connect AXI channels - W
    LSU.lsu_axi_io.axi_w_ready_i := axi_io.axi_w_ready_i
    axi_io.axi_w_valid_o := LSU.lsu_axi_io.axi_w_valid_o
    axi_io.axi_w_data_o := LSU.lsu_axi_io.axi_w_data_o
    axi_io.axi_w_strb_o := LSU.lsu_axi_io.axi_w_strb_o
    axi_io.axi_w_last_o := LSU.lsu_axi_io.axi_w_last_o

    // Connect AXI channels - B
    axi_io.axi_b_ready_o := LSU.lsu_axi_io.axi_b_ready_o
    LSU.axi_io.axi_b_valid_i := axi_io.axi_b_valid_i
    LSU.axi_io.axi_b_resp_i := axi_io.axi_b_resp_i
    LSU.axi_io.axi_b_id_i := axi_io.axi_b_id_i

    // Stage update
    NPC_State_Register := MuxCase(NPC_State_Register,
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
        (NPC_State_Register === NPC_State.NPC_State_EX) -> Mux(
            EXU.exu_internal_io.exu_internal_valid_o,
            NPC_State.NPC_State_LS,
            NPC_State.NPC_State_EX
        ),

        // when LS is finish, continue to WB
        (NPC_State_Register === NPC_State.NPC_State_LS) -> Mux(
            LSU.lsu_internal_io.lsu_internal_valid_o,
            NPC_State.NPC_State_WB,
            NPC_State.NPC_State_LS
        ),

        // when WB is finish, set to idle, so next cycle will start (WB is one-cycle only)
        (NPC_State_Register === NPC_State.NPC_State_WB) -> Mux(
            WBU.lsu_internal_io.lsu_internal_valid_o,
            NPC_State.NPC_State_Idle,
            NPC_State.NPC_State_WB
        ),

        // If the state is stop (ebreak), continue to ebreak and do nothing else
        (NPC_State_Register === NPC_State.NPC_State_Stop) -> (NPC_State.NPC_State_Stop)
    )
}