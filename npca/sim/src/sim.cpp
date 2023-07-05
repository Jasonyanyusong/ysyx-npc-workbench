/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing No.101 High School
* Copyright (c) 2023 Yusong Yan, University of Washington - Seattle
*
* NSIM is licensed under Mulan PSL v2.
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

#include<sim.h>

void sim_sim_init(){
    printf("\33[1;33m[sim] initializing simulation\33[0m\n");
    if(generate_dump_wave_file) {contextp = new VerilatedContext;}
    if(generate_dump_wave_file) {tfp = new VerilatedVcdC;}
    top = new Vnpc;
    if(generate_dump_wave_file) {contextp -> traceEverOn(true);}
    if(generate_dump_wave_file) {top -> trace(tfp, 0);}
    if(generate_dump_wave_file) {tfp -> open("dump.vcd");}
    printf("\33[1;33m[sim] initialize finished\33[0m\n");

    // tell NPC the correct start PC
    top -> clock = 0;
    //top -> io_NPC_startPC = mem_start_addr; // In NPC Advanced, we will automatically set start PC value to 0x80000000
    top -> reset = 1;
    //top -> eval();
    sim_step_and_dump_wave();
    top -> clock = 1;
    //top -> eval();
    sim_step_and_dump_wave();
    top -> reset = 0;
    top -> eval();
    //sim_step_and_dump_wave();

    //tfp -> close();
    printf("\33[1;33m[sim] module's start PC is 0x%x\33[0m\n", mem_start_addr);
    return;
}

void sim_sim_exit(){
    printf("\33[1;33m[sim] exit simulation\33[0m\n");
    sim_step_and_dump_wave();
    if(generate_dump_wave_file) {tfp -> close();}
    return;
}

