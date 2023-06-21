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

object inst_types{
    // We will use a four bit representation of inst types, so it will be very easy to add/modify or optimize our codes
    // Enum all RISC-V Instruction types, so Instruction Decode Unit will know how to devide immediate number, how to get rs and rd
    def inst_R = 0.U(4.W)
    def inst_I = 1.U(4.W)
    def inst_S = 2.U(4.W)
    def inst_B = 3.U(4.W)
    def inst_U = 4.U(4.W)
    def inst_J = 5.U(4.W)
    def inst_N = 6.U(4.W) // ebreak, ecall, mret
    def inst_E = 7.U(4.W) // E indicates that we cought an invalid instruction
}