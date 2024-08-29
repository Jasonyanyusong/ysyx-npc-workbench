package npc

import chisel3._
import chisel3.util._

object NPC_LSU_LS_Ops {
    def LS_NOP    =  0.U(4.W)
    def LS_LB     =  1.U(4.W)
    def LS_LH     =  2.U(4.W)
    def LS_LW     =  3.U(4.W)
    def LS_LBU    =  4.U(4.W)
    def LS_LHU    =  5.U(4.W)
    def LS_SB     =  6.U(4.W)
    def LS_SH     =  7.U(4.W)
    def LS_SW     =  8.U(4.W)
}

class NPC_LSU_Internal_IO extends Bundle {
    val lsu_internal_work_i = Input(Bool())
    val lsu_internal_valid_o = Output(Bool())

    val lsu_internal_ls_op_i = Input(UInt(4.W))
    val lsu_internal_ls_wdata_i = Input(UInt(32.W))
    val lsu_internal_ls_result_o = Output(UInt(32.W))

    val lsu_internal_ex_result_i = Input(UInt(32.W))
    val lsu_internal_ex_result_o = Output(UInt(32.W))

    val lsu_internal_wb_opcode_i = Input(UInt(2.W))
    val lsu_internal_wb_rd_i = Input(UInt(4.W))
    val lsu_internal_wb_opcode_o = Output(UInt(2.W))
    val lsu_internal_wb_rd_o = Output(UInt(4.W))
}

class NPC_LSU_AXI_IO extends Bundle {
    // Address Read (AR)
    val axi_ar_ready_i = Input(Bool())
    val axi_ar_valid_o = Output(Bool())
    val axi_ar_addr_o = Output(UInt(32.W)) // fixed exu result
    val axi_ar_id_o = Output(UInt(4.W)) // fixed 0b0001
    val axi_ar_len_o = Output(UInt(8.W)) // fixed 0, only 1 data transfer per transaction
    val axi_ar_size_o = Output(UInt(3.W)) // according to different load instructions
    val axi_ar_burst_o = Output(UInt(2.W)) // no matter, since only 1 data transfer per transaction

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
    val axi_aw_addr_o = Output(UInt(32.W)) // fixed exu result
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

object NPC_LSU_State { // 3 bit register save current lsu state
    val NPC_LSU_State_Idle = 0.U(3.W)
    val NPC_LSU_State_SendingReadRequest = 1.U(3.W)
    val NPC_LSU_State_WaitingReadResponse = 2.U(3.W)
    val NPC_LSU_State_SendingWriteRequest = 3.U(3.W)
    val NPC_LSU_State_WaitingWriteResponse = 4.U(4.W)
}

object NPC_LSU_AWState { // 2 bit register save current AW state
    val NPC_LSU_NotWriting = 0.U(2.W)
    val NPC_LSU_AWState_Sending = 1.U(2.W)
    val NPC_LSU_AWState_SendDone = 2.U(2.W)
}

object NPC_LSU_WState { // 2 bit register save current W state
    val NPC_LSU_NotWriting = 0.U(2.W)
    val NPC_LSU_WState_Sending = 1.U(2.W)
    val NPC_LSU_WState_SendDone = 2.U(2.W)
}

class NPC_LSU extends Module {
    // IO (axi full and internal to transfer opcodes and data)
    val lsu_axi_io = IO(new NPC_LSU_AXI_IO)
    val lsu_internal_io = IO(new NPC_LSU_Internal_IO)

    // LSU state registers
    val NPC_LSU_State_Register = RegInit(NPC_LSU_State.NPC_LSU_State_Idle)
    val NPC_LSU_AWState_Register = RegInit(NPC_LSU_AWState.NPC_LSU_NotWriting)
    val NPC_LSU_WState_Register = RegInit(NPC_LSU_WState.NPC_LSU_NotWriting)

