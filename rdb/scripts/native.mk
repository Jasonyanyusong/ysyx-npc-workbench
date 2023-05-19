#***************************************************************************************
# Copyright (c) 2014-2022 Zihao Yu, Nanjing University
#
# NEMU is licensed under Mulan PSL v2.
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

-include $(RDB_HOME)/../Makefile
include $(RDB_HOME)/scripts/build.mk

include $(RDB_HOME)/tools/difftest.mk

compile_git:
	$(call git_commit, "compile RDB")
$(BINARY): compile_git

# Some convenient rules

override ARGS ?= --log=$(BUILD_DIR)/nemu-log.txt
override ARGS += $(ARGS_DIFF)

# Command to execute RDB
# Enable "-b" in RDB_EXEC to run in batch mode
IMG ?=
RDB_EXEC := $(BINARY) $(ARGS) $(IMG) 

run-env: $(BINARY) $(DIFF_REF_SO)

run: run-env
	$(call git_commit, "run RDB")
	$(RDB_EXEC)

gdb: run-env
	$(call git_commit, "gdb RDB")
	gdb -s $(BINARY) --args $(RDB_EXEC)

lldb: run-env # For Apple ARM-Processors macOS, we can use lldb as gdb is not currently available.
	$(call git_commit, "lldb RDB")
	lldb -s $(BINARY) --  $(RDB_EXEC)

clean-tools = $(dir $(shell find ./tools -maxdepth 2 -mindepth 2 -name "Makefile"))
$(clean-tools):
	-@$(MAKE) -s -C $@ clean
clean-tools: $(clean-tools)
clean-all: clean distclean clean-tools

.PHONY: run gdb run-env clean-tools clean-all $(clean-tools)
.PHONY: run lldb run-env clean-tools clean-all $(clean-tools)
