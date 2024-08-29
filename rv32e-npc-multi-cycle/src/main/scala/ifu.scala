package npc

import chisel3._
import chisel3.util._

object NPC_IFU_State { // 2 bit register save current ifu state
    val NPC_IFU_State_Idle = 0.U(2.W)
    val NPC_IFU_State_SendingReadRequest = 1.U(2.W)
    val NPC_IFU_State_WaitingReadResponse = 2.U(2.W)
}

class NPC_IFU_AXI_IO extends Bundle {
    // Address Read (AR)
    val ifu_axi_ar_ready_i = Input(Bool())
    val ifu_axi_ar_valid_o = Output(Bool())
    val ifu_axi_ar_addr_o = Output(UInt(32.W))
    val ifu_axi_ar_id_o = Output(UInt(4.W)) // fixed to 0b0000 for IFU
    val ifu_axi_ar_len_o = Output(UInt(8.W)) // only 1 transfer per transaction
    val ifu_axi_ar_size_o = Output(UInt(3.W)) // fixed to 32 bit, 4 bytes
    val ifu_axi_ar_burst_o = Output(UInt(2.W)) // not used, assume any is okay

    // Read (R)
    val ifu_axi_r_ready_o = Output(Bool())
    val ifu_axi_r_valid_i = Input(Bool())
    val ifu_axi_r_resp_i = Input(UInt(2.W)) // not used, assume success
    val ifu_axi_r_data_i = Input(UInt(64.W))
    val ifu_axi_r_last_i = Input(Bool()) // not used
    val ifu_axi_r_id_i = Input(UInt(4.W)) // not used
}

class NPC_IFU_Internal_IO extends Bundle {
    val ifu_internal_pc_i = Input(UInt(32.W))
    val ifu_internal_work_i = Input(Bool())

    val ifu_internal_instValid_o = Output(Bool())
    val ifu_internal_inst_o = Output(UInt(32.W))
}

class NPC_IFU extends Module {
    // IO (axi read channels and internal to transfer PC and work state)
    val ifu_axi_io = IO(new NPC_IFU_AXI_IO)
    val ifu_internal_io = IO(new NPC_IFU_Internal_IO)

    // IFU state register
    NPC_IFU_State_Register = RegInit(NPC_IFU_State.NPC_IFU_State_Idle) // start with idle state

    // connect to AXI IO - AR
    ifu_axi_io.ifu_axi_ar_valid_o := (
        ifu_internal_io.ifu_internal_work_i && 
        NPC_IFU_State_Register === NPC_IFU_State_SendingReadRequest
    ) // the adress is valid when IFU is in working mode and sending the read request
    ifu_axi_io.ifu_axi_ar_addr_o := ifu_internal_io.ifu_internal_pc_i // the adress is always the PC
    ifu_axi_io.ifu_axi_ar_id_o := 0.U(4.W) // IFU's read channel is assumed to be 0000
    ifu_axi_io.ifu_axi_ar_len_o := 0.U(8.W) // only 1 data transfer per transaction
    ifu_axi_io.ifu_axi_ar_size_o := 2.U(3.W) // instruction is 32 bit, which is 4 bytes, thus size is 0b010
    ifu_axi_io.ifu_axi_ar_burst_o := 0.U(2.W) // no matter, since only 1 data transfer per transaction

    // connect to AXI IO - R
    ifu_axi_io.ifu_axi_r_ready_o := (
        ifu_internal_io.ifu_internal_work_i &&
        NPC_IFU_State_Register === NPC_IFU_State_WaitingReadResponse
    ) // IFU can accept read response when it is working and in the wait response state

    // connect to internal IO
    ifu_internal_io.ifu_internal_instValid_o := (
        NPC_IFU_State_Register === NPC_IFU_State_WaitingReadResponse && // the ifu is waiting for instr
        ifu_internal_io.ifu_internal_work_i && // the ifu is working
        ifu_axi_io.ifu_axi_r_valid_i // read response channel is valid signal
    ) // the instruction is valid when 1) wait for read-resp 2) working 3) read resp valid
    ifu_internal_io.ifu_internal_inst_o := ifu_axi_io.ifu_axi_r_data_i(31, 0) // lower 32 bit is valid

    // Update IFU state register
    NPC_IFU_State_Register := MuxCase(NPC_IFU_State_Register,
        (NPC_IFU_State_Register === NPC_IFU_State.NPC_IFU_State_Idle) -> (
            Mux(ifu_internal_io.ifu_internal_work_i,
                NPC_IFU_State.NPC_IFU_State_SendingReadRequest,
                NPC_IFU_State.NPC_IFU_State_Idle
            )
        ), // state management if IFU is in idle

        (NPC_IFU_State_Register === NPC_IFU_State.NPC_IFU_State_SendingReadRequest) -> (
            Mux(ifu_axi_io.ifu_axi_ar_ready_i,
                NPC_IFU_State.NPC_IFU_State_WaitingReadResponse,
                NPC_IFU_State.NPC_IFU_State_SendingReadRequest
            )
        ), // state management if IFU is sending read request

        (NPC_IFU_State_Register === NPC_IFU_State.NPC_IFU_State_WaitingReadResponse) -> (
            Mux(ifu_axi_io.ifu_axi_r_valid_i,
                NPC_IFU_State.NPC_IFU_State_Idle,
                NPC_IFU_State.NPC_IFU_State_WaitingReadResponse
            )
        ), // state management if IFU is waiting read response
    )
}