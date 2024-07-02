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
    val iIDUDecodingJump = Input(Bool())
    //val iPCHaveWB = Input(Bool())

    val PipeLine_IF2ID_MsgBundle = Output(UInt(PipeLine_IF2ID_Width.W))
    val PipeLine_IF2ID_ChangeReg = Output(Bool())
}

class iFetchExternal extends Bundle{
    val iInst = Input(UInt(InstWidth.W))
    val oPC = Output(UInt(AddrWidth.W))
    val oMemEnable = Output(Bool())

    // the following is used for bus layer supoort
    val iMemValid = Input(Bool())
}

class IFU extends Module{
    val ioInternal = IO(new iFetchInternal)
    val ioExternal = IO(new iFetchExternal)

    val IFU_StateOK = ioInternal.iMasterReady.asBool

    ioExternal.oMemEnable := (IFU_StateOK) && (!ioInternal.iIDUDecodingBranch) && (!ioInternal.iFeedBackPCChanged) && (!ioInternal.iFeedBackDecodingJumpInstr) && (!ioInternal.iIDUDecodingJump)// && ioInternal.iPCHaveWB
    ioExternal.oPC := ioInternal.iPC
    ioInternal.oMasterValid := ioExternal.iMemValid && ((!ioInternal.iFeedBackPCChanged) && (!ioInternal.iIDUDecodingBranch) && ioInternal.iPC =/= 0.U) && (!ioInternal.iFeedBackDecodingJumpInstr) && (!ioInternal.iIDUDecodingJump)// && ioInternal.iPCHaveWB

    val Inst = Mux(IFU_StateOK && ioExternal.iMemValid, ioExternal.iInst, 0.U(InstWidth.W))
    val PC = ioInternal.iPC

    val PrePare_PipeLine_IF2ID_Bundle = Cat(Seq(Inst, PC))
    ioInternal.PipeLine_IF2ID_MsgBundle := PrePare_PipeLine_IF2ID_Bundle
    ioInternal.PipeLine_IF2ID_ChangeReg := IFU_StateOK
}