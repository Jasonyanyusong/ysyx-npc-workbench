CROSS_COMPILE := riscv64-linux-gnu-
COMMON_FLAGS  := -fno-pic -march=rv64g -mcmodel=medany -mstrict-align
CFLAGS        += $(COMMON_FLAGS) -static -O3
ASFLAGS       += $(COMMON_FLAGS) -O3
LDFLAGS       += -melf64lriscv
