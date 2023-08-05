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

package npc.helper.opcode

object OpEXU{
    def EX_NOP    =  0.U(7.W)
    def EX_PS1    =  1.U(7.W)
    def EX_ADD    =  2.U(7.W)
    def EX_SLT    =  3.U(7.W)
    def EX_SLTU   =  4.U(7.W)
    def EX_XOR    =  5.U(7.W)
    def EX_OR     =  6.U(7.W)
    def EX_AND    =  7.U(7.W)
    def EX_SLL    =  8.U(7.W)
    def EX_SRL    =  9.U(7.W)
    def EX_SRA    = 10.U(7.W)
    def EX_SUB    = 11.U(7.W)
    def EX_ADDW   = 12.U(7.W)
    def EX_SLLW   = 13.U(7.W)
    def EX_SRLW   = 14.U(7.W)
    def EX_SRAW   = 15.U(7.W)
    def EX_MUL    = 16.U(7.W)
    def EX_MULH   = 17.U(7.W)
    def EX_MULHSU = 18.U(7.W)
    def EX_MULHU  = 19.U(7.W)
    def EX_DIV    = 20.U(7.W)
    def EX_DIVU   = 21.U(7.W)
    def EX_REM    = 22.U(7.W)
    def EX_REMU   = 23.U(7.W)
    def EX_MULW   = 24.U(7.W)
    def EX_DIVW   = 25.U(7.W)
    def EX_DIVUW  = 26.U(7.W)
    def EX_REMW   = 27.U(7.W)
    def EX_REMUW  = 28.U(7.W)
    def EX_SUBW   = 29.U(7.W)
}

object OpLSULen{
    def LS_B = 0.U(2.W)
    def LS_H = 1.U(2.W)
    def LS_W = 2.U(2.W)
    def LS_D = 3.U(2.W)
}

object OpLSUFunc{
    def LS_NOP = 0.U(2.W)
    def LS_LD  = 1.U(2.W)
    def LS_ST  = 2.U(2.W)
    def LS_LDU = 3.U(2.W)
}

object OpWBU{
    def WB_NOP  = 0.U(2.W)
    def WB_EXU  = 1.U(2.W)
    def WB_LSU  = 2.U(2.W)
    def WB_SNPC = 3.U(2.W)
}

object OpPriv{
    def PR_NORM   = 0.U(2.W)
    def PR_ECALL  = 1.U(2.W)
    def PR_MRET   = 2.U(2.W)
    def PR_ZICSR  = 3.U(2.W)
}

object OpState{
    def NPC_RUNNING = 0.U(1.W)
    def NPC_STOPPED = 1.U(1.W)
}

object MemOp{
    def MEM_NOP = 0.U(2.W)
    def MEM_READ = 1.U(2.W)
    def MEM_WRITE = 2.U(2.W)
    def MEM_ATOMIC = 3.U(2.W)
}