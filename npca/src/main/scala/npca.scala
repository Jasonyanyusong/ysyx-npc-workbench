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
import chisel3.util._

class npc extends Module{
    val io = IO(new Bundle{
        val NPC_startPC = Input(UInt(64.W))

        val NPC_sendCurrentPC = Output(UInt(64.W))
        val NPC_sendNextPC = Output(UInt(64.W))
        val NPC_getInst = Input(UInt(32.W))
        val NPC_LSU_O_memAddr = Output(UInt(64.W))
        val NPC_LSU_O_accessMem = Output(Bool())
        val NPC_LSU_O_memRW = Output(Bool()) // Low: Read, High: Write
        val NPC_LSU_I_memR = Input(UInt(64.W))
        val NPC_LSU_O_memW = Output(UInt(64.W))
        val NPC_LSU_O_len = Output(UInt(2.W))

        val NPC_GPRchanged = Output(Bool())
        val NPC_halt = Output(Bool())
        //val NPC_error = Output(Bool())

        val NPC_GPR00 = Output(UInt(64.W))
        val NPC_GPR01 = Output(UInt(64.W))
        val NPC_GPR02 = Output(UInt(64.W))
        val NPC_GPR03 = Output(UInt(64.W))
        val NPC_GPR04 = Output(UInt(64.W))
        val NPC_GPR05 = Output(UInt(64.W))
        val NPC_GPR06 = Output(UInt(64.W))
        val NPC_GPR07 = Output(UInt(64.W))
        val NPC_GPR08 = Output(UInt(64.W))
        val NPC_GPR09 = Output(UInt(64.W))
        val NPC_GPR10 = Output(UInt(64.W))
        val NPC_GPR11 = Output(UInt(64.W))
        val NPC_GPR12 = Output(UInt(64.W))
        val NPC_GPR13 = Output(UInt(64.W))
        val NPC_GPR14 = Output(UInt(64.W))
        val NPC_GPR15 = Output(UInt(64.W))
        val NPC_GPR16 = Output(UInt(64.W))
        val NPC_GPR17 = Output(UInt(64.W))
        val NPC_GPR18 = Output(UInt(64.W))
        val NPC_GPR19 = Output(UInt(64.W))
        val NPC_GPR20 = Output(UInt(64.W))
        val NPC_GPR21 = Output(UInt(64.W))
        val NPC_GPR22 = Output(UInt(64.W))
        val NPC_GPR23 = Output(UInt(64.W))
        val NPC_GPR24 = Output(UInt(64.W))
        val NPC_GPR25 = Output(UInt(64.W))
        val NPC_GPR26 = Output(UInt(64.W))
        val NPC_GPR27 = Output(UInt(64.W))
        val NPC_GPR28 = Output(UInt(64.W))
        val NPC_GPR29 = Output(UInt(64.W))
        val NPC_GPR30 = Output(UInt(64.W))
        val NPC_GPR31 = Output(UInt(64.W))
    })

    val PC = RegInit(io.NPC_startPC)
    //PC := io.NPC_startPC;
    val GPR = Mem(32, UInt(64.W))
    def GPR_read (index: UInt) = Mux(index === 0.U, 0.U(64.W), GPR(index)) // So that we make sure R(0) is always 0

    // Step I: Fetch one piece of instruction from Mem
    val npca_IFU = Module(new IFU)
    npca_IFU.io.IFU_I_inst := io.NPC_getInst
    npca_IFU.io.IFU_I_PC := PC
    io.NPC_sendCurrentPC := PC

    // Step II: Decode the instruction: 1) generate OPCODE for LSU and EXU 2) cut RS1, RS2 and RD 3) get SRC1 and SRC2 from GPR 4) Decide if this instruction may cause PC jump
    val npca_IDU = Module(new IDU)
    npca_IDU.io.IDU_I_inst := npca_IFU.io.IFU_I_inst
    //npca_IDU.io.IDU_I_src1 := GPR_read(npca_IDU.io.IDU_O_rs1)
    //npca_IDU.io.IDU_I_src2 := GPR_read(npca_IDU.io.IDU_O_rs2)
    val npca_src1 = GPR_read(npca_IDU.io.IDU_O_rs1)
    val npca_src2 = GPR_read(npca_IDU.io.IDU_O_rs2)
    io.NPC_halt := npca_IDU.io.IDU_O_halt

