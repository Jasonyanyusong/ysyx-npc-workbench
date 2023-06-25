/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing 101 High School
* Copyright (c) 2023 Yusong Yan, University of Washington - Seattle
*
* YSYX-NPCA is licensed under Mulan PSL v2.
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

package npca
import chisel3._
import chisel3-util._

class LSU extends Module{
    val io = IO(new Bundle{
        val LSU_I_optype           = Input(UInt(4.W))
        val LSU_I_sign             = Input(Bool())
        val LSU_I_len              = Input(UInt(2.W))
        val LSU_I_memOperationType = Input(UInt(2.W))

        val LSU_I_EXUresult        = Input(UInt(64.W)) // Normal Load-Store Instructions
        val LSU_I_src1             = Input(UInt(64.W)) // Atomic Load-Store Instructions
        val LSU_I_src2             = Input(UInt(64.W))

        val LSU_O_memReadEnable    = Output(Bool())
        val LSU_O_memReadAddress   = Output(UInt(64.W))
        val LSU_I_memReadResult    = Input(UInt(64.W))

        val LSU_O_memWriteEnable   = Output(Bool())
        val LSU_O_memWriteAddress  = Output(UInt(64.W))
        val LSU_O_memWriteData     = Output(UInt(64.W))

        val LSU_O_resultWriteBack  = Output(UInt(64.W))
    })
}