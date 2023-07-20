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

package npca_future
import chisel3._
import chisel3.util._

class IDUC extends Module{
    val io = IO(new Bundle{
        val IDUC_I_inst                  = Input(UInt(16.W))
        
        val IDUC_O_rs1                   = Output(UInt(5.W))
        val IDUC_O_rs2                   = Output(UInt(5.W))
        val IDUC_O_rd                    = Output(UInt(5.W))

        val IDUC_O_EXU_Int_opcode        = Output(UInt(4.W))
        val IDUC_O_EXU_Int_sign          = Output(UInt(2.W))
        val IDUC_O_EXU_Int_computeLength = Output(UInt(2.W))
        val IDUC_O_EXU_Int_resultPart    = Output(Bool())
        val IDUC_O_EXU_Int_operand       = Output(UInt(4.W))

        val IDUC_O_LSU_optype            = Output(UInt(4.W))
        val IDUC_O_LSU_sign              = Output(Bool())
        val IDUC_O_LSU_len               = Output(UInt(2.W))
        val IDUC_O_LSU_memOperationType  = Output(UInt(2.W))

        val IDUC_O_AMO_memOps            = Output(UInt(4.W)) // no RVC
        val IDUC_O_AMO_opLen             = Output(UInt(2.W)) // no RVC
        val IDUC_O_AMO_aqrl              = Output(UInt(2.W)) // no RVC

        val IDUC_O_writeBackRegType      = Output(UInt(4.W))
        val IDUC_O_privState             = Output(UInt(4.W))
        val IDUC_O_nextPCStatic          = Output(Bool())
        val IDUC_O_PCJumpReason          = Output(UInt(4.W))

        val IDUC_O_csrOperation          = Output(UInt(4.W)) // no RVC
        val IDUC_O_privOperation         = Output(UInt(4.W))
        val IDUC_O_csrAddr               = Output(UInt(12.W)) // 0-4095 CSR index // no RVC

        val IDUC_O_decodeSuccess         = Output(Bool()) // backup decoder for RVC, if fail, raise error

        val IDUC_O_imm                   = Output(UInt(64.W))
    })

    // RVC do not contain Atomic Memory Operation instructions, so just set the Operation code of AMO to nope

    def RVC_RegisterLookup (index : UInt) = MuxCase(0.U(5.W), Array(
        (index === 0.U) -> ( 8.U(5.W)), (index === 1.U) -> ( 9.U(5.W)), (index === 2.U) -> (10.U(5.W)), (index === 3.U) -> (11.U(5.W)),
        (index === 4.U) -> (12.U(5.W)), (index === 5.U) -> (13.U(5.W)), (index === 6.U) -> (14.U(5.W)), (index === 7.U) -> (15.U(5.W)),
    ))

    // First, close AMO decoding process in RVC decoder
    io.IDUC_O_AMO_memOps := opcodes_AMO_memOps.amo_nope
    io.

    val rd_rs1   = io.IDUC_I_inst(11, 7)
    val rs2      = io.IDUC_I_inst( 6, 2)
    val rdc_rs1c = io.IDUC_I_inst( 9, 7)
    val rdc_rs2c = io.IDUC_I_inst( 4, 2)

    val CR_RD  = rd_rs1
    val CR_RS1 = rd_rs1
    val CR_RS2 = rs2
    val CR_IMM = 0.U(64.W)

    val CI_RD  = rd_rs1
    val CI_RS1 = rd_rs1
    val CI_RS2 = 0.U(5.W) // CI instruction type does not contain rs2

    val CSS_RD  = 0.U(5.W) // CSS instruction type does not contain rd
    val CSS_RS1 = 0.U(5.W) // CSS instruction type does not contain rs1
    val CSS_RS2 = rs2

    val CIW_RD  = RVC_RegisterLookup(rdc_rs2c)
    val CIW_RC1 = 0.U(5.W)
    val CIW_RC2 = 0.U(5.W)

    val CL_RD  = RVC_RegisterLookup(rdc_rs2c)
    val CL_RS1 = RVC_RegisterLookup(rdc_rs1c)
    val CL_RS2 = 0.U(5.W)

    val CS_RD  = 0.U(5.W) // CS instruction type does not have RD (This is compressed store instruction)
    val CS_RS1 = RVC_RegisterLookup(rdc_rs1c)
    val CS_RS2 = RVC_RegisterLookup(rdc_rs2c)

    val CA_RD  = RVC_RegisterLookup(rdc_rs1c)
    val CA_RS1 = RVC_RegisterLookup(rdc_rs1c)
    val CA_RS2 = RVC_RegisterLookup(rdc_rs2c)

    val CB_RD  = RVC_RegisterLookup(rdc_rs1c)
    val CB_RS1 = RVC_RegisterLookup(rdc_rs1c)
    val CB_RS2 = 0.U(5.W) // CB (compressed branch) do not need src2 (be equal to zero, be not equal to zero)

    val CJ_RD  = 0.U(5.W) // CJ (compressed jump) do not need GPR write back
    val CJ_RS1 = 0.U(5.W) // CJ (compressed jump) do not need GPR read
    val CJ_RS2 = 0.U(5.W) // CJ (compressed jump) do not need GPR read
}