package npc

import chisel3._
import chisel3.util._

class NPC_Arbiter_AXI_IO_Slave extends Bundle {
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

class NPC_Arbiter_AXI_IO_Master extends Bundle {
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

object NPC_Arbiter_State {
    val NPC_Arbiter_State_Idle = 0.U(2.W)
    val NPC_Arbiter_State_ConnectToIFU = 1.U(2.W)
    val NPC_Arbiter_State_ConnectToLSU = 2.U(2.W)
}

class NPC_Arbiter extends Module {
    // IO - Initiate 2 slave and 1 master
    val arbiter_ifu_axi_io = IO(new NPC_Arbiter_AXI_IO_Slave)
    val arbiter_lsu_axi_io = IO(new NPC_Arbiter_AXI_IO_Slave)

    val arbiter_top_axi_io = IO(new NPC_Arbiter_AXI_IO_Master)

    // Initiate Arbiter state register
    val NPC_Arbiter_State_Register = RegInit(NPC_Arbiter_State.NPC_Arbiter_State_Idle)

    // Connect to IFU - AR
    arbiter_ifu_axi_io.axi_slave_ar_ready_o := Mux(
        NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU,
        arbiter_top_axi_io.axi_master_ar_ready_i,
        false.B
    )

    // Connect to IFU - R
    arbiter_ifu_axi_io.axi_slave_r_valid_o := Mux(
        NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU,
        arbiter_top_axi_io.axi_master_r_valid_i,
        false.B
    )

    arbiter_ifu_axi_io.axi_slave_r_resp_o := Mux(
        NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU,
        arbiter_top_axi_io.axi_master_r_resp_i,
        0.U(2.W)
    )

    arbiter_ifu_axi_io.axi_slave_r_data_o := Mux(
        NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU,
        arbiter_top_axi_io.axi_master_r_data_i,
        0.U(64.W)
    )

    arbiter_ifu_axi_io.axi_slave_r_last_o := Mux(
        NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU,
        arbiter_top_axi_io.axi_master_r_last_i,
        false.B
    )

    arbiter_ifu_axi_io.axi_slave_r_id_o := Mux(
        NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU,
        arbiter_top_axi_io.axi_master_r_id_i,
        0.U(4.W)
    )

    // Connect to LSU - AR
    arbiter_lsu_axi_io.axi_slave_ar_ready_o := Mux(
        NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU,
        arbiter_top_axi_io.axi_master_ar_ready_i,
        false.B
    )

    // Connect to LSU - R
    arbiter_lsu_axi_io.axi_slave_r_valid_o := Mux(
        NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU,
        arbiter_top_axi_io.axi_master_r_valid_i,
        false.B
    )

    arbiter_lsu_axi_io.axi_slave_r_resp_o := Mux(
        NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU,
        arbiter_top_axi_io.axi_master_r_resp_i,
        0.U(2.W)
    )

    arbiter_lsu_axi_io.axi_slave_r_data_o := Mux(
        NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU,
        arbiter_top_axi_io.axi_master_r_data_i,
        0.U(64.W)
    )

    arbiter_lsu_axi_io.axi_slave_r_last_o := Mux(
        NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU,
        arbiter_top_axi_io.axi_master_r_last_i,
        false.B
    )

    arbiter_lsu_axi_io.axi_slave_r_id_o := Mux(
        NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU,
        arbiter_top_axi_io.axi_master_r_id_i,
        0.U(4.W)
    )

    // Connect to Top - AR
    arbiter_top_axi_io.axi_master_ar_valid_o := MuxCase(false.B, Array(
        // Case 1: Not connected
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_Idle) -> (false.B),

        // Case 2: Connected to IFU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU) -> (arbiter_ifu_axi_io.axi_slave_ar_valid_i),

        // Case 3: Connected to LSU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU) -> (arbiter_lsu_axi_io.axi_slave_ar_valid_i),
    ))

    arbiter_top_axi_io.axi_master_ar_addr_o := MuxCase(0.U(32.W), Array(
        // Case 1: Not connected
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_Idle) -> (0.U(32.W)),

        // Case 2: Connected to IFU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU) -> (arbiter_ifu_axi_io.axi_slave_ar_addr_i),

        // Case 3: Connected to LSU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU) -> (arbiter_lsu_axi_io.axi_slave_ar_addr_i),
    ))

