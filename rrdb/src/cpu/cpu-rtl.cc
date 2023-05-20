#include <stdio.h>
#include "../../obj_dir/Vnpc.h"
using namespace std;
extern "C" void attach_rtl(){
  printf("Attach RTL\n");
}

extern "C" void rtl_exec_once(){
  // TODO: implement Verilated headers
  printf("Verilated RTL exec once\n");
  return;
}