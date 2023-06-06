.DEFAULT_GOAL = app

WORK_DIR  = $(shell pwd)
BUILD_DIR = $(WORK_DIR)/build

INC_PATH := $(WORK_DIR)/include $(INC_PATH)
OBJ_DIR  = $(BUILD_DIR)/obj-$(NAME)$(SO)
BINARY   = $(BUILD_DIR)/$(NAME)$(SO)

INCLUDES = $(addprefix -I, $(INC_PATH))
VERILATOR = verilator
CFLAGS  := $(INCLUDES)
LDFLAGS := -lreadline -lSDL2 $(shell llvm-config --libs)
VERILOG_FILE := $(NSIM_HOME)/../rtl/generated/npc/npc.v

# Compilation patterns
$(BINARY):
	$(VERILATOR) --cc $(shell find ./src "*.c") $(shell find ./utils "*.c") $(shell find ./utils "*.cc") --exe --build --trace $(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) $(VERILOG_FILE) -Wno-WIDTHEXPAND

# Depencies
-include $(OBJS:.o=.d)

# Some convenient rules

.PHONY: app clean

app: $(BINARY)

clean:
	-rm -rf $(NSIM_HOME)/obj_dir
	-rm -rf $(BUILD_DIR)
