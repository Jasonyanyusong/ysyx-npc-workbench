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
import npc.helper.defs.PipeLineDefs._

import npc.helper.opcode.OpLSULen._
import npc.helper.opcode.OpLSUFunc._
import npc.helper.opcode.MemOp._

class iLoadStoreInternal extends Bundle{
    // ON-PIPELINE VALUES
    val oSlaveReady = Output(Bool())
    val iSlaveValid = Input(Bool())
    
    val iMasterReady = Input(Bool())
    val oMasterValid = Output(Bool())

    // TODO: change singal type
    val PipeLine_EX2LS_MsgBundle = Input(UInt(PipeLine_EX2LS_Width.W))
    val PipeLine_LS2WB_MsgBundle = Output(UInt(PipeLine_LS2WB_Width.W))
    val PipeLine_LS2WB_ChangeReg = Output(Bool())

    //val iDecodeBundle = Input(UInt(DecodeWidth.W))
    //val oDecodeBundle = Output(UInt(DecodeWidth.W))

    //val iEXU_RET = Input(UInt(DataWidth.W))
    //val iLSU_SRC2 = Input(UInt(DataWidth.W))

    //val oEXU_RET = Output(UInt(DataWidth.W))
    //val oLSU_RET = Output(UInt(DataWidth.W))

    //val iRD = Input(UInt(RegIDWidth.W))
    //val oRD = Output(UInt(RegIDWidth.W))

    //val iPC = Input(UInt(AddrWidth.W))
    //val oPC = Output(UInt(AddrWidth.W))
}

class iLoadStoreExternal extends Bundle{
    val oMemoryOP = Output(UInt(2.W))
    val oMemoryLen = Output(UInt(2.W))
    val oMemoryAddr = Output(UInt(AddrWidth.W))
    val oMemoryWrite = Output(UInt(DataWidth.W))
    val iMemoryRead = Input(UInt(DataWidth.W))
}

class LSU extends Module{
    val ioInternal = IO(new iLoadStoreInternal)
    val ioExternal = IO(new iLoadStoreExternal)

    val LSU_NotBusy = RegInit(true.B)

    val PipeLine_EX2LS_Bundle = new Bundle{
        val Instr = UInt(InstWidth.W)
        val PC = UInt(AddrWidth.W)
        val DecodeVal = UInt(DecodeWidth.W)
        val RD = UInt(RegIDWidth.W)
        val EX_RET = UInt(DataWidth.W)
        val LS_SRC2 = UInt(DataWidth.W)
    }

    val EX2LS_Msg = ioInternal.PipeLine_EX2LS_MsgBundle.asTypeOf(PipeLine_EX2LS_Bundle)

    //val LoadStoreResult = RegInit(0.U(DataWidth.W))

    //val iLoadStoreLen = RegInit(LS_B) // Default is LS_B since its value is 0
    //val iLoadStoreFunc = RegInit(LS_NOP)
    //val iLoadStoreAddr = RegInit(0.U(AddrWidth.W))
    //val iLoadStoreMemOP = RegInit(MEM_NOP)
    //val iLoadStoreWrite = RegInit(0.U(DataWidth.W))

    val DecodeBundle = EX2LS_Msg.DecodeVal
    val EXU_RET = EX2LS_Msg.EX_RET
    val LSU_SRC = EX2LS_Msg.LS_SRC2

    def WordSignExt(WordVal : UInt) = Cat(Fill(DataWidth - WordWidth, WordVal(WordWidth - 1).asUInt), WordVal(WordWidth - 1, 0))
    def HalfSignExt(HalfVal : UInt) = Cat(Fill(DataWidth - HalfWidth, HalfVal(HalfWidth - 1).asUInt), HalfVal(HalfWidth - 1, 0))
    def ByteSignExt(ByteVal : UInt) = Cat(Fill(DataWidth - ByteWidth, ByteVal(ByteWidth - 1).asUInt), ByteVal(ByteWidth - 1, 0))

