/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing 101 High School
* Copyright (c) 2023 Yusong Yan, University of Washington - Seattle
*
* YSYX-NPC is licensed under Mulan PSL v2.
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

package npc.units

import chisel3._
import chisel3.util._

import npc.helper.defs.Base._
import npc.helper.opcode.OpEXU._

object iExecuteInternal extends Bundle{
    // ON-PIPELINE VALUES
    val oSlaveReady = Output(Bool())
    val iSlaveValid = Input(Bool())
    
    val iMasterReady = Input(Bool())
    val oMasterValid = Output(Bool())

    val iDecodeBundle = Input(UInt(DecodeWidth.W))
    val oDecodeBundle = Output(UInt(DecodeWidth.W))

    val iEXU_SRC1 = Input(UInt(DataWidth.W))
    val iEXU_SRC2 = Input(UInt(DataWidth.W))
    val iLSU_SRC2 = Input(UInt(DataWidth.W))
    val oLSU_SRC2 = Output(UInt(DataWidth.W))

    val oEXU_RET = Output(UInt(DataWidth.W))
    val iRD = Input(UInt(RegIDWidth.W))
    val oRD = Output(UInt(RegIDWidth.W))

    val iPC = Input(UInt(AddrWidth.W))
    val oPC = Ouptut(UInt(AddrWidth.W))
}

class EXU extends Module{
    val ioInternal = IO(new iExecuteInternal)

    val EXU_NotBusy = RegInit(true.B)

    val ExecuteResult = 0.U(DataWidth.W)
    val iExecuteEnable = true.B
    val iExecuteOPcode = ioInternal.iDecodeBundle(13, 7)

    val SRC1 = ioInternal.iEXU_SRC1
    val SRC2 = ioInternal.iEXU_SRC2

    def WordSignExt(WordVal : UInt) = Cat(Fill(WordWidth, WordVal(WordWidth - 1).asUInt), WordVal (WordWidth - 1, 0))
    def WordCut(DoubleVal : UInt) = DoubleVal(WordWidth - 1, 0)

    // Only can execute arthmetic if Master (IDU) 's output is valid
    Mux(ioInternal.iSlaveValid.asBool, iExecuteEnable := true.B, iExecuteEnable := false.B)

    when(iExecuteEnable.asBool){
        ExecuteResult := MuxCase(0.U(DataWidth.W), Array(
            iExecuteOPcode === EX_NOP -> (0.U(DataWidth.W)),
            iExecuteOPcode === EX_PS1 -> (SRC1),
            iExecuteOPcode === EX_ADD -> (SRC1 + SRC2),
            iExecuteOPcode === EX_SLT -> Mux(SRC1.asSInt < SRC2.asSInt, 1.U(DataWidth.W), 0.U(DataWidth.W)),
            iExecuteOPcode === EX_SLTU -> Mux(SRC1.asUInt < SRC2.asUInt, 1.U(DataWidth.W), 0.U(DataWidth.W)),
            iExecuteOPcode === EX_XOR -> (SRC1 ^ SRC2),
            iExecuteOPcode === EX_OR -> (SRC1 | SRC2),
            iExecuteOPcode === EX_AND -> (SRC1 & SRC2),
            iExecuteOPcode === EX_SLL -> (SRC1.asUInt << SRC2(5, 0)),
            iExecuteOPcode === EX_SRL -> (SRC1.asUInt >> SRC2(5, 0)),
            iExecuteOPcode === EX_SRA -> (SRC1.asSInt >> SRC2(5, 0)),
            iExecuteOPcode === EX_SUB -> (SRC1 - SRC2),

            iExecuteOPcode === EX_ADDW -> (WordSignExt(WordCut(WordCut(SRC1) + WordCut(SRC2)))),
            iExecuteOPcode === EX_SUBW -> (WordSignExt(WordCut(WordCut(SRC1) - WordCut(SRC2)))),
            iExecuteOPcode === EX_SLLW -> (WordSignExt(WordCut(WordCut(SRC1).asUInt << SRC2(4, 0)))),
            iExecuteOPcode === EX_SRLW -> (WordSignExt(WordCut(WordCut(SRC1).asUInt >> SRC2(4, 0)))),
            iExecuteOPcode === EX_SRAW -> (WordSignExt(WordCut(WordCut(SRC1).asSInt >> SRC2(4, 0)))),

            iExecuteOPcode === EX_MUL -> (SRC1 * SRC2)(DataWidth.W - 1, 0),
            iExecuteOPcode === EX_MULH -> (SRC1.asSInt * SRC2.asSInt)(2 * DataWidth - 1, DataWidth),
            iExecuteOPcode === EX_MULHSU -> (SRC1.asSInt * SRC2.asUInt)(2 * DataWidth - 1, DataWidth),
            iExecuteOPcode === EX_MULHU -> (SRC1.asUInt * SRC2.asUInt)(2 * DataWidth - 1, DataWidth),
            iExecuteOPcode === EX_DIV -> (SRC1 / SRC2),
            iExecuteOPcode === EX_DIVU -> (SRC1.asUInt / SRC2.asUInt),
            iExecuteOPcode === EX_REM -> (SRC1 % SRC2),
            iExecuteOPcode === EX_REMU -> (SRC1.asUInt % SRC2.asUInt),

            iExecuteOPcode === EX_MULW -> (WordSignExt(WordCut(WordCut(SRC1) * WordCut(SRC2)))),
            iExecuteOPcode === EX_DIVW -> (WordSignExt(WordCut(WordCut(SRC1).asSInt / WordCut(SRC2).asSInt))),
            iExecuteOPcode === EX_DIVUW -> (WordSignExt(WordCut(WordCut(SRC1).asUInt / WordCut(SRC2).asUInt))),
            iExecuteOPcode === EX_REMW -> (WordSignExt(WordCut(WordCut(SRC1).asSInt % WordCut(SRC2).asSInt))),
            iExecuteOPcode === EX_REMUW -> (WordSignExt(WordCut(WordCut(SRC1).asUInt % WordCut(SRC2).asUInt)))
        ))
    }

    ioInternal.oEXU_RET := ExecuteResult

    ioInternal.oDecodeBundle := ioInternal.iDecodeBundle
    ioInternal.oLSU_SRC2 := ioInternal.iLSU_SRC2
    ioInternal.oRD := ioInternal.iRD
    ioInternal.oPC := ioInternal.iPC

    ioInternal.oSlaveReady := EXU_NotBusy.asBool && ioInternal.iMasterReady.asBool
    ioInternal.oMasterValid := EXU_NotBusy.asBool && iExecuteEnable.asBool
}