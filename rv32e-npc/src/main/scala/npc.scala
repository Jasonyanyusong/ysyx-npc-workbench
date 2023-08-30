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

package npc

import chisel3._
import chisel3.util._

import npc.helper.defs.Base._
import npc.helper.defs.CSR_LUT._
import npc.helper.defs.PipeLineDefs._
import npc.helper.opcode.OpPriv._

import npc.units.iFetch._
import npc.units.iDecode._
import npc.units.iExecute._
import npc.units.iLoadStore._
import npc.units.iWriteBack._

class NPCIO extends Bundle{
    val iFetch_iInst = Input(UInt(InstWidth.W))
    val iFetch_oPC = Output(UInt(AddrWidth.W))
    val iFetch_oMemEnable = Output(Bool())

    val iLoadStore_oMemoryOP = Output(UInt(2.W))
    val iLoadStore_oMemoryAddr = Output(UInt(AddrWidth.W))
    val iLoadStore_oMemoryLen = Output(UInt(2.W))
    val iLoadStore_oMemoryWrite = Output(UInt(DataWidth.W))
    val iLoadStore_iMemoryRead = Input(UInt(DataWidth.W))
}

class NPCIODebug extends Bundle{
    val GPR00 = Output(UInt(DataWidth.W))
    val GPR01 = Output(UInt(DataWidth.W))
    val GPR02 = Output(UInt(DataWidth.W))
    val GPR03 = Output(UInt(DataWidth.W))
    val GPR04 = Output(UInt(DataWidth.W))
    val GPR05 = Output(UInt(DataWidth.W))
    val GPR06 = Output(UInt(DataWidth.W))
    val GPR07 = Output(UInt(DataWidth.W))
    val GPR08 = Output(UInt(DataWidth.W))
    val GPR09 = Output(UInt(DataWidth.W))
    val GPR10 = Output(UInt(DataWidth.W))
    val GPR11 = Output(UInt(DataWidth.W))
    val GPR12 = Output(UInt(DataWidth.W))
    val GPR13 = Output(UInt(DataWidth.W))
    val GPR14 = Output(UInt(DataWidth.W))
    val GPR15 = Output(UInt(DataWidth.W))

    val PC_COMMIT = Output(UInt(AddrWidth.W))
    val PC_DYNAMIC = Output(UInt(AddrWidth.W))

    val MSTATUS = Output(UInt(DataWidth.W))
    val MTVEC = Output(UInt(DataWidth.W))
    val MEPC = Output(UInt(DataWidth.W))
    val MCAUSE = Output(UInt(DataWidth.W))

    val Worked = Output(Bool())
    val Halt = Output(Bool())

    val LS_Taken = Output(Bool())
    val LS_Addr = Output(UInt(AddrWidth.W))
}

class NPC extends Module{
    val ioNPC = IO(new NPCIO)
    val ioNPCDebug = IO(new NPCIODebug)

    // Pipeline Components
    val NPC_IFU = Module(new IFU)
    val NPC_IDU = Module(new IDU)
    val NPC_EXU = Module(new EXU)
    val NPC_LSU = Module(new LSU)
    val NPC_WBU = Module(new WBU)

    // PipeLine Registers
    val PipeLine_IF2ID = RegInit(0.U(PipeLine_IF2ID_Width.W))
    val PipeLine_ID2EX = RegInit(0.U(PipeLine_ID2EX_Width.W))
    val PipeLine_EX2LS = RegInit(0.U(PipeLine_EX2LS_Width.W))
    val PipeLine_LS2WB = RegInit(0.U(PipeLine_LS2WB_Width.W))

    // PC Maintain and Manipulation
    val PC = RegInit("h80000000".U(AddrWidth.W))

    PC := Mux(NPC_IDU.ioInternal.oMasterValid, Mux(NPC_IDU.ioInternal.oFeedBackPCChanged.asBool,
        NPC_IDU.ioInternal.oFeedBackNewPCVal,
        PC + 4.U
    ), PC)

    // GPR Maintain and Manipulation
    val GPR = Mem(RegSum, UInt(DataWidth.W))
    val GPR_Status = Mem(RegSum, Bool())

    GPR_Status(NPC_IDU.ioInternal.oRD) := Mux(NPC_IDU.ioInternal.oMasterValid, true.B, GPR_Status(NPC_IDU.ioInternal.oRD))
    GPR_Status(NPC_WBU.ioInternal.oWriteGPRAddr) := Mux(NPC_WBU.ioInternal.oWorked, false.B, GPR_Status(NPC_WBU.ioInternal.oWriteGPRAddr))

    def GPR_Read(index : UInt) = Mux(index === 0.U, 0.U(DataWidth.W), GPR(index))
    def GPR_getStatus(index : UInt) = Mux(index === 0.U, false.B, GPR_Status(index))

    // CSR Maintain and Manipulation
    val mstatus = RegInit(0.U(DataWidth.W))
    val mepc = RegInit(0.U(DataWidth.W))
    val mtvec = RegInit(0.U(DataWidth.W))
    val mcause = RegInit(0.U(DataWidth.W))