    def WordZeroExt(WordVal : UInt) = Cat(Fill(DataWidth - WordWidth, 0.U(1.W)), WordVal (WordWidth - 1, 0))
    def HalfZeroExt(HalfVal : UInt) = Cat(Fill(DataWidth - HalfWidth, 0.U(1.W)), HalfVal (HalfWidth - 1, 0))
    def ByteZeroExt(ByteVal : UInt) = Cat(Fill(DataWidth - ByteWidth, 0.U(1.W)), ByteVal (ByteWidth - 1, 0))

    val LSU_StateOK = (ioInternal.iSlaveValid.asBool && ioInternal.iMasterReady.asBool)

    val LSU_LEN = Mux(LSU_StateOK, DecodeBundle(6, 5), LS_B)
    val LSU_FUNC = Mux(LSU_StateOK, DecodeBundle(4, 3), LS_NOP)
    val LSU_ADDR = Mux(LSU_StateOK, EXU_RET, 0.U(DataWidth.W))
    val LSU_MEMOP = Mux(LSU_StateOK, MuxCase(MEM_NOP, Array(
            (LSU_FUNC === LS_LD) -> MEM_READ,
            (LSU_FUNC === LS_LDU) -> MEM_READ,
            (LSU_FUNC === LS_ST) -> MEM_WRITE
        )), MEM_NOP
    )
    val LSU_WRITE_SRC = Mux(LSU_StateOK, LSU_SRC, 0.U(DataWidth.W))

    // Connect IO External
    ioExternal.oMemoryOP := LSU_MEMOP
    ioExternal.oMemoryAddr := LSU_ADDR
    ioExternal.oMemoryWrite := LSU_WRITE_SRC
    ioExternal.oMemoryLen := LSU_LEN

    val MEM_READ_RET = Mux(LSU_StateOK, ioExternal.iMemoryRead, 0.U(DataWidth.W))

    val LSU_Compute_Result = Mux(LSU_StateOK, MuxCase(0.U(DataWidth.W), Array(
            (LSU_FUNC === LS_LD) -> (MuxCase(0.U(DataWidth.W), Array(
                (LSU_LEN === LS_B) -> ByteSignExt(MEM_READ_RET.asUInt),
                (LSU_LEN === LS_H) -> HalfSignExt(MEM_READ_RET.asUInt),
                (LSU_LEN === LS_W) -> WordSignExt(MEM_READ_RET.asUInt),
                (LSU_LEN === LS_D) -> MEM_READ_RET
            ))),
            (LSU_FUNC === LS_LDU) -> (MuxCase(0.U(DataWidth.W), Array(
                (LSU_LEN === LS_B) -> ByteZeroExt(MEM_READ_RET.asUInt),
                (LSU_LEN === LS_H) -> HalfZeroExt(MEM_READ_RET.asUInt),
                (LSU_LEN === LS_W) -> WordZeroExt(MEM_READ_RET.asUInt)
            )))
        )), 0.U(DataWidth.W))

    val PrePare_PipeLine_LS2WB_Bundle = Mux(LSU_StateOK, Cat(Seq(
        EX2LS_Msg.Instr, EX2LS_Msg.PC, EX2LS_Msg.DecodeVal, EX2LS_Msg.RD, EX2LS_Msg.EX_RET, LSU_Compute_Result
    )), 0.U(PipeLine_LS2WB_Width.W))

    ioInternal.PipeLine_LS2WB_MsgBundle := PrePare_PipeLine_LS2WB_Bundle
    ioInternal.PipeLine_LS2WB_ChangeReg := LSU_StateOK && LSU_NotBusy

    // Connect IO Internal
    //ioInternal.oLSU_RET := LSU_Compute_Result

    // Connect passtorhough for WBU
    //ioInternal.oDecodeBundle := ioInternal.iDecodeBundle
    //ioInternal.oEXU_RET := ioInternal.iEXU_RET
    //ioInternal.oRD := ioInternal.iRD
    //ioInternal.oPC := ioInternal.iPC

    // Pipeline shake hand implementation
    ioInternal.oSlaveReady := (LSU_NotBusy.asBool && ioInternal.iMasterReady.asBool)
    ioInternal.oMasterValid := (LSU_NotBusy.asBool && ioInternal.iSlaveValid.asBool)
}