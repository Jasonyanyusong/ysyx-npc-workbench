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

package npc.helper.defs

import chisel3._
import chisel3.util._

object Base{
    //val NR_GPR = 16
    
    val DataWidth = 32
    val WordWidth = 32
    val HalfWidth = 16
    val ByteWidth = 8

    val InstWidth = 32
    val InstSize = 4
    
    val AddrWidth = 32

    val RegIDWidth = 5
    val RegSum = 16
    val CSRIDWidth = 12

    val iDecPrivValLen = 2
    val iDecEXUValLen = 7
    val iDecLSlenValLen = 2
    val iDecLSfuncValLen = 2
    val iDecWBTypValLen = 2
    val iDecDSValLen = 1
    val DecodeWidth = iDecPrivValLen + iDecEXUValLen + iDecLSlenValLen + iDecLSfuncValLen + iDecWBTypValLen + iDecDSValLen

    val InstTypeWidth = 4

    val immILen = 12
    val immSLen = 12
    val immBLen = 13
    val immULen = 32
    val immJLen = 21

    val RS1Hi = 19
    val RS1Lo = 15

    val RS2Hi = 24
    val RS2Lo = 20

    val RDHi = 11
    val RDLo = 7
}

object PipeLineDefs{
    val PipeLine_IF2ID_Width = Base.InstWidth + Base.AddrWidth
    val PipeLine_ID2EX_Width = Base.InstWidth + Base.AddrWidth + Base.DecodeWidth + Base.RegIDWidth + (3 * Base.DataWidth) // EX have 2 SRC, LS have 1 SRC
    val PipeLine_EX2LS_Width = Base.InstWidth + Base.AddrWidth + Base.DecodeWidth + Base.RegIDWidth + (2 * Base.DataWidth) // EX_RET and LS_SRC
    val PipeLine_LS2WB_Width = Base.InstWidth + Base.AddrWidth + Base.DecodeWidth + Base.RegIDWidth + (2 * Base.DataWidth) // EX_RET and LS_RET
}

object CSR_LUT{
    val CSR_MSTATUS = "h300".U
    val CSR_MTVEC = "h305".U
    val CSR_MEPC = "h341".U
    val CSR_MCAUSE = "h342".U
}

object InstType{
    val instR = 0.U(Base.InstTypeWidth.W)
    val instI = 1.U(Base.InstTypeWidth.W)
    val instS = 2.U(Base.InstTypeWidth.W)
    val instB = 3.U(Base.InstTypeWidth.W)
    val instU = 4.U(Base.InstTypeWidth.W)
    val instJ = 5.U(Base.InstTypeWidth.W)
}