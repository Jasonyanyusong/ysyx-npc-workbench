ifeq ($(wildcard $(NSIM_HOME)/src/nsim-main.c),)
	$(error NSIM_HOME=$(NSIM_HOME) is not a NSIM repo)
endif

-include $(NSIM_HOME)/include/config/auto.conf
-include $(NSIM_HOME)/include/config/auto.conf.cmd

include $(NSIM_HOME)/scripts/config.mk

remove_quote = $(patsubst "%",%,$(1))

NAME = riscv64-nsim-simulator

BUILD_DIR = $(NSIM_HOME)/build
#MODULE_BUILD_DIR = $(BUILD_DIR)/obj_dir
#BIN_GENNSIM = $(MODULE_BUILD_DIR)

VERILATOR = verilator
VERILATOR_CFLAGS += --trace --build --debug \
					--no-assert -Wno-WIDTHEXPAND

INC_PATH += $(NSIM_HOME)/include \
			$(NSIM_HOME)/include/generated
INCFLAGS = $(addprefix -I, $(INC_PATH))

CFLAGS_BUILD += $(call remove_quote,$(CONFIG_CC_OPT))
CFLAGS_BUILD += $(if $(CONFIG_CC_LTO),-flto,)
CFLAGS_BUILD += $(if $(CONFIG_CC_DEBUG), -Og -ggdb3,)
CFLAGS_BUILD += $(if $(CONFIG_CC_ASAN), -fsanitize=address,)

CFLAGS += $(CFLAGS_BUILD) $(INCFLAGS) $(shell llvm-config --cxxflags) -fPIE

LDFLAGS += $(CFLAGS_BUILD) $(shell llvm-config --libs) -lreadline -lSDL2

SRCC_DIR += $(NSIM_HOME)/src \
			$(NSIM_HOME)/src/device \
			$(NSIM_HOME)/src/memory \
			$(NSIM_HOME)/src/monitor \
			$(NSIM_HOME)/src/rtl \
			$(NSIM_HOME)/src/trace \
			$(NSIM_HOME)/src/utils \

SRCC_DIR_BLACKLIST +=
SRCC_SRC_BLACKLIST +=

SRCC_BLACKLISTS  += $(SRCC_SRC_BLACKLIST) \
					$(shell find $(SRCC_DIR_BLACKLIST) -name "*.c")
SRCC_WHITELISTS  += $(shell find $(SRCC_DIR) -name "*.c")
SRCC_BUILDNSIM   += $(filter-out $(SRCC_BLACKLISTS),$(SRCC_WHITELISTS)) \
					$(NSIM_HOME)/src/utils/diasm.cc

$(BIN_GENNSIM): 
	$(VERILATOR) $(VERILATOR_CFLAGS) \
	$(addprefix -CFLAGS, $(CFLAGS)) \
	$(addprefix -LDFLAGS, $(LDFLAGS)) \
	--exe

ifdef CONFIG_DIFFTEST
ARGS_DIFF = --diff=$(NEMU_HOME)/build/riscv64-nemu-interpreter-so
endif

override ARGS ?= --log=$(NSIM_HOME)/nsim-log.txt
override ARGS += $(ARGS_DIFF)

IMG ?=
NSIM_EXEC := $(BIN_GENNSIM) $(ARGS) $(IMG)

.PHONY: run clean

run:
	$(call git_commit, "run NSIM")
	$(info Generate NSIM)
	$(BIN_GENNSIM)
	$(NSIM_EXEC)

clean: 
	rm -rf build