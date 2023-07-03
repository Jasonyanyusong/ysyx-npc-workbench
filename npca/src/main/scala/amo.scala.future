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

package npca_future
import chisel3._
import chisel3.util._

class AMO extends Module{
    val io = IO(new Bundle{
        AMO_I_src1 = Input(UInt(64.W))
        AMO_I_src2 = Input(UInt(64.W))

        AMO_I_memOps = Input(UInt(4.W))
        AMO_I_opLen  = Input(UInt(2.W))
        AMO_I_aqrl   = Input(UInt(2.W))

        AMO_O_readMemEnable = Output(Bool())
        AMO_O_readMemAddr   = Output(UInt(64.W))
        AMO_I_readMemData   = Input(UInt(64.W))

        AMO_O_writeMemEnable = Output(Bool())
        AMO_O_writeMemAddr   = Output(UInt(64.W))
        AMO_O_writeMemData   = Output(UInt(64.W))

        AMO_O_MemOpLength   = Output(UInt(2.W))
        AMO_O_ValuWriteBack = Output(UInt(64.W))
    })

    // Get two operands, the first one is memory address, the second one is operate value
    val AMO_SRC1 = io.AMO_I_src1
    val AMO_SRC2 = io.AMO_I_src2

    // Process the Memory IO port
    io.AMO_O_readMemEnable  := MuxCase(false.B, Array(
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_nope) -> (false.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_lr)   -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_sc)   -> (false.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_swap) -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_add)  -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_xor)  -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_and)  -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_or)   -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_min)  -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_max)  -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_minu) -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_maxu) -> (true.B),
    ))

    io.AMO_O_writeMemEnable := MuxCase(false.B, Array(
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_nope) -> (false.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_lr)   -> (false.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_sc)   -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_swap) -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_add)  -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_xor)  -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_and)  -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_or)   -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_min)  -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_max)  -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_minu) -> (true.B),
        (io.AMO_I_memOps === opcodes_AMO_memOps.amo_maxu) -> (true.B),
    ))

    io.AMO_O_readMemAddr  := AMO_SRC1
    io.AMO_O_writeMemAddr := AMO_SRC1

    io.AMO_O_MemOpLength := io.AMO_I_opLen

    val AMO_ReadMem = io.AMO_I_readMemData // This is also the old val for AMO operations
    val AMO_ReadMemDoubleRT = AMO_ReadMem
    val AMO_ReadMemWordSEXT = Cat(Fill(32, AMO_ReadMem(31, 31)), AMO_ReadMem(31, 0))

    val AMO_AMOWriteBack = MuxCase(0.U(64.W), Array(
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Word)   -> (AMO_ReadMemWordSEXT),
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Double) -> (AMO_ReadMemDoubleRT)
    ))

    val AMO_SWAP = MuxCase(0.U(64.W), Array(
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Word)   -> (AMO_SRC2(31, 0)),
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Double) -> (AMO_SRC2(63, 0))
    ))

    val AMO_ADD = MuxCase(0.U(64.W), Array(
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Word)   -> (AMO_SRC2(31, 0) + AMO_ReadMem(31, 0)),
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Double) -> (AMO_SRC2(63, 0) + AMO_ReadMem(63, 0))
    ))

    val AMO_XOR = MuxCase(0.U(64.W), Array(
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Word)   -> (AMO_SRC2(31, 0) ^ AMO_ReadMem(31, 0)),
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Double) -> (AMO_SRC2(63, 0) ^ AMO_ReadMem(63, 0))
    ))

    val AMO_AND = MuxCase(0.U(64.W), Array(
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Word)   -> (AMO_SRC2(31, 0) & AMO_ReadMem(31, 0)),
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Double) -> (AMO_SRC2(63, 0) & AMO_ReadMem(63, 0))
    ))

    val AMO_OR = MuxCase(0.U(64.W), Array(
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Word)   -> (AMO_SRC2(31, 0) | AMO_ReadMem(31, 0)),
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Double) -> (AMO_SRC2(63, 0) | AMO_ReadMem(63, 0))
    ))

    val AMO_MIN = MuxCase(0.U(64.W), Array(
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Word)   -> (Mux(AMO_SRC2(31, 0).asSInt < AMO_ReadMem(31, 0).asSInt, AMO_SRC2(31, 0), AMO_ReadMem(31, 0))),
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Double) -> (Mux(AMO_SRC2(63, 0).asSInt < AMO_ReadMem(63, 0).asSInt, AMO_SRC2(63, 0), AMO_ReadMem(63, 0)))
    ))

    val AMO_MAX = MuxCase(0.U(64.W), Array(
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Word)   -> (Mux(AMO_SRC2(31, 0).asSInt > AMO_ReadMem(31, 0).asSInt, AMO_SRC2(31, 0), AMO_ReadMem(31, 0))),
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Double) -> (Mux(AMO_SRC2(63, 0).asSInt > AMO_ReadMem(63, 0).asSInt, AMO_SRC2(63, 0), AMO_ReadMem(63, 0)))
    ))

    val AMO_MINU = MuxCase(0.U(64.W), Array(
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Word)   -> (Mux(AMO_SRC2(31, 0).asUInt < AMO_ReadMem(31, 0).asUInt, AMO_SRC2(31, 0), AMO_ReadMem(31, 0))),
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Double) -> (Mux(AMO_SRC2(63, 0).asUInt < AMO_ReadMem(63, 0).asUInt, AMO_SRC2(63, 0), AMO_ReadMem(63, 0)))
    ))

    val AMO_MAXU = MuxCase(0.U(64.W), Array(
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Word)   -> (Mux(AMO_SRC2(31, 0).asUInt > AMO_ReadMem(31, 0).asUInt, AMO_SRC2(31, 0), AMO_ReadMem(31, 0))),
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Double) -> (Mux(AMO_SRC2(63, 0).asUInt > AMO_ReadMem(63, 0).asUInt, AMO_SRC2(63, 0), AMO_ReadMem(63, 0)))
    ))

    val SRC2Write = MuxCase(0.U(64.W), Array(
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Word)   -> (AMO_SRC2(31, 0)),
        (io.AMO_I_opLen === opcodes_AMO_opLen.amoLen_Double) -> (AMO_SRC2(63, 0))
    ))

    io.AMO_O_writeMemData := MuxCase(0.U(64.W), Array(
        (io.AMO_I_memOps === amo_nope) -> (0.U(64.W)),
        (io.AMO_I_memOps === amo_lr)   -> (0.U(64.W)),
        (io.AMO_I_memOps === amo_sc)   -> (SRC2Write),
        (io.AMO_I_memOps === amo_swap) -> (AMO_SWAP),
        (io.AMO_I_memOps === amo_add)  -> (AMO_ADD),
        (io.AMO_I_memOps === amo_xor)  -> (AMO_XOR),
        (io.AMO_I_memOps === amo_and)  -> (AMO_AND),
        (io.AMO_I_memOps === amo_or)   -> (AMO_OR),
        (io.AMO_I_memOps === amo_min)  -> (AMO_MIN),
        (io.AMO_I_memOps === amo_max)  -> (AMO_MAX),
        (io.AMO_I_memOps === amo_minu) -> (AMO_MINU),
        (io.AMO_I_memOps === amo_maxu) -> (AMO_MAXU),
    ))

    io.AMO_O_ValuWriteBack := MuxCase(0.U(64.W), Array(
        (io.AMO_I_memOps === amo_nope) -> (0.U(64.W)),
        (io.AMO_I_memOps === amo_lr)   -> (AMO_AMOWriteBack),
        (io.AMO_I_memOps === amo_sc)   -> (0.U(64.W)),
        (io.AMO_I_memOps === amo_swap) -> (AMO_AMOWriteBack),
        (io.AMO_I_memOps === amo_add)  -> (AMO_AMOWriteBack),
        (io.AMO_I_memOps === amo_xor)  -> (AMO_AMOWriteBack),
        (io.AMO_I_memOps === amo_and)  -> (AMO_AMOWriteBack),
        (io.AMO_I_memOps === amo_or)   -> (AMO_AMOWriteBack),
        (io.AMO_I_memOps === amo_min)  -> (AMO_AMOWriteBack),
        (io.AMO_I_memOps === amo_max)  -> (AMO_AMOWriteBack),
        (io.AMO_I_memOps === amo_minu) -> (AMO_AMOWriteBack),
        (io.AMO_I_memOps === amo_maxu) -> (AMO_AMOWriteBack),
    ))
}