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
import npc.helper.axi._

class iFetchInternal extends Bundle{
    // ON-PIPELINE VALUES
    val iMasterReady = Input(Bool())
    val oMasterValid = Output(Bool())

    val iPC = Input(UInt(AddrWidth.W))

    val iFeedBackPCChanged = Input(Bool())
    val iFeedBackDecodingJumpInstr = Input(Bool())
    val iIDUDecodingBranch = Input(Bool())
    val iIDUDecodingJump = Input(Bool())

    val PipeLine_IF2ID_MsgBundle = Output(UInt(PipeLine_IF2ID_Width.W))
    val PipeLine_IF2ID_ChangeReg = Output(Bool())
}

object IFU_AXI_State {
    val AXI_Free = 0.U(2.W)
    val AXI_Sending = 1.U(2.W)
    val AXI_Reading = 2.U(2.W)
}

object IFU_AXI_Defs {
    val AXI_AR_ID = 1.U(4.W) // hard wired ID
    val AXI_AR_LEN = 0.U(8.W) // only 1 transfer per transaction
    val AXI_AR_SIZE = 2.U(3.W) // hard wired, inst size is 32 bit
    val AXI_AR_BURST = 0.U(2.W) // fixed burst, 1 transfer per transaction
}

class IFU extends Module{
    val ioInternal = IO(new iFetchInternal)

    val IFU_AXI_AR = IO(new AXIMasterAR)
    val IFU_AXI_R  = IO(new AXIMasterR)

    val AXI_State_IFU = RegInit(IFU_AXI_State.AXI_Free)

    val IFU_StateOK = ioInternal.iMasterReady.asBool

    // axi hard wired settings
    IFU_AXI_AR.oMasterARid := IFU_AXI_Defs.AXI_AR_ID
    IFU_AXI_AR.oMasterARlen := IFU_AXI_Defs.AXI_AR_LEN
    IFU_AXI_AR.oMasterARsize := IFU_AXI_Defs.AXI_AR_SIZE
    IFU_AXI_AR.oMasterARburst := IFU_AXI_Defs.AXI_AR_BURST

    // if we can fetch a instr
    val CanFetch = (IFU_StateOK) && (!ioInternal.iIDUDecodingBranch) && (!ioInternal.iFeedBackPCChanged) && (!ioInternal.iFeedBackDecodingJumpInstr) && (!ioInternal.iIDUDecodingJump)
    AXI_State_IFU := Mux(AXI_State_IFU === IFU_AXI_State.AXI_Free, 
        Mux(CanFetch, IFU_AXI_State.AXI_Sending, AXI_State_IFU), AXI_State_IFU)

    // update AXI status
    AXI_State_IFU := Mux(AXI_State_IFU === IFU_AXI_State.AXI_Sending,
        Mux(IFU_AXI_AR.iMasterARready, IFU_AXI_State.AXI_Reading, AXI_State_IFU), AXI_State_IFU)

    // axi hand-shaking signals
    IFU_AXI_AR.oMasterARvalid := AXI_State_IFU === IFU_AXI_State.AXI_Sending
    IFU_AXI_R.oMasterRready := ioInternal.iMasterReady // IDU is ready for decoding

    // axi address information
    IFU_AXI_AR.oMasterARaddr := ioInternal.iPC // the memory address is current PC

    // update AXI status
    AXI_State_IFU := Mux(AXI_State_IFU === IFU_AXI_State.AXI_Reading,
        Mux(IFU_AXI_R.iMasterRvalid, IFU_AXI_State.AXI_Free, AXI_State_IFU), AXI_State_IFU)

    // prepare for read response
    val InstResp = IFU_AXI_R.iMasterRdata(InstWidth, 0)

    ioInternal.oMasterValid := IFU_AXI_R.iMasterRvalid && ((!ioInternal.iFeedBackPCChanged) && (!ioInternal.iIDUDecodingBranch) && ioInternal.iPC =/= 0.U) && (!ioInternal.iFeedBackDecodingJumpInstr) && (!ioInternal.iIDUDecodingJump)// && ioInternal.iPCHaveWB
    val PC = ioInternal.iPC

    val PrePare_PipeLine_IF2ID_Bundle = Cat(Seq(InstResp, PC))
    ioInternal.PipeLine_IF2ID_MsgBundle := PrePare_PipeLine_IF2ID_Bundle
    ioInternal.PipeLine_IF2ID_ChangeReg := ((IFU_StateOK))
}