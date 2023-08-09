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
import npc.helper.defs.PR_ZICSR._

object NPCIO extends Bundle{
    val iFetch_iInst = Input(UInt(InstWidth.W))
    val iFetch_oPC = Output(UInt(AddrWidth.W))
    val iFetch_oMemEnable = Output(Bool())

    val iLoadStore_oMemoryOP = Output(UInt(2.W))
    val iLoadStore_oMemoryAddr = Output(UInt(AddrWidth.W))
    val iLoadStore_oMemoryWrite = Output(UInt(DataWidth.W))
    val iLoadStore_iMemoryRead = Input(UInt(DataWidth.W))
}

object NPCIODebug extends Bundle{
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
    val GPR16 = Output(UInt(DataWidth.W))
    val GPR17 = Output(UInt(DataWidth.W))
    val GPR18 = Output(UInt(DataWidth.W))
    val GPR19 = Output(UInt(DataWidth.W))
    val GPR20 = Output(UInt(DataWidth.W))
    val GPR21 = Output(UInt(DataWidth.W))
    val GPR22 = Output(UInt(DataWidth.W))
    val GPR23 = Output(UInt(DataWidth.W))
    val GPR24 = Output(UInt(DataWidth.W))
    val GPR25 = Output(UInt(DataWidth.W))
    val GPR26 = Output(UInt(DataWidth.W))
    val GPR27 = Output(UInt(DataWidth.W))
    val GPR28 = Output(UInt(DataWidth.W))
    val GPR29 = Output(UInt(DataWidth.W))
    val GPR30 = Output(UInt(DataWidth.W))
    val GPR31 = Output(UInt(DataWidth.W))

    val PC    = Output(UInt(AddrWidth.W))

    val MSTATUS = Output(UInt(DataWidth.W))
    val MTVEC = Output(UInt(DataWidth.W))
    val MEPC = Output(UInt(DataWidth.W))
    val MCAUSE = Output(UInt(DataWidth.W))
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

    // GPR Maintain and Manipulation
    val GPR = Mem(RegSum, 0.U(DataWidth.W))
    def GPR_Read(index : UInt) = Mux(index === 0.U, 0.U(DataWidth.W), GPR(index))

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

    val PrivDecode = NPC_IDU.oDecodeBundle(15, 14)

    val isZicsr = PrivDecode === PR_ZICSR
    val isECALL = PrivDecode === PR_ECALL

    def CSR_Write(CSR_idx, CSR_val : UInt) = {
        switch (CSR_idx.asUInt){
            is(CSR_MSTATUS){
                Mux(isZicsr.asBool, mstatus := CSR_val, mstatus := mstatus)
            }
            is(CSR_MTVEC){
                Mux(isZicsr.asBool, mtvec := CSR_val, mtvec := mtvec)
            }
            is(CSR_MEPC){
                Mux(isZicsr.asBool, mepc := CSR_val, mepc := mepc)
            }
            is(CSR_MCAUSE){
                Mux(isZicsr.asBool, mcause := CSR_val, mcause := mcause)
            }
        }
        Mux(isECALL, mcause := "b11".asUInt, mcause := mcause)
    }

    // NPC Inside Logic: IFU <-> IDU
    NPC_IFU.ioInternal.iMasterReady := RegNext(NPC_IDU.ioInternal.oSlaveReady)
    NPC_IDU.ioInternal.iSlaveValid  := RegNext(NPC_IDU.ioInternal.oMasterValid)
    
    NPC_IFU.ioInternal.iFeedBackPCChanged := RegNext(NPC_IDU.ioInternal.oFeedBackPCChanged)
    NPC_IFU.ioInternal.iFeedBackNewPCVal  := RegNext(NPC_IDU.ioInternal.oFeedBackNewPCVal)

    NPC_IDU.ioInternal.iInst := RegNext(NPC_IFU.ioInternal.oInst)
    NPC_IDU.iPC              := RegNext(NPC_IFU.ioInternal.oPC)

    // NPC Outside Logic: IFU <-> IO
    NPCIO.iFetch_oPC := NPC_IFU.ioExternal.oPC
    NPCIO.iFetch_oMemEnable := NPC_IFU.ioExternal.oMemEnable
    NPC_IFU.ioExternal.iInst := NPCIO.iInst

    // NPC Inside Logic: IDU <-> EXU
    NPC_IDU.ioInternal.iMasterReady := RegNext(NPC_EXU.ioInternal.oSlaveReady)
    NPC_EXU.ioInternal.iSlaveValid  := RegNext(NPC_IDU.ioInternal.oMasterValid)

