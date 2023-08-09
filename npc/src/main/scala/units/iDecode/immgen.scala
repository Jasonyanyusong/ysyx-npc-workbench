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

package npc.units.iDecode

import chisel3._
import chisel3.util._

import npc.helper.defs.Base._
import npc.helper.defs.InstType._

class immGenInternal extends Bundle{
    val iInst = Input(UInt(AddrWidth.W))
    val iType = Input(UInt(InstTypeWidth.W))
    val oImm = Output(UInt(DataWidth.W))
}

class immGen extends Module{
    val ioSubmodule = IO(new immGenInternal)

    val immI = ioSubmodule.iInst(31, 20).asUInt
    val immS = Cat(
        Seq(
            ioSubmodule.iInst(31, 25).asUInt,
            ioSubmodule.iInst(11,  7).asUInt
        )
    )
    val immB = Cat(
        Seq(
            ioSubmodule.iInst(31, 31).asUInt,
            ioSubmodule.iInst( 7,  7).asUInt,
            ioSubmodule.iInst(30, 25).asUInt,
            ioSubmodule.iInst(11,  8).asUInt,
            Fill(1, 0.U(1.W)).asUInt
        )
    )
    val immU = Cat(
        Seq(
            ioSubmodule.iInst(31, 12).asUInt,
            Fill(12, 0.U(1.W)).asUInt
        )
    )
    val immJ = Cat(
        Seq(
            ioSubmodule.iInst(31, 31).asUInt,
            ioSubmodule.iInst(19, 12).asUInt,
            ioSubmodule.iInst(20, 20).asUInt,
            ioSubmodule.iInst(30, 21).asUInt,
            Fill(1, 0.U(1.W)).asUInt
        )
    )

    val InstType = ioSubmodule.iType

    ioSubmodule.oImm := MuxCase(
        0.U(DataWidth.W),
        Array(
            (InstType === instI) -> Cat(Fill(DataWidth - immILen, immI(immILen - 1)), immI),
            (InstType === instS) -> Cat(Fill(DataWidth - immSLen, immS(immSLen - 1)), immS),
            (InstType === instB) -> Cat(Fill(DataWidth - immBLen, immB(immBLen - 1)), immB),
            (InstType === instU) -> Cat(Fill(DataWidth - immULen, immU(immULen - 1)), immU),
            (InstType === instJ) -> Cat(Fill(DataWidth - immJLen, immJ(immJLen - 1)), immJ),
        )
    )
}