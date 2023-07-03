/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing 101 High School
* Copyright (c) 2023 Yusong Yan, University of Washington - Seattle
*
* YSYX-NPCA is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

package npca
import chisel3._
import chisel3.util._

class WBU extends Module{
    val io = IO(new Bundle{
        val WBU_I_EXU_ValuResult = Input(UInt(64.W))
        val WBU_I_EXU_CompResult = Input(Bool())
        val WBU_I_LSU_Result     = Input(UInt(64.W))
        val WBU_I_PC             = Input(UInt(64.W))

        val WBU_I_WriteBackType  = Input(UInt(4.W))
        val WBU_I_nextPCType     = Input(Bool())
        val WBU_I_PCJumpReason   = Input(UInt(4.W))

        val WBU_O_nextPC         = Output(UInt(64.W))

        val WBU_I_GPR_RD         = Input(UInt(5.W))
        val WBU_O_GPR_RD         = Output(UInt(5.W))
        val WBU_O_GPR_WBenable   = Output(Bool())
        val WBU_O_GPR_WBdata     = Output(UInt(64.W))

        val WBU_I_CSR_RD         = Input(UInt(12.W))
        val WBU_O_CSR_RD         = Output(UInt(12.W))
        val WBU_I_CSR_WBenable   = Input(Bool())
        val WBU_O_CSR_WBenable   = Output(Bool())
        val WBU_I_CSR_WBdata     = Input(UInt(64.W))
        val WBU_O_CSR_WBdata     = Output(UInt(64.W))

        val WBU_I_privOpcode     = Input(UInt(4.W))

        val WBU_O_CSR_mstatus_writeEnable = Output(Bool())
        val WBU_I_CSR_mstatus_readData    = Input(UInt(64.W))
        val WBU_O_CSR_mstatus_writeData   = Output(UInt(64.W))

        val WBU_O_CSR_mepc_writeEnable = Output(Bool())
        val WBU_I_CSR_mepc_readData    = Input(UInt(64.W))
        val WBU_O_CSR_mepc_writeData   = Output(UInt(64.W))

        val WBU_O_CSR_mtvec_writeEnable = Output(Bool())
        val WBU_I_CSR_mtvec_readData    = Output(UInt(64.W))
        val WBU_O_CSR_mtvec_writeData   = Output(UInt(64.W))

        val WBU_O_CSR_mcause_writeEnable = Output(Bool())
        val WBU_I_CSR_mcause_readData    = Output(UInt(64.W))
        val WBU_O_CSR_mcause_writeData   = Output(UInt(64.W))
    })

    // Write Back GPR
    io.WBU_O_GPR_RD := io.WBU_I_GPR_RD
    io.WBU_O_GPR_WBenable := MuxCase(false.B, Array(
        (io.WBU_I_WriteBackType === opcodes_writeBackGPRType.WB_GPR_NOP     ) -> (false.B),
        (io.WBU_I_WriteBackType === opcodes_writeBackGPRType.WB_GPR_EXU_Val ) -> (true.B ),
        (io.WBU_I_WriteBackType === opcodes_writeBackGPRType.WB_GPR_EXU_Bool) -> (true.B ),
        (io.WBU_I_WriteBackType === opcodes_writeBackGPRType.WB_GPR_LSU     ) -> (true.B ),
        (io.WBU_I_WriteBackType === opcodes_writeBackGPRType.WB_GPR_SNPC    ) -> (true.B )
    ))
    io.WBU_O_GPR_WBdata := MuxCase(0.U(64.W), Array(
        (io.WBU_I_WriteBackType === opcodes_writeBackGPRType.WB_GPR_NOP     ) -> (0.U(64.W)),
        (io.WBU_I_WriteBackType === opcodes_writeBackGPRType.WB_GPR_EXU_Val ) -> (io.WBU_I_EXU_ValuResult),
        (io.WBU_I_WriteBackType === opcodes_writeBackGPRType.WB_GPR_EXU_Bool) -> (Mux(io.WBU_I_EXU_CompResult, 1.U(64.W), 0.U(64.W))),
        (io.WBU_I_WriteBackType === opcodes_writeBackGPRType.WB_GPR_LSU     ) -> (io.WBU_I_LSU_Result),
        (io.WBU_I_WriteBackType === opcodes_writeBackGPRType.WB_GPR_SNPC    ) -> (io.WBU_I_PC + 4.U)
    ))