    def CSR_Read(index : UInt) = MuxCase(0.U(DataWidth.W), Array(
        (index === CSR_MSTATUS) -> (mstatus.asUInt), // mstatus
        (index === CSR_MTVEC) -> (mtvec.asUInt  ), // mtvec
        (index === CSR_MEPC) -> (mepc.asUInt   ), // mepc
        (index === CSR_MCAUSE) -> (mcause.asUInt )  // mcause
    ))

    // NPC Inside Logic: Top -> IFU
    NPC_IFU.ioInternal.iPC := PC

    // NPC Pipeline Logic: IFU <-> IDU
    PipeLine_IF2ID := Mux(
        NPC_IFU.ioInternal.PipeLine_IF2ID_ChangeReg, 
        NPC_IFU.ioInternal.PipeLine_IF2ID_MsgBundle, 
        PipeLine_IF2ID
    )
    NPC_IFU.ioInternal.iMasterReady := RegNext(NPC_IDU.ioInternal.oSlaveReady)
    NPC_IDU.ioInternal.iSlaveValid  := RegNext(NPC_IFU.ioInternal.oMasterValid)
    NPC_IDU.ioInternal.PipeLine_IF2ID_MsgBundle := PipeLine_IF2ID
    // The PC change feedback need to do at the same time, not using RegNext
    NPC_IFU.ioInternal.iFeedBackPCChanged := (NPC_IDU.ioInternal.oFeedBackPCChanged)

    // NPC Outside Logic: IFU <-> IO
    ioNPC.iFetch_oPC := NPC_IFU.ioExternal.oPC
    ioNPC.iFetch_oMemEnable := NPC_IFU.ioExternal.oMemEnable
    NPC_IFU.ioExternal.iInst := ioNPC.iFetch_iInst

    // NPC Pipeline Logic: IDU <-> EXU
    PipeLine_ID2EX := Mux(
        NPC_IDU.ioInternal.PipeLine_ID2EX_ChangeReg,
        NPC_IDU.ioInternal.PipeLine_ID2EX_MsgBundle,
        PipeLine_ID2EX
    )
    NPC_IDU.ioInternal.iMasterReady := RegNext(NPC_EXU.ioInternal.oSlaveReady)
    NPC_EXU.ioInternal.iSlaveValid  := RegNext(NPC_IDU.ioInternal.oMasterValid)
    NPC_EXU.ioInternal.PipeLine_ID2EX_MsgBundle := PipeLine_ID2EX

    // NPC Inside Logic: IDU <-> Top
    NPC_IDU.ioInternal.iSRC1 := GPR_Read(NPC_IDU.ioInternal.oRS1.asUInt)
    NPC_IDU.ioInternal.iSRC2 := GPR_Read(NPC_IDU.ioInternal.oRS2.asUInt)
    NPC_IDU.ioInternal.iSRC1Dirty := GPR_getStatus(NPC_IDU.ioInternal.oRS1.asUInt)
    NPC_IDU.ioInternal.iSRC2Dirty := GPR_getStatus(NPC_IDU.ioInternal.oRS2.asUInt)
    val CSR_index = NPC_IDU.ioInternal.oCSR_ZicsrWSCIdx.asUInt
    NPC_IDU.ioInternal.iCSR_ZicsrOldVal := CSR_Read(CSR_index)
    val NPC_PipeLine_ID2EX_Bundle = new Bundle{
        val Instr = UInt(InstWidth.W)
        val PC = UInt(AddrWidth.W)
        val DecodeVal = UInt(DecodeWidth.W)
        val RD = UInt(RegIDWidth.W)
        val EXU_SRC1 = UInt(DataWidth.W)
        val EXU_SRC2 = UInt(DataWidth.W)
        val LSU_SRC2 = UInt(DataWidth.W)
    }
    val NPC_ID2EX_Msg = PipeLine_ID2EX.asTypeOf(NPC_PipeLine_ID2EX_Bundle)
    val PrivDecode = NPC_ID2EX_Msg.DecodeVal(15, 14)
    val isZicsr = PrivDecode === PR_ZICSR
    val isECALL = PrivDecode === PR_ECALL
    mstatus := MuxCase(mstatus, Array(
        (isZicsr && CSR_index === CSR_MSTATUS) -> NPC_IDU.ioInternal.oCSR_ZicsrNewVal,
        isECALL -> "h1800".asUInt
    ))
    mtvec := MuxCase(mtvec, Array(
        (isZicsr && CSR_index === CSR_MTVEC) -> NPC_IDU.ioInternal.oCSR_ZicsrNewVal,
    ))
    mepc := MuxCase(mepc, Array(
        (isZicsr && CSR_index === CSR_MEPC) -> NPC_IDU.ioInternal.oCSR_ZicsrNewVal,
        isECALL -> NPC_ID2EX_Msg.PC
    ))
    mcause := MuxCase(mcause, Array(
        (isZicsr && CSR_index === CSR_MCAUSE) -> NPC_IDU.ioInternal.oCSR_ZicsrNewVal,
        isECALL -> 11.U(DataWidth.W)
    ))
    NPC_IDU.ioInternal.iCSR_mtvec := mtvec
    NPC_IDU.ioInternal.iCSR_mepc  := mepc

