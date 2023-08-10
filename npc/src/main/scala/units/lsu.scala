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

package npc.units.iLoadStore

import chisel3._
import chisel3.util._

import npc.helper.defs.Base._
import npc.helper.opcode.OpLSULen._
import npc.helper.opcode.OpLSUFunc._
import npc.helper.opcode.MemOp._

class iLoadStoreInternal extends Bundle{
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

    val iPC = Input(UInt(AddrWidth.W))
    val oPC = Output(UInt(AddrWidth.W))
}

class iLoadStoreExternal extends Bundle{
    val oMemoryOP = Output(UInt(2.W))
    val oMemoryAddr = Output(UInt(AddrWidth.W))
    val oMemoryWrite = Output(UInt(DataWidth.W))
    val iMemoryRead = Input(UInt(DataWidth.W))
}

class LSU extends Module{
    val ioInternal = IO(new iLoadStoreInternal)
    val ioExternal = IO(new iLoadStoreExternal)

    val LSU_NotBusy = RegInit(true.B)
    val LoadStoreResult = RegInit(0.U(DataWidth.W))

    val iLoadStoreLen = RegInit(LS_B) // Default is LS_B since its value is 0
    val iLoadStoreFunc = RegInit(LS_NOP)
    val iLoadStoreAddr = RegInit(0.U(AddrWidth.W))
    val iLoadStoreMemOP = RegInit(MEM_NOP)
    val iLoadStoreWrite = RegInit(0.U(DataWidth.W))

    val DecodeBundle = ioInternal.iDecodeBundle
    val EXU_RET = ioInternal.iEXU_RET
    val LSU_SRC = ioInternal.iLSU_SRC2

    val LD_RET = RegInit(0.U(DataWidth.W))

    def WordSignExt(WordVal : UInt) = Cat(Fill(DataWidth - WordWidth, WordVal(WordWidth - 1).asUInt), WordVal(WordWidth - 1, 0))
    def HalfSignExt(HalfVal : UInt) = Cat(Fill(DataWidth - HalfWidth, HalfVal(HalfWidth - 1).asUInt), HalfVal(HalfWidth - 1, 0))
    def ByteSignExt(ByteVal : UInt) = Cat(Fill(DataWidth - ByteWidth, ByteVal(ByteWidth - 1).asUInt), ByteVal(ByteWidth - 1, 0))

    def WordZeroExt(WordVal : UInt) = Cat(Fill(DataWidth - WordWidth, 0.U(1.W)), WordVal (WordWidth - 1, 0))
    def HalfZeroExt(HalfVal : UInt) = Cat(Fill(DataWidth - HalfWidth, 0.U(1.W)), HalfVal (HalfWidth - 1, 0))
    def ByteZeroExt(ByteVal : UInt) = Cat(Fill(DataWidth - ByteWidth, 0.U(1.W)), ByteVal (ByteWidth - 1, 0))

    when(ioInternal.iSlaveValid.asBool && ioInternal.iMasterReady.asBool){
        iLoadStoreLen := DecodeBundle(6, 5)
        iLoadStoreFunc := DecodeBundle(4, 3)
        iLoadStoreAddr := EXU_RET
        iLoadStoreMemOP := MuxCase(MEM_NOP, Array(
            (iLoadStoreFunc === LS_LD) -> MEM_READ,
            (iLoadStoreFunc === LS_LDU) -> MEM_READ,
            (iLoadStoreFunc === LS_ST) -> MEM_WRITE
        ))
        iLoadStoreWrite := LSU_SRC
    }

    // Connect IO External
    ioExternal.oMemoryOP := iLoadStoreMemOP
    ioExternal.oMemoryAddr := iLoadStoreAddr
    ioExternal.oMemoryWrite := iLoadStoreWrite
    LD_RET := ioExternal.iMemoryRead

    when(ioInternal.iSlaveValid.asBool && ioInternal.iMasterReady.asBool){
        LoadStoreResult :=MuxCase(0.U(DataWidth.W), Array(
            (iLoadStoreFunc === LS_LD) -> (MuxCase(0.U(DataWidth.W), Array(
                (iLoadStoreLen === LS_B) -> ByteSignExt(LD_RET.asUInt),
                (iLoadStoreLen === LS_H) -> HalfSignExt(LD_RET.asUInt),
                (iLoadStoreLen === LS_W) -> WordSignExt(LD_RET.asUInt),
                (iLoadStoreLen === LS_D) -> LD_RET
            ))),
            (iLoadStoreFunc === LS_LDU) -> (MuxCase(0.U(DataWidth.W), Array(
                (iLoadStoreLen === LS_B) -> ByteZeroExt(LD_RET.asUInt),
                (iLoadStoreLen === LS_H) -> HalfZeroExt(LD_RET.asUInt),
                (iLoadStoreLen === LS_W) -> WordZeroExt(LD_RET.asUInt)
            )))
        ))
    }

    // Connect IO Internal
    ioInternal.oLSU_RET := LoadStoreResult

    // Connect passtorhough for WBU
    ioInternal.oDecodeBundle := ioInternal.iDecodeBundle
    ioInternal.oEXU_RET := ioInternal.iEXU_RET
    ioInternal.oRD := ioInternal.iRD
    ioInternal.oPC := ioInternal.iPC

    // Pipeline shake hand implementation
    ioInternal.oSlaveReady := (LSU_NotBusy.asBool && ioInternal.iMasterReady.asBool)
    ioInternal.oMasterValid := (LSU_NotBusy.asBool && ioInternal.iSlaveValid.asBool)
}