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
import npc.helper.opcode.OpLSULen._
import npc.helper.opcode.OpLSUfunc._

object iLoadStoreInternal extends Bundle{
    // ON-PIPELINE VALUES
    val oSlaveReady = Output(Bool())
    val iSlaveValid = Input(Bool())
    
    val iMasterReady = Input(Bool())
    val oMasterValid = Output(Bool())

    val iDecodeBundle = Input(UInt(DecodeWidth.W))
    val oDecodeBundle = Output(UInt(DecodeWidth.W))

    val iEXU_RET = Input(UInt(DataWidth.W))
    val iLSU_SRC2 = Input(UInt(DataWidth.W))

    val oEXU_RET = Output(UInt(DataWidth.W))
    val oLSU_RET = Output(UInt(DataWidth.W))

    val iRD = Input(UInt(RegIDWidth.W))
    val oRD = Output(UInt(RegIDWidth.W))
}

object iLoadStoreExternal extends Bundle{
    val oMemoryOP = Output(UInt(2.W))
    val oMemoryAddr = Output(UInt(AddrWidth.W))
    val oMemoryWrite = Output(UInt(DataWidth.W))
    val iMemoryRead = Input(UInt(DataWidth.W))
}

class LSU extends Module{
    // TODO: implement pipelined-LSU
    val ioInternal = IO(new iLoadStoreInternal)
    val ioExternal = IO(new iLoadStoreExternal)

    val LSU_NotBusy = RegInit(true.B)
    val LoadStoreResult = 0.U(DataWidth.W)
    val iLoadStoreEnable = true.B
    val iLoadStoreLen = ioInternal.iDecodeBundle(6, 5)
    val iLoadStoreFunc = ioInternal.iDecodeBundle(4, 3)

    def WordSignExt(WordVal : UInt(WordWidth.W)) = Cat(Fill(DataWidth - WordWidth, WordVal(WordWidth - 1).asUInt), WordVal (WordWidth - 1, 0))
    def HalfSignExt(HalfVal : UInt(HalfWidth.W)) = Cat(Fill(DataWidth - HalfWidth, HalfVal(HalfWidth - 1).asUInt), HalfVal (HalfWidth - 1, 0))
    def ByteSignExt(ByteVal : UInt(ByteWidth.W)) = Cat(Fill(ByteWidth - ByteWidth, ByteVal(ByteWidth - 1).asUInt), ByteVal (ByteWidth - 1, 0))

    def WordZeroExt(WordVal : UInt(WordWidth.W)) = Cat(Fill(DataWidth - WordWidth, 0.U(1.W)), WordVal (WordWidth - 1, 0))
    def HalfZeroExt(HalfVal : UInt(HalfWidth.W)) = Cat(Fill(DataWidth - HalfWidth, 0.U(1.W)), HalfVal (HalfWidth - 1, 0))
    def ByteZeroExt(ByteVal : UInt(ByteWidth.W)) = Cat(Fill(ByteWidth - ByteWidth, 0.U(1.W)), ByteVal (ByteWidth - 1, 0))

    Mux(ioInternal.iSlaveValid.asBool, iLoadStoreEnable := true.B, iLoadStoreEnable := false.B)

    when(iLoadStoreEnable.asBool){
        LoadStoreResult := MuxCase(0.U(DataWidth.W), Array(
            // TODO: Add more Load-Store type
        ))
    }

    ioInternal.oDecodeBundle := ioInternal.iDecodeBundle
    ioInternal.oEXU_RET := ioInternal.iEXU_RET
    ioInternal.oRD := ioInternal.iRD

    // TODO: Pipeline shake hand implementation
}