    NPC_EXU.ioInternal.iDecodeBundle := RegNext(NPC_IDU.ioInternal.oDecodeBundle)
    NPC_EXU.ioInternal.iEXU_SRC1     := RegNext(NPC_IDU.ioInternal.oEXU_src1)
    NPC_EXU.ioInternal.iEXU_SRC2     := RegNext(NPC_IDU.ioInternal.oEXU_src2)
    NPC_EXU.ioInternal.iLSU_SRC2     := RegNext(NPC_IDU.ioInternal.oLSU_SRC2)

    NPC_EXU.ioInternal.iRD := RegNext(NPC_IDU.ioInternal.oRD)
    NPC_EXU.ioInternal.iPC := RegNext(NPC_IDU.ioInternal.oPC)

    // NPC Inside Logic: IDU <-> Top

    // NPC Inside Logic: EXU <-> LSU
    NPC_EXU.ioInternal.iMasterReady := RegNext(NPC_LSU.ioInternal.oSlaveReady)
    NPC_LSU.ioInternal.iSlaveValid  := RegNext(NPC_EXU.ioInternal.oMasterValid)

    NPC_LSU.ioInternal.iDecodeBundle := RegNext(NPC_EXU.ioInternal.oDecodeBundle)
    NPC_LSU.ioInternal.iEXU_RET      := RegNext(NPC_EXU.ioInternal.oEXU_RET)
    NPC_LSU.ioInternal.iLSU_SRC2     := RegNext(NPC_EXU.ioInternal.oLSU_SRC2)

    NPC_LSU.ioInternal.iRD := RegNext(NPC_EXU.ioInternal.oRD)
    NPC_LSU.ioInternal.iPC := RegNext(NPC_EXU.ioInternal.oPC)

    // NPC Inside Logic: LSU <-> WBU
    NPC_LSU.ioInternal.iMasterReady := RegNext(NPC_WBU.ioInternal.oSlaveReady)
    NPC_WBU.ioInternal.iSlaveValid  := RegNext(NPC_LSU.ioInternal.oMasterValid)
    
    NPC_WBU.ioInternal.iDecodeBundle := RegNext(NPC_LSU.ioInternal.oDecodeBundle)
    NPC_WBU.ioInternal.iEXU_RET      := RegNext(NPC_LSU.ioInternal.oEXU_RET)
    NPC_WBU.ioInternal.iLSU_RET      := RegNext(NPC_LSU.ioInternal.oLSU_RET)

    NPC_WBU.ioInternal.iRD := RegNext(NPC_LSU.ioInternal.oRD)
    NPC_WBU.ioInternal.iPC := RegNext(NPC_LSU.ioInternal.oPC)

    // NPC Outside Logic: LSU <-> IO
    NPCIO.iLoadStore_oMemoryOP     := NPC_LSU.ioExternal.oMemoryOP
    NPCIO.iLoadStore_oMemoryAddr   := NPC_LSU.ioExternal.oMemoryAddr
    NPCIO.iLoadStore_oMemoryWrite  := NPC_LSU.ioExternal.oMemoryWrite
    NPC_LSU.ioExternal.iMemoryRead := NPCIO.iLoadStore_iMemoryRead

    // NPC Inside Logic: WBU <-> Top


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
    ioNPCDebug.GPR16 := GPR_Read(16.U)
    ioNPCDebug.GPR17 := GPR_Read(17.U)
    ioNPCDebug.GPR18 := GPR_Read(18.U)
    ioNPCDebug.GPR19 := GPR_Read(19.U)
    ioNPCDebug.GPR20 := GPR_Read(20.U)
    ioNPCDebug.GPR21 := GPR_Read(21.U)
    ioNPCDebug.GPR22 := GPR_Read(22.U)
    ioNPCDebug.GPR23 := GPR_Read(23.U)
    ioNPCDebug.GPR24 := GPR_Read(24.U)
    ioNPCDebug.GPR25 := GPR_Read(25.U)
    ioNPCDebug.GPR26 := GPR_Read(26.U)
    ioNPCDebug.GPR27 := GPR_Read(27.U)
    ioNPCDebug.GPR28 := GPR_Read(28.U)
    ioNPCDebug.GPR29 := GPR_Read(29.U)
    ioNPCDebug.GPR30 := GPR_Read(30.U)
    ioNPCDebug.GPR31 := GPR_Read(31.U)

    ioNPCDebug.PC := NPC_WBU.ioInternal.oPC

    // CSR: since it was written in IDU, need to shift for EXU -> LSU -> WBU, 3 cycles
    ioNPCDebug.MSTATUS := ShiftRegister(mstatus, 3)
    ioNPCDebug.MTVEC := ShiftRegister(mtvec, 3)
    ioNPCDebug.MEPC := ShiftRegister(mepc, 3)
    ioNPCDebug.MCAUSE := ShiftRegister(mcause, 3)
}