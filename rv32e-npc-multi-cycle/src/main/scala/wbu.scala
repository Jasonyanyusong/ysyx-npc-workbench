package npc

import chisel3._
import chisel3.util._

object NPC_WBU_WB_Ops {
    def WB_NOP    =  0.U(2.W)
    def WB_EXU    =  1.U(2.W)
    def WB_LSU    =  2.U(2.W)
}

class NPC_WBU_Internal_IO extends Bundle {
    val wbu_internal_work_i = Input(Bool())

    val wbu_internal_ex_ret_i = Input(UInt(32.W))
    val wbu_internal_ls_ret_i = Input(UInt(32.W))

    val wbu_internal_opcode_i = Input(UInt(2.W))
    val wbu_internal_rd_i = Input(UInt(4.W))
    val wbu_internal_rd_o = Output(UInt(4.W))
    val wbu_internal_wdata_o = Output(UInt(32.W))

    val wbu_internal_valid_o = Output(Bool())
}

class NPC_WBU extends Module {
    // IO definitions
    val wbu_internal_io = IO(new NPC_WBU_Internal_IO)

    // connect
    wbu_internal_io.wbu_internal_wdata_o := MuxCase(0.U(32.W),
        // Case 1: No Write back
        (wbu_internal_io.wbu_internal_opcode_i === NPC_WBU_WB_Ops.WB_NOP) -> (0.U(32.W)),

        // Case 2: Write back EXU result
        (wbu_internal_io.wbu_internal_opcode_i === NPC_WBU_WB_Ops.WB_EXU) -> (wbu_internal_io.wbu_internal_ex_ret_i),

        // Case 3: Write back LSU result
        (wbu_internal_io.wbu_internal_opcode_i === NPC_WBU_WB_Ops.WB_LSU) -> (wbu_internal_io.wbu_internal_ls_ret_i),
    )

    wbu_internal_io.wbu_internal_rd_o := Mux(
        wbu_internal_io.wbu_internal_opcode_i === NPC_WBU_WB_Ops.WB_NOP, // check if there is no wb, if no, then let rd = 0
        0.U(4.W), // no write back, so just write to x0, does not effect correctness
        wbu_internal_io.wbu_internal_rd_i // have write back, so just write to what input is
    )

    wbu_internal_io.wbu_internal_valid_o := wbu_internal_io.wbu_internal_work_i
}