    // NPC Pipeline Logic: EXU <-> LSU
    PipeLine_EX2LS := Mux(
        NPC_EXU.ioInternal.PipeLine_EX2LS_ChangeReg,
        NPC_EXU.ioInternal.PipeLine_EX2LS_MsgBundle,
        PipeLine_EX2LS
    )
    NPC_EXU.ioInternal.iMasterReady := RegNext(NPC_LSU.ioInternal.oSlaveReady)
    NPC_LSU.ioInternal.iSlaveValid  := RegNext(NPC_EXU.ioInternal.oMasterValid)
    NPC_LSU.ioInternal.PipeLine_EX2LS_MsgBundle := PipeLine_EX2LS

    // NPC Pipeline Logic: LSU <-> WBU
    PipeLine_LS2WB := Mux(
        NPC_LSU.ioInternal.PipeLine_LS2WB_ChangeReg,
        NPC_LSU.ioInternal.PipeLine_LS2WB_MsgBundle,
        PipeLine_LS2WB
    )
    NPC_LSU.ioInternal.iMasterReady := RegNext(NPC_WBU.ioInternal.oSlaveReady)
    NPC_WBU.ioInternal.iSlaveValid  := RegNext(NPC_LSU.ioInternal.oMasterValid)
    NPC_WBU.ioInternal.PipeLine_LS2WB_MsgBundle := PipeLine_LS2WB

    // NPC Outside Logic: LSU <-> IO
    ioNPC.iLoadStore_oMemoryOP     := NPC_LSU.ioExternal.oMemoryOP
    ioNPC.iLoadStore_oMemoryAddr   := NPC_LSU.ioExternal.oMemoryAddr
    ioNPC.iLoadStore_oMemoryLen    := NPC_LSU.ioExternal.oMemoryLen
    ioNPC.iLoadStore_oMemoryWrite  := NPC_LSU.ioExternal.oMemoryWrite
    NPC_LSU.ioExternal.iMemoryRead := ioNPC.iLoadStore_iMemoryRead

    // NPC Inside Logic: WBU <-> Top
    GPR(NPC_WBU.ioInternal.oWriteGPRAddr.asUInt) := 
        Mux(
            NPC_WBU.ioInternal.oWriteGPREnable.asBool, 
            NPC_WBU.ioInternal.oWriteGPRVal, 
            GPR(NPC_WBU.ioInternal.oWriteGPRAddr.asUInt
        )
    )

    // Connect Debug Logic, Debug Transfer will happen at Write-Back Phase, CSR will use shift register to debug
    ioNPCDebug.GPR00 := GPR_Read(00.U)
    ioNPCDebug.GPR01 := GPR_Read(01.U)
    ioNPCDebug.GPR02 := GPR_Read(02.U)
    ioNPCDebug.GPR03 := GPR_Read(03.U)
    ioNPCDebug.GPR04 := GPR_Read(04.U)
    ioNPCDebug.GPR05 := GPR_Read(05.U)
    ioNPCDebug.GPR06 := GPR_Read(06.U)
    ioNPCDebug.GPR07 := GPR_Read(07.U)
    ioNPCDebug.GPR08 := GPR_Read(08.U)
    ioNPCDebug.GPR09 := GPR_Read(09.U)
    ioNPCDebug.GPR10 := GPR_Read(10.U)
    ioNPCDebug.GPR11 := GPR_Read(11.U)
    ioNPCDebug.GPR12 := GPR_Read(12.U)
    ioNPCDebug.GPR13 := GPR_Read(13.U)
    ioNPCDebug.GPR14 := GPR_Read(14.U)
    ioNPCDebug.GPR15 := GPR_Read(15.U)

    ioNPCDebug.PC_COMMIT := NPC_WBU.ioInternal.oPC
    ioNPCDebug.PC_DYNAMIC := ShiftRegister(PC, 4)

    ioNPCDebug.Worked := RegNext(RegNext(NPC_WBU.ioInternal.oWorked))
    ioNPCDebug.Halt := NPC_WBU.ioInternal.oStopped

    // CSR: since it was written in IDU, need to shift for EXU -> LSU -> WBU, 3 cycles
    ioNPCDebug.MSTATUS := ShiftRegister(mstatus, 3)
    ioNPCDebug.MTVEC := ShiftRegister(mtvec, 3)
    ioNPCDebug.MEPC := ShiftRegister(mepc, 3)
    ioNPCDebug.MCAUSE := ShiftRegister(mcause, 3)

    // Memory: to help simulation environemnt judge the addr is in pmem
    ioNPCDebug.LS_Taken := ShiftRegister(NPC_LSU.ioDebug.oLoadStoreTaken, 1)
    ioNPCDebug.LS_Addr := ShiftRegister(NPC_LSU.ioDebug.oLoadStoreAddress, 1)
}