#***************************************************************************************
# Copyright (c) 2023 Yusong Yan, Beijing No.101 High School
# Copyright (c) 2023 Yusong Yan, University of Washington - Seattle
#
# YSYX-NPCA is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
#
# See the Mulan PSL v2 for more details.
#**************************************************************************************/

CSRC += $(shell find $(abspath ./sim) -name "npca.cpp")
VSRC += $(shell find $(abspath ./generated/npc) -name "*.v")
HSRC += $(shell find $(abspath ./sim) -name "*.h")
TOP_NAME := npc
SEARCHPATH := $(NPC_HOME)/generated/npc

VERILATOR_FLAGS += -I$(SEARCHPATH)
VERILATOR_FLAGS += --top $(TOP_NAME)
VERILATOR_FLAGS += --trace --cc --exe --debug --build
VERILATOR_FLAGS += -Wno-lint
VERILATOR_FLAGS += -Wno-style
VERILATOR_FLAGS += -Wno-UNUSED

INC_PATH		?= $(NPC_HOME)/sim
CFLAGS			+= -g -I$(INC_PATH)
LDFLAGS			+= -lreadline -lSDL2 -ldl

DIFFTEST		:= $(NEMU_HOME)/build/riscv64-nemu-interpreter-so
OBJ_DIR			:= ./obj_dir
BIN				:= $(OBJ_DIR)/V$(TOP_NAME)

IMG				?= ../am-kernels/tests/cpu-tests/build/dummy-riscv64-npc.bin
ELF				?= ../am-kernels/tests/cpu-tests/build/dummy-riscv64-npc.elf

RUN_FLAGS		:= --readbin=${IMG} --readelf=${ELF} --diff=${DIFFTEST} -s
NPC_EXEC		:= ${BIN} ${RUN_FLAGS}

.PHONY:run gdb wave clean

com:${CSRC} ${VSRC}
	@verilator ${VERILATOR_FLAGS} ${CSRC} ${VSRC} $(addprefix -CFLAGS , $(CFLAGS)) -LDFLAGS -lSDL2 -LDFLAGS -lreadline

run:com
	$(call git_commit, "sim NPC")
	@${NPC_EXEC}

gdb:com ${DIFFTEST}
	$(call git_commit, "gdb NPC")
	@gdb --silent -s ${BIN} --args ${NPC_EXEC}

wave:
	@gtkwave $(OBJ_DIR)/dump.vcd

clean:
	rm -rf ${OBJ_DIR}