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

package npc.units.iFetch

import chisel3._
import chisel3.util._

import npc.helper.defs.Base._
import npc.helper.defs.PipeLineDefs._

import npc.axi.master._

class iFetchInternal extends Bundle{
    // ON-PIPELINE VALUES
    val iMasterReady = Input(Bool())
    val oMasterValid = Output(Bool())

    val iPC = Input(UInt(AddrWidth.W))

    val iFeedBackPCChanged = Input(Bool())
    val iFeedBackDecodingJumpInstr = Input(Bool())
    val iIDUDecodingBranch = Input(Bool())
    //val iPCHaveWB = Input(Bool())

    val PipeLine_IF2ID_MsgBundle = Output(UInt(PipeLine_IF2ID_Width.W))
    val PipeLine_IF2ID_ChangeReg = Output(Bool())
}

class iFetchExternal extends Bundle{
    val iInst = Input(UInt(InstWidth.W))
    val oPC = Output(UInt(AddrWidth.W))
    val oMemEnable = Output(Bool())
}

class IFU extends Module{
    val ioInternal = IO(new iFetchInternal)
    val ioExternal = IO(new iFetchExternal)

    // Non-AXI, pipelined Version IFU

    val IFU_StateOK = ioInternal.iMasterReady.asBool

    ioExternal.oMemEnable := (IFU_StateOK) && (!ioInternal.iIDUDecodingBranch) && (!ioInternal.iFeedBackPCChanged) && (!ioInternal.iFeedBackDecodingJumpInstr)// && ioInternal.iPCHaveWB
    ioExternal.oPC := ioInternal.iPC
    ioInternal.oMasterValid := ((!ioInternal.iFeedBackPCChanged) && (!ioInternal.iIDUDecodingBranch) && ioInternal.iPC =/= 0.U) && (!ioInternal.iFeedBackDecodingJumpInstr)// && ioInternal.iPCHaveWB

    val Inst = Mux(IFU_StateOK, ioExternal.iInst, 0.U(InstWidth.W))
    val PC = ioInternal.iPC

    val PrePare_PipeLine_IF2ID_Bundle = Cat(Seq(Inst, PC))
    ioInternal.PipeLine_IF2ID_MsgBundle := PrePare_PipeLine_IF2ID_Bundle
    ioInternal.PipeLine_IF2ID_ChangeReg := ((IFU_StateOK))
    
    /*

    // AXI Version IFU

    val ioExternalAR = IO(new AXIMasterAR)
    val ioExternalR  = IO(new AXIMasterR)
    //def iFetch(addr: UInt(AddrWidth.W))

    val PC := ioInternal.iPC

    //val HoldVal = RegInit(false.B)
    //val Instruction = RegInit(0.U(InstWidth.W))

    val BufferSize = 8.U // Hold 8 Instructions in Buffer
    val InstBuffer = Mem(BufferSize.asUInt, UInt(InstWidth.W))
    val InstBufferStart = RegInit(0.U(InstWidth.W).asUInt)
    val InstBufferEnd = RegInit(0.U(InstWidth.W).asUInt)
    val InstBufferIdx = RegInit(0.U(3.W).asUInt) // Idx from 0 - 7, need to change with BufferSize
    val InstBufferOK = RegInit(false.B)

    // (InstBufferOK && (!iResetInstBuffer)) --YES--> No operation now
    //                                       --N O--> clear all buffer & record -> buffer fetch -> update Buffer Idx & OK

    // Get PC -> Check Inst in buffer --YES--> assign output + (clear buffer -> optional) & record
    //                                --N O--> clear all buffer & record -> buffer fetch -> assign output + clear buffer & record

    if((!InstBufferOK.asBool) || ioInternal.iResetInstBuffer){
        // Current IFU does not hold instruction or current instructions are invalid, fetch new
        ioExternalAR.oMasterARaddr := PC // Assigne current PC to AXI AR address channel
        ioExternalAR.oMasterARid := 0.U(4.W) // Use default ID now
        ioExternalAR.oMasterARlen := 0.U(8.W) // Just fetch 1 inst using AXI
        ioExternalAR.oMasterARsize := 2.U(3.W) // Size of one transfer event is 0b010
        ioExternalAR.oMasterARburst := 1.U(2.W) // Incrementing brust access
        ioExternalAR.oMasterARvalid := true.B // These data are ready

        ioExternalR.oMasterRready := true.B
    }

    when(ioExternalAR.iMasterARready.asBool && ioExternalR.iMasterRvalid.asBool){

    }

    // Only fetch instruction when slave (IDU) is ready
    if(io.iMasterReady.asBool){
        // AXI Interface get inst
    }else{
        // Hold signals, do nor perform iFetch
    }

    */
}