# import packages
import argparse
import os, sys

# configurable parts
design_name = "npca"

# prepare local variables
verilator_name = "verilator"
verilator_compile_flags = "--cc --exe --trace --build --Wno-lint -Wno-style"
verilator_ld_flags = "-LDFLAGS -lreadline -LDFLAGS -lSDL2"

YSYX_HOME = os.getcwd()
GOTO_YSYX_HOME = "cd " + YSYX_HOME
CHISEL_HOME = YSYX_HOME + "/" + design_name
VERILOG_HOME = CHISEL_HOME + "/generated/npc"
TEST_HOME = YSYX_HOME + "/am-kernels/benchmarks"

VERILOG_PLACE = VERILOG_HOME + "/npc.v"
SIMULATOR_PLACE = CHISEL_HOME + "/sim/" + design_name + ".cpp"

# parse-args
parser = argparse.ArgumentParser(description= "ysyx-npc cpu-test auto-runner, By Jasonyanyusong")
parser.add_argument('--difftestSoFile', type=str, default="riscv64-nemu-interpreter-so")

args = parser.parse_args()

test_difftest_so_file = args.difftestSoFile

# sbt compile and generate verilog
sbt_build_command = "cd " + design_name + " && sbt \"test:runMain " + design_name + ".npcMain\""
os.system(sbt_build_command)
input("Press any key to continue")

# verilator generate executable
verilator_command = verilator_name + " " + verilator_compile_flags + " " + VERILOG_PLACE + " " + SIMULATOR_PLACE + " " + verilator_ld_flags
print(verilator_command)
os.system(verilator_command)
input("Press any key to continue")

# am-kernels build executables
print("benchmarkss, type a number to execute")
print("0: coremark")
print("1: dhrystone")
print("2: microbench-test")
print("3: microbench-train")
print("4: microbench-ref")
print("5: microbench-huge")


benchmark_number = int(input("input the test indicator (one digit number)"))

if benchmark_number == 0:
    am_make_command = "cd " + TEST_HOME + "/coremark && make ARCH=riscv64-npc"
    print(am_make_command)
    os.system(am_make_command)
    am_run_command = YSYX_HOME + "/obj_dir/Vnpc -d " + YSYX_HOME + "/nemu/build/" + test_difftest_so_file + " --readbin " + TEST_HOME + "/coremark/build/" + "coremark-riscv64-npc.bin" + " -s"
    print(am_run_command)
    os.system(am_run_command)
elif benchmark_number == 1:
    am_make_command = "cd " + TEST_HOME + "/dhrystone && make ARCH=riscv64-npc"
    print(am_make_command)
    os.system(am_make_command)
    am_run_command = YSYX_HOME + "/obj_dir/Vnpc -d " + YSYX_HOME + "/nemu/build/" + test_difftest_so_file + " --readbin " + TEST_HOME + "/dhrystone/build/" + "dhrystone-riscv64-npc.bin" + " -s"
    print(am_run_command)
    os.system(am_run_command)
elif benchmark_number == 2:
    am_make_command = "cd " + TEST_HOME + "/microbench && make ARCH=riscv64-npc mainargs=test"
    print(am_make_command)
    os.system(am_make_command)
    am_run_command = YSYX_HOME + "/obj_dir/Vnpc -d " + YSYX_HOME + "/nemu/build/" + test_difftest_so_file + " --readbin " + TEST_HOME + "/microbench/build/" + "microbench-riscv64-npc.bin" + " -s"
    print(am_run_command)
    os.system(am_run_command)
elif benchmark_number == 3:
    am_make_command = "cd " + TEST_HOME + "/microbench && make ARCH=riscv64-npc mainargs=train"
    print(am_make_command)
    os.system(am_make_command)
    am_run_command = YSYX_HOME + "/obj_dir/Vnpc -d " + YSYX_HOME + "/nemu/build/" + test_difftest_so_file + " --readbin " + TEST_HOME + "/microbench/build/" + "microbench-riscv64-npc.bin" + " -s"
    print(am_run_command)
    os.system(am_run_command)
elif benchmark_number == 4:
    am_make_command = "cd " + TEST_HOME + "/microbench && make ARCH=riscv64-npc mainargs=ref"
    print(am_make_command)
    os.system(am_make_command)
    am_run_command = YSYX_HOME + "/obj_dir/Vnpc -d " + YSYX_HOME + "/nemu/build/" + test_difftest_so_file + " --readbin " + TEST_HOME + "/microbench/build/" + "microbench-riscv64-npc.bin" + " -s"
    print(am_run_command)
    os.system(am_run_command)
elif benchmark_number == 5:
    am_make_command = "cd " + TEST_HOME + "/microbench && make ARCH=riscv64-npc mainargs=huge"
    print(am_make_command)
    os.system(am_make_command)
    am_run_command = YSYX_HOME + "/obj_dir/Vnpc -d " + YSYX_HOME + "/nemu/build/" + test_difftest_so_file + " --readbin " + TEST_HOME + "/microbench/build/" + "microbench-riscv64-npc.bin" + " -s"
    print(am_run_command)
    os.system(am_run_command)
else:
    print("error")
