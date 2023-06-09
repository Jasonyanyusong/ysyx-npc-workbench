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

class EXU extends Module{
    val io = IO(new Bundle{
        val EXU_I_src1 = Input(UInt(64.W)) // get from NPC by readGPR()
        val EXU_I_src2 = Input(UInt(64.W)) // get from NPC by readGPR()
        val EXU_I_imm  = Input(UInt(64.W)) // get from IDU
        val EXU_I_cPC  = Input(UInt(64.W)) // get from NPC

        val EXU_I_Int_opcode = Input(UInt(4.W))
        val EXU_I_Int_sign = Input(UInt(2.W))
        val EXU_I_Int_computeLength = Input(UInt(2.W))
        val EXU_I_Int_resultPart = Input(Bool())
        val EXU_I_Int_operand = Input(UInt(4.W))

        val EXU_I_csrOp        = Input(UInt(4.W))
        //val EXU_I_privOp       = Input(UInt(4.W))
        val EXU_I_csrValu      = Input(UInt(64.W))
        val EXU_O_csrWriteBack = Output(UInt(64.W))

        val EXU_O_BoolResult = Output(Bool())
        val EXU_O_ValuResult = Output(UInt(64.W))
    })

    // When Zicsr instructions are added, we will first assume this is not a CSR related inst,
    // Then if it is a csr inst, we will re-assign value for EXU_O_ValuResult
    // It is the Instruction-Decode Unit's role to decide where to write back