    // Write Back PC
    val static_nextPC  = io.WBU_I_PC + 4.U

    val dynamic_nextPC = MuxCase(0.U(64.W), Array(
        (io.WBU_I_PCJumpReason === opcodes_PCJumpReason.NoJumpPC  ) -> (static_nextPC),
        (io.WBU_I_PCJumpReason === opcodes_PCJumpReason.BranchInst) -> (Mux(io.WBU_I_EXU_CompResult, io.WBU_I_EXU_ValuResult, static_nextPC)),
        (io.WBU_I_PCJumpReason === opcodes_PCJumpReason.JAL_Inst  ) -> (io.WBU_I_EXU_ValuResult),
        (io.WBU_I_PCJumpReason === opcodes_PCJumpReason.JALR_Inst ) -> (io.WBU_I_EXU_ValuResult & (Cat(Fill(63, 1.U(1.W)), 0.U(1.W)))),
        (io.WBU_I_PCJumpReason === opcodes_PCJumpReason.ECALL_Inst) -> (io.WBU_I_CSR_mtvec_readData),
        (io.WBU_I_PCJumpReason === opcodes_PCJumpReason.MRET_Inst ) -> (io.WBU_I_CSR_mepc_readData),
    ))

    io.WBU_O_nextPC := MuxCase(0.U(64.W), Array(
        (io.WBU_I_nextPCType === opcodes_nextPCTypes.PC_Next_Static ) -> (static_nextPC ),
        (io.WBU_I_nextPCType === opcodes_nextPCTypes.PC_Next_Dynamic) -> (dynamic_nextPC)
    ))

    // Write Back CSR (Zicsr Instructions)
    io.WBU_O_CSR_RD       := io.WBU_I_CSR_RD
    io.WBU_O_CSR_WBenable := io.WBU_I_CSR_WBenable
    io.WBU_O_CSR_WBdata   := io.WBU_I_CSR_WBdata

    // Write Back CSR (Priv Instructions)
    // CSR manipulations I: write enable
    io.WBU_O_CSR_mstatus_writeEnable := MuxCase(false.B, Array(
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_nope ) -> (false.B),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_ecall) -> (true.B ),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_mret ) -> (false.B)
    ))

    io.WBU_O_CSR_mepc_writeEnable := MuxCase(false.B, Array(
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_nope ) -> (false.B),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_ecall) -> (true.B ),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_mret ) -> (false.B)
    ))

    io.WBU_O_CSR_mtvec_writeEnable := MuxCase(false.B, Array(
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_nope ) -> (false.B),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_ecall) -> (false.B),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_mret ) -> (false.B)
    ))

    io.WBU_O_CSR_mcause_writeEnable := MuxCase(false.B, Array(
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_nope ) -> (false.B),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_ecall) -> (true.B ),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_mret ) -> (false.B)
    ))

    // CSR manipulations II: write back data
    io.WBU_O_CSR_mstatus_writeData := MuxCase(0.U(64.W), Array(
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_nope ) -> (0.U(64.W)),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_ecall) -> ("ha00001800".asUInt),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_mret ) -> (0.U(64.W))
    ))

    io.WBU_O_CSR_mepc_writeData := MuxCase(0.U(64.W), Array(
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_nope ) -> (0.U(64.W)),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_ecall) -> (io.WBU_I_PC),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_mret ) -> (0.U(64.W))
    ))

    io.WBU_O_CSR_mtvec_writeData := MuxCase(0.U(64.W), Array(
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_nope ) -> (0.U(64.W)),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_ecall) -> (0.U(64.W)),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_mret ) -> (0.U(64.W))
    ))

    io.WBU_O_CSR_mcause_writeData := MuxCase(0.U(64.W), Array(
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_nope ) -> (0.U(64.W)),
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_ecall) -> (11.U(64.W)), // M state sys intr's NO is 11
        (io.WBU_I_privOpcode === opcodes_IDU_privOps.priv_mret ) -> (0.U(64.W))
    ))
}