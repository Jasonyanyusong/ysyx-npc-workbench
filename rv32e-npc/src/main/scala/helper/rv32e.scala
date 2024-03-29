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

package npc.helper.rv32e

import chisel3._
import chisel3.util._

object Inst{
    // Non-Priv
    def LUI    = BitPat("b???????_?????_?????_???_?????_01101_11")
    def AUIPC  = BitPat("b???????_?????_?????_???_?????_00101_11")
    def JAL    = BitPat("b???????_?????_?????_???_?????_11011_11")
    def JALR   = BitPat("b???????_?????_?????_000_?????_11001_11")
    def BEQ    = BitPat("b???????_?????_?????_000_?????_11000_11")
    def BNE    = BitPat("b???????_?????_?????_001_?????_11000_11")
    def BLT    = BitPat("b???????_?????_?????_100_?????_11000_11")
    def BGE    = BitPat("b???????_?????_?????_101_?????_11000_11")
    def BLTU   = BitPat("b???????_?????_?????_110_?????_11000_11")
    def BGEU   = BitPat("b???????_?????_?????_111_?????_11000_11")
    def LB     = BitPat("b???????_?????_?????_000_?????_00000_11")
    def LH     = BitPat("b???????_?????_?????_001_?????_00000_11")
    def LW     = BitPat("b???????_?????_?????_010_?????_00000_11")
    def LBU    = BitPat("b???????_?????_?????_100_?????_00000_11")
    def LHU    = BitPat("b???????_?????_?????_101_?????_00000_11")
    def SB     = BitPat("b???????_?????_?????_000_?????_01000_11")
    def SH     = BitPat("b???????_?????_?????_001_?????_01000_11")
    def SW     = BitPat("b???????_?????_?????_010_?????_01000_11")
    def ADDI   = BitPat("b???????_?????_?????_000_?????_00100_11")
    def SLTI   = BitPat("b???????_?????_?????_010_?????_00100_11")
    def SLTIU  = BitPat("b???????_?????_?????_011_?????_00100_11")
    def XORI   = BitPat("b???????_?????_?????_100_?????_00100_11")
    def ORI    = BitPat("b???????_?????_?????_110_?????_00100_11")
    def ANDI   = BitPat("b???????_?????_?????_111_?????_00100_11")
    def SLLI   = BitPat("b000000?_?????_?????_001_?????_00100_11")
    def SRLI   = BitPat("b000000?_?????_?????_101_?????_00100_11")
    def SRAI   = BitPat("b010000?_?????_?????_101_?????_00100_11")
    def ADD    = BitPat("b0000000_?????_?????_000_?????_01100_11")
    def SUB    = BitPat("b0100000_?????_?????_000_?????_01100_11")
    def SLL    = BitPat("b0000000_?????_?????_001_?????_01100_11")
    def SLT    = BitPat("b0000000_?????_?????_010_?????_01100_11")
    def SLTU   = BitPat("b0000000_?????_?????_011_?????_01100_11")
    def XOR    = BitPat("b0000000_?????_?????_100_?????_01100_11")
    def SRL    = BitPat("b0000000_?????_?????_101_?????_01100_11")
    def SRA    = BitPat("b0100000_?????_?????_101_?????_01100_11")
    def OR     = BitPat("b0000000_?????_?????_110_?????_01100_11")
    def AND    = BitPat("b0000000_?????_?????_111_?????_01100_11")

    // Priv
    def ECALL  = BitPat("b0000000_00000_00000_000_00000_11100_11")
    def EBREAK = BitPat("b0000000_00001_00000_000_00000_11100_11")
    def MRET   = BitPat("b0011000_00010_00000_000_00000_11100_11")

    // Zicsr
    def CSRRW  = BitPat("b???????_?????_?????_001_?????_11100_11")
    def CSRRS  = BitPat("b???????_?????_?????_010_?????_11100_11")
    def CSRRC  = BitPat("b???????_?????_?????_011_?????_11100_11")
    def CSRRWI = BitPat("b???????_?????_?????_101_?????_11100_11")
    def CSRRSI = BitPat("b???????_?????_?????_110_?????_11100_11")
    def CSRRCI = BitPat("b???????_?????_?????_111_?????_11100_11")
}