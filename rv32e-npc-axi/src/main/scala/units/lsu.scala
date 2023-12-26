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
import npc.helper.defs.PipeLine_Bundle._

import npc.helper.axi._

import npc.helper.opcode.OpLSULen._
import npc.helper.opcode.OpLSUFunc._
import npc.helper.opcode.MemOp._

class iLoadStoreInternal extends Bundle{
    // ON-PIPELINE VALUES
    val oSlaveReady = Output(Bool())
    val iSlaveValid = Input(Bool())
    
    val iMasterReady = Input(Bool())
    val oMasterValid = Output(Bool())

    val PipeLine_EX2LS_MsgBundle = Input(UInt(PipeLine_EX2LS_Width.W))
    val PipeLine_LS2WB_MsgBundle = Output(UInt(PipeLine_LS2WB_Width.W))
    val PipeLine_LS2WB_ChangeReg = Output(Bool())
}

object LSU_AXI_State {
    val AXI_Free = 0.U(2.W)
    val AXI_Sending = 1.U(2.W)
    val AXI_Reading = 2.U(2.W)
    val AXI_Writing = 3.U(2.W)
}

object LSU_AXI_R_Defs {
    val AXI_AR_ID = 2.U(4.W) // hard wired ID
    val AXI_AR_LEN = 0.U(8.W) // only 1 transfer per transaction
    val AXI_AR_BURST = 0.U(2.W) // fixed burst, 1 transfer per transaction
}

object LSU_AXI_W_Defs {
    val AXI_AW_ID = 2.U(4.W) // hard wired ID
    val AXI_AW_LEN = 0.U(8.W)
    val AXI_AW_BURST = 0.U(2.W)
}

class iLoadStoreExternal extends Bundle{
    val oMemoryOP = Output(UInt(2.W))
    val oMemoryLen = Output(UInt(2.W))
    val oMemoryAddr = Output(UInt(AddrWidth.W))
    val oMemoryWrite = Output(UInt(DataWidth.W))
    val iMemoryRead = Input(UInt(DataWidth.W))
}

class iLoadStoreDebug extends Bundle{
    val oLoadStoreTaken = Output(Bool())
    val oLoadStoreAddress = Output(UInt(AddrWidth.W))
}

class LSU extends Module{
    val ioInternal = IO(new iLoadStoreInternal)
    val ioExternal = IO(new iLoadStoreExternal)

    // AXI
    val LSU_AXI_AW = IO(new AXIMasterAW)
    val LSU_AXI_W  = IO(new AXIMasterW)
    val LSU_AXI_B  = IO(new AXIMasterB)
    val LSU_AXI_AR = IO(new AXIMasterAR)
    val LSU_AXI_R  = IO(new AXIMasterR)

    val ioDebug    = IO(new iLoadStoreDebug)

    val LSU_NotBusy = RegInit(true.B)

    // record the status of AXI
    val AXI_State_LSU = RegInit(LSU_AXI_State.AXI_Free)

    val EX2LS_Msg = ioInternal.PipeLine_EX2LS_MsgBundle.asTypeOf(PipeLine_EX2LS_Bundle)

    val DecodeBundle = EX2LS_Msg.DecodeVal
    val EXU_RET = EX2LS_Msg.EX_RET
    val LSU_SRC = EX2LS_Msg.LS_SRC2

    def HalfSignExt(HalfVal : UInt) = Cat(Fill(DataWidth - HalfWidth, HalfVal(HalfWidth - 1).asUInt), HalfVal(HalfWidth - 1, 0))
    def ByteSignExt(ByteVal : UInt) = Cat(Fill(DataWidth - ByteWidth, ByteVal(ByteWidth - 1).asUInt), ByteVal(ByteWidth - 1, 0))

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
    ioExternal.oMemoryOP := MuxCase(LS_NOP, Array(
        (LSU_ADDR >= "h80000000".U && LSU_ADDR <= "h8fffffff".U) -> LSU_MEMOP,
        (LSU_ADDR >= "ha0000048".U && LSU_ADDR <= "ha000004f".U) -> LSU_MEMOP,
        (LSU_ADDR >= "ha00003f8".U && LSU_ADDR <= "ha00003ff".U) -> LSU_MEMOP,
        (LSU_ADDR >= "ha0000060".U && LSU_ADDR <= "ha0000063".U) -> LSU_MEMOP,
    ))
    ioExternal.oMemoryAddr := LSU_ADDR
    ioExternal.oMemoryWrite := LSU_WRITE_SRC
    ioExternal.oMemoryLen := LSU_LEN

    val MEM_READ_RET = Mux(LSU_StateOK, ioExternal.iMemoryRead, 0.U(DataWidth.W))

    val LSU_Compute_Result = Mux(LSU_StateOK, MuxCase(0.U(DataWidth.W), Array(
            (LSU_FUNC === LS_LD) -> (MuxCase(0.U(DataWidth.W), Array(
                (LSU_LEN === LS_B) -> ByteSignExt(MEM_READ_RET.asUInt),
                (LSU_LEN === LS_H) -> HalfSignExt(MEM_READ_RET.asUInt),
                (LSU_LEN === LS_W) -> MEM_READ_RET.asUInt
            ))),
            (LSU_FUNC === LS_LDU) -> (MuxCase(0.U(DataWidth.W), Array(
                (LSU_LEN === LS_B) -> ByteZeroExt(MEM_READ_RET.asUInt),
                (LSU_LEN === LS_H) -> HalfZeroExt(MEM_READ_RET.asUInt),
            )))
        )), 0.U(DataWidth.W))

    val PrePare_PipeLine_LS2WB_Bundle = Mux(LSU_StateOK, Cat(Seq(
        EX2LS_Msg.Instr, EX2LS_Msg.PC, EX2LS_Msg.DNPC, EX2LS_Msg.DecodeVal, EX2LS_Msg.RD, EX2LS_Msg.EX_RET, LSU_Compute_Result
    )), 0.U(PipeLine_LS2WB_Width.W))

    ioDebug.oLoadStoreTaken := Mux(LSU_StateOK, Mux((LSU_MEMOP === MEM_READ) || (LSU_MEMOP === MEM_WRITE), true.B, false.B), false.B)
    ioDebug.oLoadStoreAddress := LSU_ADDR

    ioInternal.PipeLine_LS2WB_MsgBundle := PrePare_PipeLine_LS2WB_Bundle
    ioInternal.PipeLine_LS2WB_ChangeReg := LSU_StateOK && LSU_NotBusy

    // Pipeline shake hand implementation
    ioInternal.oSlaveReady := (LSU_NotBusy.asBool && ioInternal.iMasterReady.asBool)
    ioInternal.oMasterValid := (LSU_NotBusy.asBool && ioInternal.iSlaveValid.asBool)
}