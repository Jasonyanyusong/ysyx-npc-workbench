# import packages
import argparse
import os, sys

# prepare local variables
verilator_name = "verilator"
verilator_compile_flags = "--cc --exe --trace --build --Wno-lint -Wno-style"
verilator_ld_flags = "-LDFLAGS -lreadline -LDFLAGS -lSDL2"

YSYX_HOME = os.getcwd()
GOTO_YSYX_HOME = "cd " + YSYX_HOME
CHISEL_HOME = YSYX_HOME + "/rtl"
VERILOG_HOME = CHISEL_HOME + "/generated/npc"
TEST_HOME = YSYX_HOME + "/am-kernels/tests/cpu-tests"
TEST_CSRCS = TEST_HOME + "/tests"

# parse-args
parser = argparse.ArgumentParser(description= "ysyx-npc cpu-test auto-runner, By Jasonyanyusong")
parser.add_argument('--difftestSoFile', type=str, default="riscv64-nemu-interpreter-so")

args = parser.parse_args()

test_difftest_so_file = args.difftestSoFile

# sbt compile and generate verilog
os.system("cd rtl && sbt \"test:runMain npc.npcMain\"")

# verilator generate executable
verilator_command = verilator_name + " " + verilator_compile_flags + " " + VERILOG_HOME + "/npc.v" + " " + YSYX_HOME + "/nsim.cpp" + " " + verilator_ld_flags
os.system(verilator_command)

# am-kernels build executables
os.system("cd ./am-kernels/tests/cpu-tests && make ARCH=riscv64-npc")

# get cpu-tests list
cpu_tests_csrcs = os.listdir(TEST_CSRCS)
cpu_tests_names = []
cpu_tests_count = len(cpu_tests_csrcs)
for i in range(0, cpu_tests_count, 1):
    cpu_tests_names.append(cpu_tests_csrcs[i][0:cpu_tests_csrcs[i].index(".c")])

# start executing cpu-tests
cpu_tests_result = []
for i in range(0, cpu_tests_count, 1):
    test_exec_command = YSYX_HOME + "/obj_dir/Vnpc -d " + YSYX_HOME + "/nemu/build/" + test_difftest_so_file + " --readbin " + TEST_HOME + "/build/" + cpu_tests_names[i] + "-riscv64-npc.bin" + " -s"
    os.system(test_exec_command)
    print()
    print("CPU-Test: ", cpu_tests_names[i])
    cpu_tests_result.append(input("Press any key to continue"))

# display results
#for i in range(0, cpu_tests_count, 1):
    #print(cpu_tests_names[i], end="\t\t\t")
    #if  cpu_tests_result == 0x80000000:
        #print(" PASS", end="")
    #else:
        #print(" FAIL", end="")
    #pc_str = hex(cpu_tests_result[i])
    #print(" at PC = ", end="")
    #print(pc_str)