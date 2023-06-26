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

class LSU extends Module{
    val io = IO(new Bundle{
        val LSU_I_optype           = Input(UInt(4.W))
        val LSU_I_sign             = Input(Bool())
        val LSU_I_len              = Input(UInt(2.W))
        val LSU_I_memOperationType = Input(UInt(2.W))

        val LSU_I_EXUresult        = Input(UInt(64.W)) // Normal Load-Store Instructions
        val LSU_I_src1             = Input(UInt(64.W)) // Atomic Load-Store Instructions
        val LSU_I_src2             = Input(UInt(64.W))

        val LSU_O_memReadEnable    = Output(Bool())
        val LSU_O_memReadAddress   = Output(UInt(64.W))
        val LSU_O_memReadLength    = Output(UInt(2.W))
        val LSU_I_memReadResult    = Input(UInt(64.W))

        val LSU_O_memWriteEnable   = Output(Bool())
        val LSU_O_memWriteAddress  = Output(UInt(64.W))
        val LSU_O_memWriteLength   = Output(UInt(2.W))
        val LSU_O_memWriteData     = Output(UInt(64.W))

        val LSU_O_resultWriteBack  = Output(UInt(64.W))
    })

    // LSU Read Memory opertations
    io.LSU_O_memReadEnable := MuxCase(false.B, Array(
        (io.LSU_I_memOperationType === opcodes_LSU_memOpreationType.LSU_R)  -> (true.B),
        (io.LSU_I_memOperationType === opcodes_LSU_memOpreationType.LSU_RW) -> (true.B)
    ))

    io.LSU_O_memReadAddress := MuxCase(0.U(64.W), Array(
        (io.LSU_I_memOperationType === opcodes_LSU_memOpreationType.LSU_R)  -> (io.LSU_I_EXUresult),
        (io.LSU_I_memOperationType === opcodes_LSU_memOpreationType.LSU_RW) -> (io.LSU_I_EXUresult)
    ))

    io.LSU_O_memReadLength := io.LSU_I_len

    val ReadMemInput = MuxCase(0.U(64.W), Array(
        (io.LSU_I_memOperationType === opcodes_LSU_memOpreationType.LSU_R)  -> (io.LSU_I_memReadResult),
        (io.LSU_I_memOperationType === opcodes_LSU_memOpreationType.LSU_RW) -> (io.LSU_I_memReadResult)
    ))

    val LoadStoreUnsignedMask = MuxCase(0.U(64.W), Array(
        (io.LSU_I_len === opcodes_LSU_len.LSU_Byte  ) -> (Cat(Fill(56, 0.U), Fill( 8, 1.U))),
        (io.LSU_I_len === opcodes_LSU_len.LSU_Half  ) -> (Cat(Fill(48, 0.U), Fill(16, 1.U))),
        (io.LSU_I_len === opcodes_LSU_len.LSU_Word  ) -> (Cat(Fill(32, 0.U), Fill(32, 1.U))),
        (io.LSU_I_len === opcodes_LSU_len.LSU_Double) -> (Cat(Fill( 0, 0.U), Fill(64, 1.U)))
    ))

    val LoadStoreSignedMask = MuxCase(0.U(64.W), Array(
        (io.LSU_I_len === opcodes_LSU_len.LSU_Byte  ) -> (Cat(Fill(56, ReadMemInput( 7)), Fill( 8, 0.U))),
        (io.LSU_I_len === opcodes_LSU_len.LSU_Half  ) -> (Cat(Fill(48, ReadMemInput(15)), Fill(16, 0.U))),
        (io.LSU_I_len === opcodes_LSU_len.LSU_Word  ) -> (Cat(Fill(32, ReadMemInput(31)), Fill(32, 0.U))),
        (io.LSU_I_len === opcodes_LSU_len.LSU_Double) -> (Cat(Fill( 0, ReadMemInput(63)), Fill(64, 0.U)))
    ))

    io.LSU_O_resultWriteBack := MuxCase(0.U(64.W), Array(
        (io.LSU_I_sign === opcodes_LSU_sign.LSU_Signed)   -> (LoadStoreSignedMask   | ReadMemInput),
        (io.LSU_I_sign === opcodes_LSU_sign.LSU_Unsigned) -> (LoadStoreUnsignedMask & ReadMemInput),
    ))

    // LSU Write Memory operations
    io.LSU_O_memWriteEnable := MuxCase(false.B, Array(
        (io.LSU_I_memOperationType === opcodes_LSU_memOpreationType.LSU_W)  -> (true.B),
        (io.LSU_I_memOperationType === opcodes_LSU_memOpreationType.LSU_RW) -> (true.B)
    ))

    io.LSU_O_memWriteAddress := MuxCase(0.U(64.W), Array(
        (io.LSU_I_memOperationType === opcodes_LSU_memOpreationType.LSU_W)  -> (io.LSU_I_EXUresult),
        (io.LSU_I_memOperationType === opcodes_LSU_memOpreationType.LSU_RW) -> (io.LSU_I_EXUresult)
    ))

    io.LSU_O_memWriteData := MuxCase(0.U(64.W), Array(
        (io.LSU_I_memOperationType === opcodes_LSU_memOpreationType.LSU_W)  -> (io.LSU_I_src2),
        (io.LSU_I_memOperationType === opcodes_LSU_memOpreationType.LSU_RW) -> (io.LSU_I_src2)
    ))

    io.LSU_O_memWriteLength := io.LSU_I_len
}