    arbiter_top_axi_io.axi_master_ar_id_o := MuxCase(0.U(4.W), Array(
        // Case 1: Not connected
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_Idle) -> (0.U(4.W)),

        // Case 2: Connected to IFU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU) -> (arbiter_ifu_axi_io.axi_slave_ar_id_i),

        // Case 3: Connected to LSU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU) -> (arbiter_lsu_axi_io.axi_slave_ar_id_i),
    ))

    arbiter_top_axi_io.axi_master_ar_len_o := MuxCase(0.U(8.W), Array(
        // Case 1: Not connected
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_Idle) -> (0.U(8.W)),

        // Case 2: Connected to IFU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU) -> (arbiter_ifu_axi_io.axi_slave_ar_len_i),

        // Case 3: Connected to LSU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU) -> (arbiter_lsu_axi_io.axi_slave_ar_len_i),
    ))

    arbiter_top_axi_io.axi_master_ar_size_o := MuxCase(0.U(3.W), Array(
        // Case 1: Not connected
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_Idle) -> (0.U(3.W)),

        // Case 2: Connected to IFU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU) -> (arbiter_ifu_axi_io.axi_slave_ar_size_i),

        // Case 3: Connected to LSU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU) -> (arbiter_lsu_axi_io.axi_slave_ar_size_i),
    ))

    arbiter_top_axi_io.axi_master_ar_burst_o := MuxCase(0.U(2.W), Array(
        // Case 1: Not connected
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_Idle) -> (0.U(2.W)),

        // Case 2: Connected to IFU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU) -> (arbiter_ifu_axi_io.axi_slave_ar_burst_i),

        // Case 3: Connected to LSU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU) -> (arbiter_lsu_axi_io.axi_slave_ar_burst_i),
    ))

    // Connect to Top - R
    arbiter_top_axi_io.axi_master_r_ready_o := MuxCase(false.B, Array(
        // Case 1: Not connected
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_Idle) -> (false.B),

        // Case 2: Connected to IFU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU) -> (arbiter_ifu_axi_io.axi_slave_r_ready_i),

        // Case 3: Connected to LSU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU) -> (arbiter_lsu_axi_io.axi_slave_r_ready_i),
    ))

    // Arbiter State management
    NPC_Arbiter_State_Register := MuxCase(NPC_Arbiter_State_Register, Array(
        // Case 1: Still in idle
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_Idle) -> (MuxCase(NPC_Arbiter_State_Register, Array(
            // Sub-case 1: no request
            ((!arbiter_ifu_axi_io.axi_slave_ar_valid_i) && (!arbiter_lsu_axi_io.axi_slave_ar_valid_i)) -> (NPC_Arbiter_State.NPC_Arbiter_State_Idle),

            // Sub-case 2: IFU request only
            ((arbiter_ifu_axi_io.axi_slave_ar_valid_i) && (!arbiter_lsu_axi_io.axi_slave_ar_valid_i))  -> (NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU),

            // Sub-case 3: LSU request only
            ((!arbiter_ifu_axi_io.axi_slave_ar_valid_i) && (arbiter_lsu_axi_io.axi_slave_ar_valid_i))  -> (NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU),

            // Sub-case 4: IFU and LSU request (should be wrong for a multi cycle without pipeline) - connect to LSU to avoid starvation
            ((arbiter_ifu_axi_io.axi_slave_ar_valid_i) && (arbiter_lsu_axi_io.axi_slave_ar_valid_i))   -> (NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU),
        ))),

        // Case 2: Connected to IFU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU) -> (Mux(
            (arbiter_ifu_axi_io.axi_slave_r_ready_i && arbiter_top_axi_io.axi_master_r_valid_i),
            NPC_Arbiter_State.NPC_Arbiter_State_ConnectToIFU, // shake hand not completed, still connected
            NPC_Arbiter_State.NPC_Arbiter_State_Idle // shake hand complete, disconnect
        )),

        // Case 3: Connected to LSU
        (NPC_Arbiter_State_Register === NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU) -> (Mux(
            (arbiter_lsu_axi_io.axi_slave_r_ready_i && arbiter_top_axi_io.axi_master_r_valid_i),
            NPC_Arbiter_State.NPC_Arbiter_State_ConnectToLSU, // shake hand not completed, still connected
            NPC_Arbiter_State.NPC_Arbiter_State_Idle // shake hand complete, disconnect
        ))
    ))
}