    // Step III: ALU execution
    val npca_EXU = Module(new EXU)
    npca_EXU.io.EXU_I_src1 := npca_src1
    npca_EXU.io.EXU_I_src2 := npca_src2
    npca_EXU.io.EXU_I_imm := npca_IDU.io.IDU_O_imm
    npca_EXU.io.EXU_I_opcode := npca_IDU.io.IDU_O_EXUopcode
    npca_EXU.io.EXU_I_currentPC := PC

    // Step IV: LSU execution (we access memory using Verilator, NEMU and AM's debug interfaces)
    val npca_LSU = Module(new LSU)
    npca_LSU.io.LSU_I_src1 := npca_EXU.io.EXU_O_result // LSU will get src1 + imm from EXU's result
    npca_LSU.io.LSU_I_src2 := npca_src2
    npca_LSU.io.LSU_I_ModifyMem := npca_IDU.io.IDU_O_ModifyMem
    npca_LSU.io.LSU_I_opcode := npca_IDU.io.IDU_O_LSUopcode
    io.NPC_LSU_O_len := npca_LSU.io.LSU_O_len
    io.NPC_LSU_O_accessMem := npca_LSU.io.LSU_O_accessMem
    io.NPC_LSU_O_memRW := npca_LSU.io.LSU_O_memRW
    io.NPC_LSU_O_memAddr := npca_LSU.io.LSU_O_memAddr
    io.NPC_LSU_O_memW := npca_LSU.io.LSU_O_memW
    npca_LSU.io.LSU_I_memR := io.NPC_LSU_I_memR

    // Step V: Write back data to a GPR and PC
    val npca_WBU = Module(new WBU)
    npca_WBU.io.WBU_I_LSUenable := !(npca_IDU.io.IDU_O_LSUopcode === "b000000".U)
    npca_WBU.io.WBU_I_EXUresult := npca_EXU.io.EXU_O_result
    npca_WBU.io.WBU_I_LSUresult := npca_LSU.io.LSU_O_result
    npca_WBU.io.WBU_I_rd := npca_IDU.io.IDU_O_rd
    io.NPC_GPRchanged := npca_IDU.io.IDU_O_GPRneedWriteBack
    GPR(npca_WBU.io.WBU_O_GPRidx) := Mux(npca_IDU.io.IDU_O_GPRneedWriteBack, npca_WBU.io.WBU_O_GPRWriteBack, GPR_read(npca_WBU.io.WBU_O_GPRidx))
    GPR(0) := 0.U(64.W)
    PC := npca_EXU.io.EXU_O_dynamicNPC
    io.NPC_sendNextPC := PC

    io.NPC_GPR00 := GPR_read(0.U)
    io.NPC_GPR01 := GPR_read(1.U)
    io.NPC_GPR02 := GPR_read(2.U)
    io.NPC_GPR03 := GPR_read(3.U)
    io.NPC_GPR04 := GPR_read(4.U)
    io.NPC_GPR05 := GPR_read(5.U)
    io.NPC_GPR06 := GPR_read(6.U)
    io.NPC_GPR07 := GPR_read(7.U)
    io.NPC_GPR08 := GPR_read(8.U)
    io.NPC_GPR09 := GPR_read(9.U)
    io.NPC_GPR10 := GPR_read(10.U)
    io.NPC_GPR11 := GPR_read(11.U)
    io.NPC_GPR12 := GPR_read(12.U)
    io.NPC_GPR13 := GPR_read(13.U)
    io.NPC_GPR14 := GPR_read(14.U)
    io.NPC_GPR15 := GPR_read(15.U)
    io.NPC_GPR16 := GPR_read(16.U)
    io.NPC_GPR17 := GPR_read(17.U)
    io.NPC_GPR18 := GPR_read(18.U)
    io.NPC_GPR19 := GPR_read(19.U)
    io.NPC_GPR20 := GPR_read(20.U)
    io.NPC_GPR21 := GPR_read(21.U)
    io.NPC_GPR22 := GPR_read(22.U)
    io.NPC_GPR23 := GPR_read(23.U)
    io.NPC_GPR24 := GPR_read(24.U)
    io.NPC_GPR25 := GPR_read(25.U)
    io.NPC_GPR26 := GPR_read(26.U)
    io.NPC_GPR27 := GPR_read(27.U)
    io.NPC_GPR28 := GPR_read(28.U)
    io.NPC_GPR29 := GPR_read(29.U)
    io.NPC_GPR30 := GPR_read(30.U)
    io.NPC_GPR31 := GPR_read(31.U)
}