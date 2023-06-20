# import packages
import argparse
import os

# prepare local variables
verilator_name = "verilator --threads 2"
verilator_compile_flags = "--cc --exe --trace --build --Wno-lint -Wno-style"
verilator_ld_flags = "-LDFLAGS -lreadline -LDFLAGS -lSDL2"

YSYX_HOME = os.getcwd()
GOTO_YSYX_HOME = "cd " + YSYX_HOME
CHISEL_HOME = YSYX_HOME + "/rtl"
VERILOG_HOME = CHISEL_HOME + "/generated/npc"
TEST_HOME = YSYX_HOME + "/am-kernels/tests/am-tests"

# parse-args
parser = argparse.ArgumentParser(description= "ysyx-npc am-test auto-runner, By Jasonyanyusong")
parser.add_argument('--difftestSoFile', type=str, default="riscv64-nemu-interpreter-so")

args = parser.parse_args()

test_difftest_so_file = args.difftestSoFile

# sbt compile and generate verilog
os.system("cd rtl && sbt \"test:runMain npc.npcMain\"")

# verilator generate executable
verilator_command = verilator_name + " " + verilator_compile_flags + " " + VERILOG_HOME + "/npc.v" + " " + YSYX_HOME + "/nsim.cpp" + " " + verilator_ld_flags
os.system(verilator_command)

# am-kernels build executables
print("am-tests, type a test to execute")
print("h: hello")
print("i: interrupt/yield test")
print("d: scan devices")
print("m: multiprocessor test")
print("t: real-time clock test")
print("k: readkey test")
print("v: display test")
print("a: audio test")
print("p: x86 virtual memory test")

am_test_name = input("input the test indicator (one char)")
am_make_command = "make ARCH=riscv64-npc mainargs=" + am_test_name
os.system("cd ./am-kernels/tests/am-tests && " + am_make_command)

test_exec_command = YSYX_HOME + "/obj_dir/Vnpc -d " + YSYX_HOME + "/nemu/build/" + test_difftest_so_file + " --readbin " + TEST_HOME + "/build/" + "amtest-riscv64-npc.bin" + " -s"
os.system(test_exec_command)