void sim_one_exec(){
    if(nsim_state.state != NSIM_CONTINUE){
        printf("\33[1;33m[sim] current state indicates simulation can not continue\33[0m\n");
        return;
    }

    uint64_t sim_start_time = host_timer_get_time();

    if(print_debug_informations) {printf("\33[1;33m[sim] execution one round\33[0m\n");}
    top -> clock = 0;// simulate posedge

    // Step I: fetch instruction and send back
    if(print_debug_informations) {printf("\33[1;33m[sim] Phase I: Instruction fetch\33[0m\n");}
    uint64_t sim_getCurrentPC = top -> io_NPC_sendCurrentPC;
    if(print_debug_informations) {printf("\33[1;33m[sim] current pc is 0x%lx\33[0m\n", sim_getCurrentPC);}
    //printf("\33[1;33m[sim] current pc is 0x%lx\33[0m\n", sim_getCurrentPC);

    trace_pc = sim_getCurrentPC; // Update current pc counter's val so trace can work

    uint32_t sim_currentInst = mem_paddr_read(sim_getCurrentPC, 4);
    top -> io_NPC_getCurrentInst = sim_currentInst;
    if(print_debug_informations) {printf("\33[1;33m[sim] current instruction is 0x%x\33[0m\n", sim_currentInst);}

    #ifdef trace_enable_itrace
    trace_itrace_write(sim_currentInst);
    #endif

    top -> eval();

    // Step II: decode instruction
    if(print_debug_informations) {printf("\33[1;33m[sim] Phase II: Instruction decode\33[0m\n");}
    //top -> eval();

    // Step III: EXU execution
    if(print_debug_informations) {printf("\33[1;33m[sim] Phase III: execute\33[0m\n");}
    //top -> eval();

    // Step IV: Load and store
    if(print_debug_informations) {printf("\33[1;33m[sim] Phase IV: load and store\33[0m\n");}

    if(top -> io_NPC_memReadEnable == 0b1){
        u_int64_t sim_mem_read_addr = top -> io_NPC_memReadAddress;
        switch(top -> io_NPC_memReadLength){
            case 0b00: top -> io_NPC_memReadResult = mem_pmem_read(sim_mem_read_addr, 1);     break;
            case 0b01: top -> io_NPC_memReadResult = mem_pmem_read(sim_mem_read_addr, 2);     break;
            case 0b10: top -> io_NPC_memReadResult = mem_pmem_read(sim_mem_read_addr, 4);     break;
            case 0b11: top -> io_NPC_memReadResult = mem_pmem_read(sim_mem_read_addr, 8);     break;
            default:   printf("[sim] NPC returned an unknown memory length\n"); assert(0);    break;
        }
    }else {top -> io_NPC_memReadResult = 0;}

    if(top -> io_NPC_memWriteEnable == 0b1){
        uint64_t sim_mem_write_addr = top -> io_NPC_memWriteAddress;
        switch(top -> io_NPC_memWriteLength){
            case 0b00: mem_pmem_write(sim_mem_write_addr, 1, top -> io_NPC_memWriteData);    break;
            case 0b01: mem_pmem_write(sim_mem_write_addr, 2, top -> io_NPC_memWriteData);    break;
            case 0b10: mem_pmem_write(sim_mem_write_addr, 4, top -> io_NPC_memWriteData);    break;
            case 0b11: mem_pmem_write(sim_mem_write_addr, 8, top -> io_NPC_memWriteData);    break;
            default:   printf("[sim] NPC returned an unknown memory length\n"); assert(0);   break;
        }
    }
    /*if(top -> io_NPC_LSU_O_accessMem == 0b1)
    {
        if(top -> io_NPC_LSU_O_memRW == 0b1){
            // memory write
            uint64_t sim_mem_write_addr = top -> io_NPC_LSU_O_memAddr;
            switch(top -> io_NPC_LSU_O_len){
                case 0b00: mem_pmem_write(sim_mem_write_addr, 1, top -> io_NPC_LSU_O_memW);    break;
                case 0b01: mem_pmem_write(sim_mem_write_addr, 2, top -> io_NPC_LSU_O_memW);    break;
                case 0b10: mem_pmem_write(sim_mem_write_addr, 4, top -> io_NPC_LSU_O_memW);    break;
                case 0b11: mem_pmem_write(sim_mem_write_addr, 8, top -> io_NPC_LSU_O_memW);    break;
                default:   printf("[sim] NPC returned an unknown memory length\n"); assert(0); break;
            }
            top -> io_NPC_LSU_I_memR = 0;
        }else{
            // memory read
            u_int64_t sim_mem_read_addr = top -> io_NPC_LSU_O_memAddr;
            switch(top -> io_NPC_LSU_O_len){
                case 0b00: top -> io_NPC_LSU_I_memR = mem_pmem_read(sim_mem_read_addr, 1);     break;
                case 0b01: top -> io_NPC_LSU_I_memR = mem_pmem_read(sim_mem_read_addr, 2);     break;
                case 0b10: top -> io_NPC_LSU_I_memR = mem_pmem_read(sim_mem_read_addr, 4);     break;
                case 0b11: top -> io_NPC_LSU_I_memR = mem_pmem_read(sim_mem_read_addr, 8);     break;
                default:   printf("[sim] NPC returned an unknown memory length\n"); assert(0); break;
            }
        }
    }
    else{
        top -> io_NPC_LSU_I_memR = 0;
    }*/
    //top -> eval();

    // Step V: Write back
    if(print_debug_informations) {printf("\33[1;33m[sim] Phase V: write back\33[0m\n");}
    //top -> eval();

    sim_step_and_dump_wave();

    top -> clock = 1; //simulate posedge

    top -> eval();

    // We will update devices after the posedge
    device_update();

    reg_get_reg_from_sim();
    //reg_get_pcreg_from_sim();
    /*for(int i = 0; i < 32; i = i + 1){
        cpu.gpr[i] = nsim_gpr[i].value;
    }*/
    //cpu.pc = top -> io_NPC_sendCurrentPC;
    reg_display(false);

    #ifdef trace_enable_rtrace
        trace_rtrace_context context_in_sim;
        for(int i = 0; i < 32; i = i + 1){
            context_in_sim.gpr[i] = nsim_gpr[i].value;
        }
        context_in_sim.pc = top -> io_NPC_sendCurrentPC;
        trace_rtrace_write(context_in_sim);
    #endif

    //nsim_state.state = NSIM_CONTINUE;
    nsim_state.halt_pc = reg_pc;

    if(difftest_enable)
    {
        diff_difftest_one_exec();
        if(!diff_difftest_check_reg())
        {
            state_set_state(NSIM_ABORT);
        }
    }

    if(top -> io_NPC_PrivStatus == 1){ // In NPCA we set 1 to EBREAK, sim need to be stopped
        if(cpu.gpr[10] == 0){
            printf("\33[1;33m[sim] \33[1;32mHIT GOOD TRAP\33[1;33m at pc 0x%lx\33[0m\n", top -> io_NPC_sendCurrentPC - 4);
        }else{
            printf("\33[1;33m[sim] \33[1;31mHIT BAD  TRAP\33[1;33m at pc 0x%lx\33[0m\n", top -> io_NPC_sendCurrentPC - 4);
        }
        sim_step_and_dump_wave();
        state_set_state(NSIM_END);
        return;
    }

    /*if(top -> io_NPC_error == 0b1){
        // NPC raised error, stop simulation
        state_set_state(NSIM_ABORT);
        //nsim_state.state = NSIM_ABORT;
    }*/

    sim_step_and_dump_wave();

    uint64_t sim_finish_time = host_timer_get_time();

    statistics_time_exec += (sim_finish_time - sim_start_time);
    statistics_nr_exec = statistics_nr_exec + 1;

    return;
}

void sim_step_and_dump_wave(){
    top -> eval();
    if(generate_dump_wave_file) {contextp -> timeInc(1);}
    if(generate_dump_wave_file) {tfp -> dump(contextp -> time());}
    return;
}