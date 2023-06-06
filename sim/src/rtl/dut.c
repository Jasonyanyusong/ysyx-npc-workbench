#include <dlfcn.h>
#include <rtl.h>

enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };

void diff_difftest_init(long img_size){
    if(difftest_enable){
        printf("[difftest] enabled, initializing\n");
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

        ref_difftest_init(1234);
        ref_difftest_memcpy(mem_start_addr, mem_guest_to_host(mem_start_addr), img_size, DIFFTEST_TO_REF);
        ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF); // Need later changes
    }else{
        printf("[difftest] not enabled, skipping\n");
        return;
    }
    //difftest_memcpy();
}

void diff_difftest_one_exec(){
    ref_difftest_exec(1);
}

bool diff_difftest_check_reg(){
    riscv64_CPU_State ref;
    ref_difftest_regcpy(&ref, DIFFTEST_TO_DUT);
    for(int i = 0; i < 32; i = i + 1){
        if(cpu.gpr[i] != ref.gpr[i]){
            printf("[difftest] gpr x%d different, difftest failed, NSIM's val: 0x%16lx, NEMU's val: 0x%16lx\n", i, cpu.gpr[i], ref.gpr[i]);
            state_set_state(NSIM_ABORT);
            //assert(0);
            return false;
        }
    }
    if(cpu.pc != ref.pc)
    {
        printf("[difftest] pc different, difftest failed\n");
        return false;
    }
    printf("[difftest] success at current pc\n");
    return true;
}

bool rtl_difftest_checkGPR(){
    return false;
}

bool rtl_difftest_checkFPR(){
    return false;
}

bool rtl_difftest_checkCSR(){
    return false;
}