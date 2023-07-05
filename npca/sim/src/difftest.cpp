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

#include<difftest.h>

void diff_difftest_init(long img_size){
    if(difftest_enable){
        if(print_debug_informations) {printf("[difftest] enabled, initializing\n");}
        assert(monitor_diff_so_file != NULL);

        void *handle;
        handle = dlopen(monitor_diff_so_file, RTLD_LAZY);
        assert(handle);

        ref_difftest_memcpy = (void (*)(uint64_t, void*, size_t, bool)) dlsym(handle, "difftest_memcpy");
        assert(ref_difftest_memcpy);

        ref_difftest_regcpy = (void (*)(void*, bool)) dlsym(handle, "difftest_regcpy");
        assert(ref_difftest_regcpy);

        ref_difftest_exec = (void (*)(uint64_t)) dlsym(handle, "difftest_exec");
        assert(ref_difftest_exec);

        ref_difftest_raise_intr = (void (*)(uint64_t)) dlsym(handle, "difftest_raise_intr"); // Not implemented in NEMU
        assert(ref_difftest_raise_intr);

        void (*ref_difftest_init)(int) = (void (*)(int)) dlsym(handle, "difftest_init");
        assert(ref_difftest_init);

        if(print_debug_informations) {printf("[difftest] all handeler exist\n");}

        ref_difftest_init(1234);
        if(print_debug_informations) {printf("[difftest] copy mem with start addr 0x%lx\n", mem_start_addr);}
        ref_difftest_memcpy(mem_start_addr, mem_guest_to_host(mem_start_addr), img_size, DIFFTEST_TO_REF);
        if(print_debug_informations) {printf("[difftest] copy mem OK\n");}
        ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF); // Need later changes
        if(print_debug_informations) {printf("[difftest] difftest init OK\n");}
    }else{
        if(print_debug_informations) {printf("[difftest] not enabled, skipping\n");}
        return;
    }
    //difftest_memcpy();
}

void diff_difftest_one_exec(){
    if(diff_skip_ref_exec){
        ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF);
        //printf("[difftest] skipped ref\n");
        diff_skip_ref_exec = false;
        return;
    }
    ref_difftest_exec(1);
}

bool diff_difftest_check_reg(){
    if(diff_skip_ref_exec){
        return true;
    }
    riscv64_CPU_State ref;
    ref_difftest_regcpy(&ref, DIFFTEST_TO_DUT);

    if(cpu.pc != ref.pc)
    {
        printf("[difftest] pc different, difftest failed, dut = 0x%lx, ref = 0x%lx\n", cpu.pc, ref.pc);
        state_set_state(NSIM_ABORT);
        return false;
    }

    for(int i = 0; i < 32; i = i + 1){
        if(cpu.gpr[i] != ref.gpr[i]){
            printf("[difftest] at pc = 0x%lx, gpr x%d different, difftest failed, NSIM's val: 0x%16lx, NEMU's val: 0x%16lx\n", top -> io_NPC_sendCurrentPC - 4, i, cpu.gpr[i], ref.gpr[i]);
            state_set_state(NSIM_ABORT);
            //assert(0);
            return false;
        }
    }

    for(int i = 0; i < 4096; i = i + 1){
        if(cpu.csr[i] != ref.csr[i]){
            printf("[difftest] at pc = 0x%lx, csr x%x different, difftest failed, NSIM's val: 0x%16lx, NEMU's val: 0x%16lx\n", top -> io_NPC_sendCurrentPC - 4, i, cpu.csr[i], ref.csr[i]);
            state_set_state(NSIM_ABORT);
            //assert(0);
            return false;
        }
    }
    if(print_debug_informations) {printf("[difftest] success at current pc\n");}
    return true;
}