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
import npc.helper.rv64im.Inst._
import npc.helper.opcode.OpEXU._
import npc.helper.opcode.OpLSU._
import npc.helper.opcode.OpWBU._

object iDecodeInternal extends Bundle{
    // ON-PIPELINE VALUES
    val oSlaveReady = Output(Bool())
    val iSlaveValid = Input(Bool())

    val iMasterReady = Input(Bool())
    val oMasterValid = Output(Bool())

    val iInst = Input(UInt(InstWidth.W))
    val oDynamicNextPC = Output(UInt(AddrWidth.W))
    val oStaticNextPC = Output(UInt(AddrWidth.W))
    
    // Combine EXU, LSU, WBU 's decode information together
    // 1   1   1   1   1   1   0   0   0   0   0   0   0   0   0   0
    // 5   4   3   2   1   0   9   8   7   6   5   4   3   2   1   0
    // |---|   |-----------------------|   |---|   |---|   |---|   |
    // Priv               EXU              LSlen   LSfunc  WBTyp  Debug State (DS)
    val oDecodeBundle = Output(UInt(DecodeWidth.W))

    val EX_src1 = Output(UInt(DataWidth.W))
    val EX_src2 = Output(UInt(DataWidth.W))
    // Load-Store's src1 is EXU's result
    val LS_src2 = Output(UInt(DataWidth.W))

    // OFF-PIPELINE VALUES
    val CSR_FetchAddr = Output(UInt(CSRIDWidth.W)) // RV have 4096 CSRs
    val CSR_FetchRetu = Input(UInt(DataWidth.W))
    val CSR_ChangeVal = Output(UInt(DataWidth.W))

    val oRS1 = Output(UInt(RegIDWidth.W))
    val oRS2 = Output(UInt(RegIDWidth.W))
    val oRD  = Output(UInt(RegIDWidth.W))

    val iSRC1 = Input(UInt(DataWidth.W))
    val iSRC2 = Input(UInt(DataWidth.W))
}

class IDU extends Module{
    ioInternal = IO(new iDecodeInternal)
    // Instruction Decode Unit: First generate opreation code, then send operands to EXU and LSU (for operand send to LSU, it needs to be delivered to EXU first), last write back PC and CSR (WBU only write back GPR)
    // TODO: Write IDU Logic

    val DecodeReg = RegInit(0.U(DecodeWidth.W))
    val iDecodeEnable = RegInit(true.B)

    val iDecPrivReg = RegInit(0.U(2.W))
    val iDecEXUReg = RegInit(0.U(7.W))
    val iDecLSlenReg = RegInit(0.U(2.W))
    val iDecLSfuncReg = RegInit(0.U(2.W))
    val iDecWBTypReg = RegInit(0.U(2.W))
    val iDecDSReg = RegInit(0.U(1.W))

    // Only can decode instruction if Master (IFU) 's output is valid
    Mux(ioInternal.iSlaveValid.asBool, iDecodeEnable := true.B, iDecodeEnable := false.B)

    // Decode PrivReg
    Mux(iDecodeEnable.asBool, iDecPrivReg := 0.U(2.W), iDecPrivReg := Lookup(ioInternal.iInst, 0.U(2.W), Array(ECALL -> 1.U(2.W), MRET -> 2.U(2.W), EBREAK -> 3.U(2.W))))

    // Decode EXUReg
    Mux(iDecodeEnable.asBool, /*TODO: Add decode behaviors*/)

    // Decode LSlenReg
    Mux(iDecodeEnable.asBool, iDecLSlenReg := 0.U(2.W), iDecLSlenReg := Lookup(
        ioInternal.iInst, 0.U(2.W), Array(
            LB -> 0.U(2.W), LBU -> 0.U(2.W), SB -> 0.U(2.W),
            LH -> 1.U(2.W), LHU -> 1.U(2.W), SH -> 1.U(2.W),
            LW -> 2.U(2.W), LWU -> 2.U(2.W), SW -> 2.U(2.W),
            LD -> 3.U(2.W),                  SD -> 3.U(2.W)
        )
    ))

    // Decode LSfuncReg
    Mux(iDecodeEnable.asBool, /*TODO: Add decode behaviors*/)

    // Decode WBTypReg
    Mux(iDecodeEnable.asBool, /*TODO: Add decode behaviors*/)

    // Decode DSReg
    Mux(iDecodeEnable.asBool, /*TODO: Add decode behaviors*/)

    // Combine these decode results together when iDecodeEnable is true
    Mux(iDecodeEnable.asBool, /*TODO: Add decode behaviors*/)

    // Disable instruction decoding
    iDecodeEnable := false.B

    ioInternal.oDecodeBundle := DecodeReg
    // TODO: connect more decode signals

    ioInternal.oMasterValid := true.B

    when(ioInternal.iMasterReady.asBool){
        // Shake hand success, re-enable iDecode, decode next instruction if IFU have result
        iDecodeEnable := true.B
    }
}