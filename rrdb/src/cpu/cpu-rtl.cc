#include <stdio.h>
using namespace std;
extern "C" void attach_rtl(){
  printf("Attach RTL\n");
}