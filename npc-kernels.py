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
TEST_HOME = YSYX_HOME + "/am-kernels/kernels"

VERILOG_PLACE = VERILOG_HOME + "/npc.v"
SIMULATOR_PLACE = CHISEL_HOME + "/sim/" + design_name + ".cpp"

# parse-args
parser = argparse.ArgumentParser(description= "ysyx-npc kernels-test auto-runner, By Jasonyanyusong")
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
print("0: hello")
print("1: litenes")
print("2: nemu")
print("3: slider")
print("4: thread-os")
print("5: typing-game")

kernel_number = int(input("input the test indicator (one digit number)"))

if kernel_number == 0:
    am_make_command = "cd " + TEST_HOME + "/hello && make ARCH=riscv64-npc"
    print(am_make_command)
    os.system(am_make_command)
    am_run_command = YSYX_HOME + "/obj_dir/Vnpc -d " + YSYX_HOME + "/nemu/build/" + test_difftest_so_file + " --readbin " + TEST_HOME + "/hello/build/" + "hello-riscv64-npc.bin" + " -s"
    print(am_run_command)
    os.system(am_run_command)
elif kernel_number == 1:
    am_make_command = "cd " + TEST_HOME + "/litenes && make ARCH=riscv64-npc"
    print(am_make_command)
    os.system(am_make_command)
    am_run_command = YSYX_HOME + "/obj_dir/Vnpc -d " + YSYX_HOME + "/nemu/build/" + test_difftest_so_file + " --readbin " + TEST_HOME + "/litenes/build/" + "litenes-riscv64-npc.bin" + " -s"
    print(am_run_command)
    os.system(am_run_command)
elif kernel_number == 2:
    am_make_command = "cd " + TEST_HOME + "/nemu && make ARCH=riscv64-npc mainargs=test"
    print(am_make_command)
    os.system(am_make_command)
    am_run_command = YSYX_HOME + "/obj_dir/Vnpc -d " + YSYX_HOME + "/nemu/build/" + test_difftest_so_file + " --readbin " + TEST_HOME + "/nemu/build/" + "nemu-riscv64-npc.bin" + " -s"
    print(am_run_command)
    os.system(am_run_command)
elif kernel_number == 3:
    am_make_command = "cd " + TEST_HOME + "/slider && make ARCH=riscv64-npc mainargs=train"
    print(am_make_command)
    os.system(am_make_command)
    am_run_command = YSYX_HOME + "/obj_dir/Vnpc -d " + YSYX_HOME + "/nemu/build/" + test_difftest_so_file + " --readbin " + TEST_HOME + "/slider/build/" + "slider-riscv64-npc.bin" + " -s"
    print(am_run_command)
    os.system(am_run_command)
elif kernel_number == 4:
    am_make_command = "cd " + TEST_HOME + "/thread-os && make ARCH=riscv64-npc mainargs=ref"
    print(am_make_command)
    os.system(am_make_command)
    am_run_command = YSYX_HOME + "/obj_dir/Vnpc -d " + YSYX_HOME + "/nemu/build/" + test_difftest_so_file + " --readbin " + TEST_HOME + "/thread-os/build/" + "thread-os-riscv64-npc.bin" + " -s"
    print(am_run_command)
    os.system(am_run_command)
elif kernel_number == 5:
    am_make_command = "cd " + TEST_HOME + "/typing-game && make ARCH=riscv64-npc mainargs=huge"
    print(am_make_command)
    os.system(am_make_command)
    am_run_command = YSYX_HOME + "/obj_dir/Vnpc -d " + YSYX_HOME + "/nemu/build/" + test_difftest_so_file + " --readbin " + TEST_HOME + "/typing-game/build/" + "typing-game-riscv64-npc.bin" + " -s"
    print(am_run_command)
    os.system(am_run_command)
else:
    print("error")