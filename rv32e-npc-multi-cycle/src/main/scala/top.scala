package npc

import chisel3._
import chisel3.util._

object TOP_Defs {
    // This is how many cycle there will be delay for 
    val TOP_AXI_DELAY_CYCLES = 4
}

class TOP_MEM_IO extends Bundle {
    // read
    val mem_r_addr_o = Output(UInt(32.W))
    val mem_r_size_o = Output(UInt(2.W))
    val mem_r_data_i = Input(UInt(32.W))
    val mem_r_enable_o = Output(Bool())
    val mem_r_valid_i = Input(Bool())

    // write
    val mem_w_addr_o = Output(UInt(32.W))
    val mem_w_size_o = Output(UInt(2.W))
    val mem_w_data_o = Output(UInt(32.W))
    val mem_w_enable_o = Output(Bool())
    val mem_w_valid_i = Input(Bool())
}

class TOP_DEBUG_IO extends Bundle {
    val debug_gpr_00_o = Output(UInt(32.W))
    val debug_gpr_01_o = Output(UInt(32.W))
    val debug_gpr_02_o = Output(UInt(32.W))
    val debug_gpr_03_o = Output(UInt(32.W))
    val debug_gpr_04_o = Output(UInt(32.W))
    val debug_gpr_05_o = Output(UInt(32.W))
    val debug_gpr_06_o = Output(UInt(32.W))
    val debug_gpr_07_o = Output(UInt(32.W))
    val debug_gpr_08_o = Output(UInt(32.W))
    val debug_gpr_09_o = Output(UInt(32.W))
    val debug_gpr_10_o = Output(UInt(32.W))
    val debug_gpr_11_o = Output(UInt(32.W))
    val debug_gpr_12_o = Output(UInt(32.W))
    val debug_gpr_13_o = Output(UInt(32.W))
    val debug_gpr_14_o = Output(UInt(32.W))
    val debug_gpr_15_o = Output(UInt(32.W))

    val debug_pc_o = Output(UInt(32.W))

    val debug_commit_o = Output(Bool())
    val debug_ebreak_o = Output(Bool())
}

class YSYX_TOP extends Module {
    // IO (direct/atomic memory operations)
    val mem_io = IO(new TOP_MEM_IO)

    // IO debug (difftest)
    val top_debug_io = IO(new TOP_DEBUG_IO)

    // Initiate NPC
    val Core_0 = Module(new NPC)

    // AR channel
    Core_0.axi_io.axi_ar_ready_i := true.B

    // R channel
    Core_0.axi_io.axi_r_valid_i := ShiftRegister(mem_io.mem_r_valid_i, TOP_Defs.TOP_AXI_DELAY_CYCLES)
    Core_0.axi_io.axi_r_resp_i := ShiftRegister(0.U(2.W), TOP_Defs.TOP_AXI_DELAY_CYCLES)
    Core_0.axi_io.axi_r_data_i := ShiftRegister(
        Cat(Fill(32, 0.U(1.W)), mem_io.mem_r_data_i),
        TOP_Defs.TOP_AXI_DELAY_CYCLES
    )
    Core_0.axi_io.axi_r_last_i := ShiftRegister(true.B, TOP_Defs.TOP_AXI_DELAY_CYCLES)
    Core_0.axi_io.axi_r_id_i := ShiftRegister(Core_0.axi_io.axi_ar_id_o, TOP_Defs.TOP_AXI_DELAY_CYCLES)

    // AW channel
    Core_0.axi_io.axi_aw_ready_i := true.B

    // W channel
    Core_0.axi_io.axi_w_ready_i := true.B

    // B channel
    Core_0.axi_io.axi_b_valid_i := ShiftRegister(mem_io.mem_w_valid_i, TOP_Defs.TOP_AXI_DELAY_CYCLES)
    Core_0.axi_io.axi_b_resp_i := ShiftRegister(0.U(2.W), TOP_Defs.TOP_AXI_DELAY_CYCLES)
    Core_0.axi_io.axi_b_id_i := ShiftRegister(Core_0.axi_io.axi_aw_id_o, TOP_Defs.TOP_AXI_DELAY_CYCLES)

    // Connect to simulation IO
    mem_io.mem_r_addr_o := Core_0.axi_io.axi_ar_addr_o
    mem_io.mem_r_size_o := Core_0.axi_io.axi_ar_size_o
    mem_io.mem_r_enable_o := Core_0.axi_io.axi_ar_valid_o

    mem_io.mem_w_addr_o := Core_0.axi_io.axi_aw_addr_o
    mem_io.mem_w_size_o := Core_0.axi_io.axi_aw_size_o
    mem_io.mem_w_data_o := Core_0.axi_io.axi_w_data_o(31, 0)
    mem_io.mem_w_enable_o := Core_0.axi_io.axi_aw_valid_o && Core_0.axi_io.axi_w_valid_o

    // IO for debugging
    top_debug_io.debug_commit_o := Core_0.debug_io.debug_commit_o
    top_debug_io.debug_ebreak_o := Core_0.debug_io.debug_ebreak_o
    top_debug_io.debug_pc_o := Core_0.debug_io.debug_pc_o
    top_debug_io.debug_gpr_00_o := Core_0.debug_io.debug_gpr_00_o
    top_debug_io.debug_gpr_01_o := Core_0.debug_io.debug_gpr_01_o
    top_debug_io.debug_gpr_02_o := Core_0.debug_io.debug_gpr_02_o
    top_debug_io.debug_gpr_03_o := Core_0.debug_io.debug_gpr_03_o
    top_debug_io.debug_gpr_04_o := Core_0.debug_io.debug_gpr_04_o
    top_debug_io.debug_gpr_05_o := Core_0.debug_io.debug_gpr_05_o
    top_debug_io.debug_gpr_06_o := Core_0.debug_io.debug_gpr_06_o
    top_debug_io.debug_gpr_07_o := Core_0.debug_io.debug_gpr_07_o
    top_debug_io.debug_gpr_08_o := Core_0.debug_io.debug_gpr_08_o
    top_debug_io.debug_gpr_09_o := Core_0.debug_io.debug_gpr_09_o
    top_debug_io.debug_gpr_10_o := Core_0.debug_io.debug_gpr_10_o
    top_debug_io.debug_gpr_11_o := Core_0.debug_io.debug_gpr_11_o
    top_debug_io.debug_gpr_12_o := Core_0.debug_io.debug_gpr_12_o
    top_debug_io.debug_gpr_13_o := Core_0.debug_io.debug_gpr_13_o
    top_debug_io.debug_gpr_14_o := Core_0.debug_io.debug_gpr_14_o
    top_debug_io.debug_gpr_15_o := Core_0.debug_io.debug_gpr_15_o
}