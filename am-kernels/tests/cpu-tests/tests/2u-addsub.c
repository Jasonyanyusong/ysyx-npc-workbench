#include "trap.h"

int main(){
    unsigned short ans = 0;
    for(unsigned short a = 0; a < 65535; a = a + 1){
        ans = a + 1;
    }
    for(unsigned short a = 0; a < 65535; a = a + 1){
        ans = a - 1;
    }
    return 0;
}