    // Stage I: Get the correct operand
    val Get_ValuOperand1 = MuxCase(0.U(64.W), Array(
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_TwoReg) -> (io.EXU_I_src1).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_RegImm) -> (io.EXU_I_src1).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_OneImm) -> (io.EXU_I_imm ).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_PCwReg) -> (io.EXU_I_cPC ).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_PCwImm) -> (io.EXU_I_cPC ).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_UseAll) -> (io.EXU_I_cPC ).asUInt
    ))

    val Get_ValuOperand2 = MuxCase(0.U(64.W), Array(
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_TwoReg) -> (io.EXU_I_src2).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_RegImm) -> (io.EXU_I_imm ).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_OneImm) -> (0.U(64.W)    ).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_PCwReg) -> (io.EXU_I_src1).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_PCwImm) -> (io.EXU_I_imm ).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_UseAll) -> (io.EXU_I_imm ).asUInt
    ))
    
    val Val_CompareValu1 = MuxCase(0.U(64.W), Array(
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_TwoReg) -> (io.EXU_I_src1).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_RegImm) -> (io.EXU_I_src1).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_OneImm) -> (0.U(64.W)    ).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_PCwReg) -> (0.U(64.W)    ).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_PCwImm) -> (0.U(64.W)    ).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_UseAll) -> (io.EXU_I_src1).asUInt
    ))

    val Val_CompareValu2 = MuxCase(0.U(64.W), Array(
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_TwoReg) -> (io.EXU_I_src2).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_RegImm) -> (io.EXU_I_imm ).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_OneImm) -> (0.U(64.W)    ).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_PCwReg) -> (0.U(64.W)    ).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_PCwImm) -> (0.U(64.W)    ).asUInt,
        (io.EXU_I_Int_operand === opcodes_EXU_Int_opreand.Int_UseAll) -> (io.EXU_I_src2).asUInt
    ))

    // Stage II: Bit-Cut manipulation, distinguish operating with "Word" and "Double Word"
    val Val_ValuOperand1 = MuxCase(0.U(64.W), Array(
        (io.EXU_I_Int_computeLength === opcodes_EXU_Int_computeLength.Int_Word  ) -> (Get_ValuOperand1(31, 0)).asUInt,
        //(io.EXU_I_Int_sign === opcodes_EXU_Int_computeLength.Int_Word  ) -> (Get_ValuOperand1(31, 0)).asUInt,
        (io.EXU_I_Int_computeLength === opcodes_EXU_Int_computeLength.Int_Double) -> (Get_ValuOperand1(63, 0)).asUInt
    ))

    val Val_ValuOperand2 = MuxCase(0.U(64.W), Array(
        (io.EXU_I_Int_computeLength === opcodes_EXU_Int_computeLength.Int_Word  ) -> (Get_ValuOperand2(31, 0)).asUInt,
        //(io.EXU_I_Int_sign === opcodes_EXU_Int_computeLength.Int_Word  ) -> (Get_ValuOperand2(31, 0)).asUInt,
        (io.EXU_I_Int_computeLength === opcodes_EXU_Int_computeLength.Int_Double) -> (Get_ValuOperand2(63, 0)).asUInt
    ))

    // Stage III: Compute, by distinguishing sign
    val ValuRes_Signed_Signed = MuxCase(0.U(64.W), Array(
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_NOP) -> (Val_ValuOperand1.asSInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_SHL) -> (Val_ValuOperand1.asSInt << Val_ValuOperand2(5, 0).asUInt ).asUInt,
        //(io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_SHR) -> (Val_ValuOperand1.asSInt >> Val_ValuOperand2(5, 0).asUInt ).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_SHR) -> (MuxCase(0.U(64.W), Array((io.EXU_I_Int_computeLength === opcodes_EXU_Int_computeLength.Int_Word) -> (Val_ValuOperand1(31, 0).asSInt >> Val_ValuOperand2(5, 0)).asUInt, (io.EXU_I_Int_computeLength === opcodes_EXU_Int_computeLength.Int_Double) -> (Val_ValuOperand1(63, 0).asSInt >> Val_ValuOperand2(5, 0)).asUInt))).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_ADD) -> (Val_ValuOperand1.asSInt +  Val_ValuOperand2.asSInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_SUB) -> (Val_ValuOperand1.asSInt -  Val_ValuOperand2.asSInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_MUL) -> (Val_ValuOperand1.asSInt *  Val_ValuOperand2.asSInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_DIV) -> (Mux(Val_ValuOperand2.asSInt === 0.asSInt, ("hffffffffffffffff".U).asSInt, MuxCase(("hffffffffffffffff".U).asSInt, Array((io.EXU_I_Int_computeLength === opcodes_EXU_Int_computeLength.Int_Word) -> (Val_ValuOperand1(31, 0).asSInt / Val_ValuOperand2(31, 0).asSInt).asSInt, (io.EXU_I_Int_computeLength === opcodes_EXU_Int_computeLength.Int_Double) -> (Val_ValuOperand1.asSInt / Val_ValuOperand2.asSInt).asSInt)))).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_REM) -> (Mux(Val_ValuOperand2.asSInt === 0.asSInt, Val_ValuOperand1.asSInt,        MuxCase(Val_ValuOperand1.asSInt       , Array((io.EXU_I_Int_computeLength === opcodes_EXU_Int_computeLength.Int_Word) -> (Val_ValuOperand1(31, 0).asSInt % Val_ValuOperand2(31, 0).asSInt).asSInt, (io.EXU_I_Int_computeLength === opcodes_EXU_Int_computeLength.Int_Double) -> (Val_ValuOperand1.asSInt % Val_ValuOperand2.asSInt).asSInt)))).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_AND) -> (Val_ValuOperand1.asSInt &  Val_ValuOperand2.asSInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_OR ) -> (Val_ValuOperand1.asSInt |  Val_ValuOperand2.asSInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_XOR) -> (Val_ValuOperand1.asSInt ^  Val_ValuOperand2.asSInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BEQ) -> (Val_ValuOperand1.asUInt +  Val_ValuOperand2.asUInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BNE) -> (Val_ValuOperand1.asUInt +  Val_ValuOperand2.asUInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BLT) -> (Val_ValuOperand1.asUInt +  Val_ValuOperand2.asUInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BGE) -> (Val_ValuOperand1.asUInt +  Val_ValuOperand2.asUInt).asUInt
    ))

    val ValuRes_Unsigned_Unsigned = MuxCase(0.U(64.W), Array(
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_NOP) -> (Val_ValuOperand1.asUInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_SHL) -> (Val_ValuOperand1.asUInt << Val_ValuOperand2(5, 0) ).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_SHR) -> (Val_ValuOperand1.asUInt >> Val_ValuOperand2(5, 0) ).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_ADD) -> (Val_ValuOperand1.asUInt +  Val_ValuOperand2.asUInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_SUB) -> (Val_ValuOperand1.asUInt -  Val_ValuOperand2.asUInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_MUL) -> (Val_ValuOperand1.asUInt *  Val_ValuOperand2.asUInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_DIV) -> (Mux(Val_ValuOperand2.asUInt === 0.asUInt, ("hffffffffffffffff".U).asUInt, Val_ValuOperand1.asUInt / Val_ValuOperand2.asUInt)).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_REM) -> (Mux(Val_ValuOperand2.asUInt === 0.asUInt, Val_ValuOperand1.asUInt,        Val_ValuOperand1.asUInt % Val_ValuOperand2.asUInt)).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_AND) -> (Val_ValuOperand1.asUInt &  Val_ValuOperand2.asUInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_OR ) -> (Val_ValuOperand1.asUInt |  Val_ValuOperand2.asUInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_XOR) -> (Val_ValuOperand1.asUInt ^  Val_ValuOperand2.asUInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BEQ) -> (Val_ValuOperand1.asUInt +  Val_ValuOperand2.asUInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BNE) -> (Val_ValuOperand1.asUInt +  Val_ValuOperand2.asUInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BLT) -> (Val_ValuOperand1.asUInt +  Val_ValuOperand2.asUInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BGE) -> (Val_ValuOperand1.asUInt +  Val_ValuOperand2.asUInt).asUInt
    ))

    val ValuRes_Signed_Unsigned = MuxCase(0.U(64.W), Array(
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_NOP) -> (Val_ValuOperand1.asSInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_SHL) -> (Val_ValuOperand1.asSInt << Val_ValuOperand2(5, 0) ).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_SHR) -> (Val_ValuOperand1.asSInt >> Val_ValuOperand2(5, 0) ).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_MUL) -> (Val_ValuOperand1.asSInt *  Val_ValuOperand2.asUInt).asUInt
    ))

    val ValuRes_Unsigned_Signed = MuxCase(0.U(64.W), Array(
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_NOP) -> (Val_ValuOperand1.asUInt).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_SHL) -> (Val_ValuOperand1.asUInt << Val_ValuOperand2(5, 0) ).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_SHR) -> (Val_ValuOperand1.asUInt >> Val_ValuOperand2(5, 0) ).asUInt,
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_MUL) -> (Val_ValuOperand1.asUInt *  Val_ValuOperand2.asSInt).asUInt
    ))

    val CompRes_Signed_Signed = MuxCase(false.B, Array(
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BEQ) -> (Val_CompareValu1.asSInt === Val_CompareValu2.asSInt),
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BNE) -> (Val_CompareValu1.asSInt =/= Val_CompareValu2.asSInt),
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BLT) -> (Val_CompareValu1.asSInt <   Val_CompareValu2.asSInt),
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BGE) -> (Val_CompareValu1.asSInt >=  Val_CompareValu2.asSInt)
    ))

    val CompRes_Unsigned_Unsigned = MuxCase(false.B, Array(
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BEQ) -> (Val_CompareValu1.asUInt === Val_CompareValu2.asUInt),
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BNE) -> (Val_CompareValu1.asUInt =/= Val_CompareValu2.asUInt),
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BLT) -> (Val_CompareValu1.asUInt <   Val_CompareValu2.asUInt),
        (io.EXU_I_Int_opcode === opcodes_EXU_Int.Int_BGE) -> (Val_CompareValu1.asUInt >=  Val_CompareValu2.asUInt)
    ))

    val CompRes_Signed_Unsigned = false.B

    val CompRes_Unsigned_Signed = false.B

    // Stage IV: Bit cut for Upper or Lower part of result
    val Pre_ValuRes = MuxCase(0.U(128.W), Array(
        (io.EXU_I_Int_sign === opcodes_EXU_Int_sign.Int_Signed_Signed    ) -> (ValuRes_Signed_Signed    ),
        (io.EXU_I_Int_sign === opcodes_EXU_Int_sign.Int_Unsigned_Unsigned) -> (ValuRes_Unsigned_Unsigned),
        (io.EXU_I_Int_sign === opcodes_EXU_Int_sign.Int_Signed_Unsigned  ) -> (ValuRes_Signed_Unsigned  ),
        (io.EXU_I_Int_sign === opcodes_EXU_Int_sign.Int_Unsigned_Signed  ) -> (ValuRes_Unsigned_Signed  )
    ))

    val Pre_ValuRet = MuxCase(Pre_ValuRes, Array(
        (io.EXU_I_Int_computeLength === opcodes_EXU_Int_computeLength.Int_Double) -> (Pre_ValuRes),
        (io.EXU_I_Int_computeLength === opcodes_EXU_Int_computeLength.Int_Word  ) -> (Cat(Fill(32, Pre_ValuRes(31)), Pre_ValuRes(31, 0)))
    ))

    val Ret_ValuRes = MuxCase(0.U(64.W), Array(
        (io.EXU_I_Int_resultPart === opcodes_EXU_Int_resultPart.Int_Low ) -> (Pre_ValuRet(63 , 0 )),
        (io.EXU_I_Int_resultPart === opcodes_EXU_Int_resultPart.Int_High) -> (Pre_ValuRet(127, 64))
    ))

    val Ret_CompRes = MuxCase(false.B, Array(
        (io.EXU_I_Int_sign === opcodes_EXU_Int_sign.Int_Signed_Signed    ) -> (CompRes_Signed_Signed    ),
        (io.EXU_I_Int_sign === opcodes_EXU_Int_sign.Int_Unsigned_Unsigned) -> (CompRes_Unsigned_Unsigned),
        (io.EXU_I_Int_sign === opcodes_EXU_Int_sign.Int_Signed_Unsigned  ) -> (CompRes_Signed_Unsigned  ),
        (io.EXU_I_Int_sign === opcodes_EXU_Int_sign.Int_Unsigned_Signed  ) -> (CompRes_Unsigned_Signed  )
    ))

    // Stage V: Write back to output port
    io.EXU_O_BoolResult := Ret_CompRes
    io.EXU_O_ValuResult := Ret_ValuRes

    // Now a normal compute step is over, we will preform priv compute

    val OldCSR = io.EXU_I_csrValu.asUInt // CSRRS and CSRRC

    io.EXU_O_csrWriteBack := MuxCase(0.U(64.W), Array(
        (io.EXU_I_csrOp === opcodes_IDU_csrOps.csr_nope ) -> (0.U(64.W).asUInt      ),
        (io.EXU_I_csrOp === opcodes_IDU_csrOps.csr_write) -> (io.EXU_I_src1         ),
        (io.EXU_I_csrOp === opcodes_IDU_csrOps.csr_set  ) -> (OldCSR | io.EXU_I_src1),
        (io.EXU_I_csrOp === opcodes_IDU_csrOps.csr_clear) -> (OldCSR & io.EXU_I_src1)
    ))

    // Finally, check if this is a csr instruction, if so, re-assign io.EXU_O_ValuResult to OldCSR
    io.EXU_O_ValuResult := MuxCase(Ret_ValuRes, Array(
        (io.EXU_I_csrOp === opcodes_IDU_csrOps.csr_write) -> (OldCSR),
        (io.EXU_I_csrOp === opcodes_IDU_csrOps.csr_set  ) -> (OldCSR),
        (io.EXU_I_csrOp === opcodes_IDU_csrOps.csr_clear) -> (OldCSR)
    ))
}