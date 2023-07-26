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

object iFetchInternal extends Bundle{
    val iReady = Input(Bool())
    val oValid = Output(Bool())
    val iPC = Input(UInt(AddrWidth.W))
    val oInstData = Output(UInt(InstWidth.W))
}

/*object iFetchExternal extends Bundle{
    val iPC = Input(UInt(AddrWidth.W))
    val iInst = Input(UInt(InstWidth.W))
    val oPC = Input(UInt(AddrWidth.W))
    val oInst = Input(UInt(InstWidth.W))
}*/

class IFU extends Module{
    val ioInternal = IO(new iFetchInternal)
    //val ioExternal = IO(new iFetchExternal)

    val ioExternalAR = IO(new AXIMasterAR)
    val ioExternalR  = IO(new AXIMasterR)
    //def iFetch(addr: UInt(AddrWidth.W))

    val PC := ioInternal.iPC

    // Only fetch instruction when slave (IDU) is ready
    if(io.iReady.asBool){
        // AXI Interface get inst
    }else{
        // Hold signals, do nor perform iFetch
    }
}