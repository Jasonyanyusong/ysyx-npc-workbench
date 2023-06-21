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

object rv64_opcodes{
    // We will use aa ten bit representation of opreation codes, so if need later add inst, this is OK (support up to 1024 different instructions)
    // Both EXU and LSU will use this opcode, for LSU, we can set default to DoNothing if the inst is not load-store related
    // This module is more likely to be used in debugging, IDU will not use that, it will generate a better opcode send to EXU and LSU

    // RV64I
    def opcode_DoNothing =  0.U(10.W)
    def opcode_LUI       =  1.U(10.W)
    def opcode_AUIPC     =  2.U(10.W)
    def opcode_JAL       =  3.U(10.W)
    def opcode_JALR      =  4.U(10.W)
    def opcode_BEQ       =  5.U(10.W)
    def opcode_BNE       =  6.U(10.W)
    def opcode_BLT       =  7.U(10.W)
    def opcode_BGE       =  8.U(10.W)
    def opcode_BLTU      =  9.U(10.W)
    def opcode_BGEU      = 10.U(10.W)
    def opcode_LB        = 11.U(10.W)
    def opcode_LH        = 12.U(10.W)
    def opcode_LW        = 13.U(10.W)
    def opcode_LBU       = 14.U(10.W)
    def opcode_LHU       = 15.U(10.W)
    def opcode_SB        = 16.U(10.W)
    def opcode_SH        = 17.U(10.W)
    def opcode_SW        = 18.U(10.W)
    def opcode_ADDI      = 19.U(10.W)
    def opcode_SLTI      = 20.U(10.W)
    def opcode_SLTIU     = 21.U(10.W)
    def opcode_XORI      = 22.U(10.W)
    def opcode_ORI       = 23.U(10.W)
    def opcode_ANDI      = 24.U(10.W)
    def opcode_SLLI      = 25.U(10.W)
    def opcode_SRLI      = 26.U(10.W)
    def opcode_SRAI      = 27.U(10.W)
    def opcode_ADD       = 28.U(10.W)
    def opcode_SUB       = 29.U(10.W)
    def opcode_SLL       = 30.U(10.W)
    def opcode_SLT       = 31.U(10.W)
    def opcode_SLTU      = 32.U(10.W)
    def opcode_XOR       = 33.U(10.W)
    def opcode_SRL       = 34.U(10.W)
    def opcode_SRA       = 35.U(10.W)
    def opcode_OR        = 36.U(10.W)
    def opcode_AND       = 37.U(10.W)
    def opcode_LWU       = 38.U(10.W)
    def opcode_LD        = 39.U(10.W)
    def opcode_SD        = 40.U(10.W)
    def opcode_ADDIW     = 41.U(10.W)
    def opcode_SLLIW     = 42.U(10.W)
    def opcode_SRLIW     = 43.U(10.W)
    def opcode_SRAIW     = 44.U(10.W)
    def opcode_ADDW      = 45.U(10.W)
    def opcode_SUBW      = 46.U(10.W)
    def opcode_SLLW      = 47.U(10.W)
    def opcode_SRLW      = 48.U(10.W)
    def opcode_SRAW      = 49.U(10.W)

    // RV64M
    def opcode_MUL       = 50.U(10.W)
    def opcode_MULH      = 51.U(10.W)
    def opcode_MULHSU    = 52.U(10.W)
    def opcode_MULHU     = 53.U(10.W)
    def opcode_DIV       = 54.U(10.W)
    def opcode_DIVU      = 55.U(10.W)
    def opcode_REM       = 56.U(10.W)
    def opcode_REMU      = 57.U(10.W)
    def opcode_MULW      = 58.U(10.W)
    def opcode_DIVW      = 59.U(10.W)
    def opcode_DIVUW     = 60.U(10.W)
    def opcode_REMW      = 61.U(10.W)
    def opcode_REMUW     = 62.U(10.W)

    // Priv
    def opcode_ECALL     = 63.U(10.W)
    def opcode_EBREAK    = 64.U(10.W)

    // RV64 Zicsr
    def opcode_CSRRW     = 65.U(10.W)
    def opcode_CSRRS     = 66.U(10.W)
    def opcode_CSRRC     = 67.U(10.W)
    def opcode_CSRRWI    = 68.U(10.W)
    def opcode_CSRRSI    = 69.U(10.W)
    def opcode_CSRRCI    = 70.U(10.W)
}