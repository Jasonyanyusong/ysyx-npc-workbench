ifeq ($(wildcard $(RRDB_HOME)/src/rrdb-main),)
  $(error not a RRDB repo)
endif

ifeq ($(wildcard $(RRDB_HOME)/obj_dir/Vnpc.h),)
  #$(error no verilated header files in this repo)
endif

headers = mem.h rrdb.h
cppsrcs = expr.cpp  init.cpp  mem.cpp monitor.cpp rrdb-main.cpp sdb.cpp trace.cpp watchpoint.cpp
cppobjs = expr.o  init.o  mem.o monitor.o rrdb-main.o sdb.o trace.o watchpoint.o

CC=g++
CFLAGS= -c  -Wall

run : $(cppobjs)
  $(CC) $(CFLAGS) -o  rrdb-run  $(cppobjs)
$(cppobjs)  : $(headers)