    // connect AXI IO - AR
    lsu_axi_io.axi_ar_valid_o := (NPC_LSU_State_Register === NPC_LSU_State.NPC_LSU_State_SendingReadRequest)
    lsu_axi_io.axi_ar_addr_o := lsu_internal_io.lsu_internal_ex_result_o
    lsu_axi_io.axi_ar_id_o := 1.U(4.W)
    lsu_axi_io.axi_ar_len_o := 0.U(8.W)
    lsu_axi_io.axi_ar_size_o := MuxCase(0.U(3.W), Array(
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LB) -> (0.U(3.W)), // B: 1 byte
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LH) -> (1.U(3.W)), // H: 2 bytes
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LW) -> (2.U(3.W)), // W: 4 bytes
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LBU) -> (0.U(3.W)), // BU: 1 byte
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LHU) -> (1.U(3.W)), // HU: 2 bytes
    ))
    lsu_axi_io.axi_ar_burst_o := 0.U(2.W)

    // connect AXI IO - R
    lsu_axi_io.axi_r_ready_o := (NPC_LSU_State_Register === NPC_LSU_State.NPC_LSU_State_WaitingReadResponse)
    val Load_Return = lsu_axi_io.axi_r_data_i(31, 0)
    val Load_Result = MuxCase(0.U(32.W), Array(
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LB)  -> (Cat(Fill(24, Load_Return(7)), Load_Return(7, 0))),
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LH)  -> (Cat(Fill(16, Load_Return(15)), Load_Return(15, 0))),
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LW)  -> (Load_Return),
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LBU) -> (Cat(Fill(24, 0.U(1.W)), Load_Return(7, 0))),
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LHU) -> (Cat(Fill(16, 0.U(1.W)), Load_Return(15, 0))),
    ))
    lsu_internal_io.lsu_internal_ls_result_o := Load_Result

    val LSU_SendWRequest = NPC_LSU_State_Register === NPC_LSU_State.NPC_LSU_State_SendingWriteRequest

    // connect AXI IO - AW
    lsu_axi_io.axi_aw_valid_o := (LSU_SendWRequest && NPC_LSU_AWState_Register === NPC_LSU_AWState.NPC_LSU_AWState_Sending)
    lsu_axi_io.axi_aw_addr_o := lsu_internal_io.lsu_internal_ex_result_o
    lsu_axi_io.axi_aw_id_o := 0.U(4.W) // LSU's write channel is assumed to be 0000
    lsu_axi_io.axi_aw_len_o := 0.U(8.W) // only 1 data transfer per transaction
    lsu_axi_io.axi_aw_size_o := MuxCase(0.U(3.W), Array(
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SB) -> (0.U(3.W)), // B: 1 byte
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SH) -> (1.U(3.W)), // H: 2 bytes
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SW) -> (2.U(3.W)), // W: 4 bytes
    ))
    lsu_axi_io.axi_aw_burst_o := 0.U(2.W)

    // connect AXI IO - W
    lsu_axi_io.axi_w_valid_o := (LSU_SendWRequest && NPC_LSU_WState_Register === NPC_LSU_WState.NPC_LSU_WState_Sending)
    lsu_axi_io.axi_w_data_o := Cat(Fill(32, 0.U(1.W)), lsu_internal_io.lsu_internal_ls_wdata_i)
    lsu_axi_io.axi_w_strb_o := MuxCase(0.U(3.W), Array(
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SB) -> ("b00000001".asUInt), // B: last 1 byte
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SH) -> ("b00000011".asUInt), // H: last 2 bytes
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SW) -> ("b00001111".asUInt), // W: last 4 bytes
    ))
    lsu_axi_io.axi_w_last_o := true.B // only 1 data transfer per transaction, so always true

    // connect AXI IO - B
    lsu_axi_io.axi_b_ready_o := (NPC_LSU_State_Register === NPC_LSU_State.NPC_LSU_State_WaitingWriteResponse)

    // connect internal signals
    lsu_internal_io.lsu_internal_valid_o := lsu_internal_io.lsu_internal_work_i && MuxCase(false.B, Array(
        // Case 1: LSU does not have to work in this cycle
        (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_NOP) -> (true.B),

        // Case 2: Waiting for read response and R channel get data
        (NPC_LSU_State_Register === NPC_LSU_State.NPC_LSU_State_WaitingReadResponse && lsu_axi_io.axi_r_valid_i) -> (true.B),

        // Case 3: Waiting for write response and B channel get write response (later will need to consider successed write resp)
        (NPC_LSU_State_Register === NPC_LSU_State.NPC_LSU_State_WaitingWriteResponse && lsu_axi_io.axi_b_valid_i) -> (true.B)
    ))

    lsu_internal_io.lsu_internal_ex_result_o := lsu_internal_io.lsu_internal_ex_result_i
    lsu_internal_io.lsu_internal_wb_opcode_o := lsu_internal_io.lsu_internal_wb_opcode_i
    lsu_internal_io.lsu_internal_wb_rd_o := lsu_internal_io.lsu_internal_wb_rd_i

    // update state registers - overall state register
    NPC_LSU_State_Register := Mux(lsu_internal_io.lsu_internal_work_i, MuxCase(NPC_LSU_State_Register, Array(
        // Case 1: LSU in idle state
        (NPC_LSU_State_Register === NPC_LSU_State.NPC_LSU_State_Idle) -> (MuxCase(NPC_LSU_State_Register, Array(
            // Sub-case 1: Still need to be in idle state
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_NOP) -> (NPC_LSU_State.NPC_LSU_State_Idle),

            // Sub-case 2: Switch to send R request (AR channel ON)
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LB)  -> (NPC_LSU_State.NPC_LSU_State_SendingReadRequest),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LH)  -> (NPC_LSU_State.NPC_LSU_State_SendingReadRequest),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LW)  -> (NPC_LSU_State.NPC_LSU_State_SendingReadRequest),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LBU) -> (NPC_LSU_State.NPC_LSU_State_SendingReadRequest),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LHU) -> (NPC_LSU_State.NPC_LSU_State_SendingReadRequest),

            // Sub-case 3: Switch to send W request (AW, W channel ON)
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SB)  -> (NPC_LSU_State.NPC_LSU_State_SendingWriteRequest),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SH)  -> (NPC_LSU_State.NPC_LSU_State_SendingWriteRequest),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SW)  -> (NPC_LSU_State.NPC_LSU_State_SendingWriteRequest),
        ))),

        // Case 2: LSU in Sending Read Request mode
        (NPC_LSU_State_Register === NPC_LSU_State.NPC_LSU_State_SendingReadRequest) -> (Mux(
            lsu_axi_io.axi_ar_ready_i, // check if shake-hand success
            NPC_LSU_State.NPC_LSU_State_WaitingReadResponse, // shake hand success, change to wait mode, stop valid signal for AR
            NPC_LSU_State.NPC_LSU_State_SendingReadRequest // shake hand not success yet, still set valid to be high
        )),

        // Case 3: LSU in Waiting Read Response mode
        (NPC_LSU_State_Register === NPC_LSU_State.NPC_LSU_State_WaitingReadResponse) -> (Mux(
            lsu_axi_io.axi_r_valid_i, // check if shake hand success
            NPC_LSU_State.NPC_LSU_State_Idle, // transaction finished, switch back to idle mode
            NPC_LSU_State.NPC_LSU_State_WaitingReadResponse // transaction not finished, continue to wait for response
        )),

        // Case 4: LSU in Sending Write Request mode
        (NPC_LSU_State_Register === NPC_LSU_State.NPC_LSU_State_SendingWriteRequest) -> (Mux(
            // check if both channel hand shake success
            NPC_LSU_AWState_Register === NPC_LSU_AWState.NPC_LSU_AWState_SendDone && NPC_LSU_WState_Register === NPC_LSU_WState.NPC_LSU_WState_SendDone,
            NPC_LSU_State.NPC_LSU_State_WaitingWriteResponse, // both channel shake hand success, change to wait mode, stop valid for AW, W
            NPC_LSU_State.NPC_LSU_State_SendingWriteRequest // shake hand not success yet, still set valid to some signals, depend on another FSM
        )),

        // Case 5: LSU in Waiting Write Response mode
        (NPC_LSU_State_Register === NPC_LSU_State.NPC_LSU_State_WaitingWriteResponse) -> (Mux(
            lsu_axi_io.axi_b_valid_i, // check if shake hand success
            NPC_LSU_State.NPC_LSU_State_Idle, // transaction finished, switch back to idle mode
            NPC_LSU_State.NPC_LSU_State_WaitingWriteResponse // transaction not finished, continue to wait for response
        )),
    )), NPC_LSU_State_Register)

    // update state registers - update write address channel register
    NPC_LSU_AWState_Register := Mux(lsu_internal_io.lsu_internal_work_i, MuxCase(NPC_LSU_AWState_Register, Array(
        // Case 1: Not in writing mode
        (NPC_LSU_AWState_Register === NPC_LSU_AWState.NPC_LSU_NotWriting) -> (MuxCase(NPC_LSU_AWState_Register, Array(
            // Sub-case 1: Still need not to be in writing mode because not LS instruction
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_NOP) -> (NPC_LSU_AWState.NPC_LSU_NotWriting),

            // Sub-case 2: Still need not to be in writing mode because this is load instruction
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LB)  -> (NPC_LSU_AWState.NPC_LSU_NotWriting),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LH)  -> (NPC_LSU_AWState.NPC_LSU_NotWriting),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LW)  -> (NPC_LSU_AWState.NPC_LSU_NotWriting),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LBU) -> (NPC_LSU_AWState.NPC_LSU_NotWriting),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LHU) -> (NPC_LSU_AWState.NPC_LSU_NotWriting),

            // Sub-case 3: Switch to sending AW mode
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SB)  -> (NPC_LSU_AWState.NPC_LSU_AWState_Sending),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SH)  -> (NPC_LSU_AWState.NPC_LSU_AWState_Sending),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SW)  -> (NPC_LSU_AWState.NPC_LSU_AWState_Sending),
        ))),

        // Case 2: In sending AW mode
        (NPC_LSU_AWState_Register === NPC_LSU_AWState.NPC_LSU_AWState_Sending) -> (Mux(
            lsu_axi_io.axi_aw_ready_i, // check if AW channel shake hand success
            NPC_LSU_AWState.NPC_LSU_AWState_SendDone, // shake hand success, implies send done
            NPC_LSU_AWState.NPC_LSU_AWState_Sending // send AW not finished yet
        )),

        // Case 3: When finished sending AW
        (NPC_LSU_AWState_Register === NPC_LSU_AWState.NPC_LSU_AWState_SendDone) -> (Mux(
            lsu_axi_io.axi_b_valid_i, // check if this write transaction hand shake success
            NPC_LSU_AWState.NPC_LSU_NotWriting, // transaction finished
            NPC_LSU_AWState.NPC_LSU_AWState_SendDone // still waiting
        )),
    )), NPC_LSU_AWState_Register)

    // update state registers - update write data channel register
    NPC_LSU_WState_Register := Mux(lsu_internal_io.lsu_internal_work_i, MuxCase(NPC_LSU_WState_Register, Array(
        // Case 1: Not in writing mode
        (NPC_LSU_WState_Register === NPC_LSU_WState.NPC_LSU_NotWriting) -> (MuxCase(NPC_LSU_WState_Register, Array(
            // Sub-case 1: Still need not to be in writing mode because not LS instruction
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_NOP) -> (NPC_LSU_WState.NPC_LSU_NotWriting),

            // Sub-case 2: Still need not to be in writing mode because this is load instruction
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LB)  -> (NPC_LSU_WState.NPC_LSU_NotWriting),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LH)  -> (NPC_LSU_WState.NPC_LSU_NotWriting),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LW)  -> (NPC_LSU_WState.NPC_LSU_NotWriting),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LBU) -> (NPC_LSU_WState.NPC_LSU_NotWriting),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_LHU) -> (NPC_LSU_WState.NPC_LSU_NotWriting),

            // Sub-case 3: Switch to sending W mode
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SB)  -> (NPC_LSU_WState.NPC_LSU_WState_Sending),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SH)  -> (NPC_LSU_WState.NPC_LSU_WState_Sending),
            (lsu_internal_io.lsu_internal_ls_op_i === NPC_LSU_LS_Ops.LS_SW)  -> (NPC_LSU_WState.NPC_LSU_WState_Sending),
        ))),

        // Case 2: In sending W mode
        (NPC_LSU_WState_Register === NPC_LSU_WState.NPC_LSU_WState_Sending) -> (Mux(
            lsu_axi_io.axi_w_ready_i, // check if W channel shake hand success
            NPC_LSU_WState.NPC_LSU_WState_SendDone, // shake hand success, implies send done
            NPC_LSU_WState.NPC_LSU_WState_Sending // send W not finished yet
        )),

        // Case 3: When finished sending AW
        (NPC_LSU_WState_Register === NPC_LSU_WState.NPC_LSU_WState_SendDone) -> (Mux(
            lsu_axi_io.axi_b_valid_i, // check if this write transaction hand shake success
            NPC_LSU_WState.NPC_LSU_NotWriting, // transaction finished
            NPC_LSU_WState.NPC_LSU_WState_SendDone // still waiting
        )),
    )), NPC_LSU_WState_Register)
}