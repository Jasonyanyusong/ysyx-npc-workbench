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

import npc.axi.master._

object iFetchInternal extends Bundle{
    val iMasterReady = Input(Bool())
    val oMasterValid = Output(Bool())
    val iPC = Input(UInt(AddrWidth.W))
    val oInst = Output(UInt(InstWidth.W))
    //val iResetInstBuffer = Input(Bool())
}

object iFetchExternal extends Bundle{
    //val iPC = Input(UInt(AddrWidth.W))
    val iInst = Input(UInt(InstWidth.W))
    val oPC = Output(UInt(AddrWidth.W))
    val oMemEnable = Output(Bool())
    //val oInst = Input(UInt(InstWidth.W))
}

class IFU extends Module{
    val ioInternal = IO(new iFetchInternal)
    val ioExternal = IO(new iFetchExternal)

    // Non-AXI, pipelined Version IFU

    val Inst = RegInit(0.U(InstWidth.W))

    val iFetchEnable = RegInit(true.B)

    /*if(iFetchEnable.asBool){
        ioExternal.oMemEnable := true.B
        ioExternal.oPC := ioInternal.iPC
        Inst := ioExternal.iInst
        iFetchEnable := false.B
    }else{
        ioExternal.oMemEnable := false.B
        ioExternal.oPC := 0.U(AddrWidth.W)
    }*/

    Mux(iFetchEnable.asBool, ioExternal.oMemEnable := true.B,  ioExternal.oMemEnable := false.B)
    Mux(iFetchEnable.asBool, ioExternal.oPC := ioInternal.iPC, ioExternal.oPC := 0.U(AddrWidth.W))
    Mux(iFetchEnable.asBool, Inst := ioExternal.iInst,         Inst := Inst.asUInt)
    Mux(iFetchEnable.asBool, iFetchEnable := false.B,          iFetchEnable := true.B)

    ioInternal.oInst := Inst
    ioInternal.oMasterValid := true.B

    when(ioInternal.iMasterReady.asBool){
        // Shake hand success, re-enable iFetch, fetch next PC's instruction
        iFetchEnable := true.B
    }
    
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