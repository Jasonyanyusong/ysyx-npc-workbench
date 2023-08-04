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

object Base{
    val NR_GPR = 32
    
    val DataWidth = 64

    val InstWidth = 32
    val InstSize = 4
    
    val AddrWidth = 32

    val RegIDWidth = 5
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
}

object InstType{
    val instR = 0.U(InstTypeWidth.W)
    val instI = 1.U(InstTypeWidth.W)
    val instS = 2.U(InstTypeWidth.W)
    val instB = 3.U(InstTypeWidth.W)
    val instU = 4.U(InstTypeWidth.W)
    val instJ = 5.U(InstTypeWidth.W)
}