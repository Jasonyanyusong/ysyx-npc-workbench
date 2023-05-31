#include "trap.h"

int main(){
    unsigned short ans = 0;
    for(unsigned short a = 0; a < 65535; a = a + 1){
        for(unsigned short b = 0; b < 16; b = b + 1){
            ans = (unsigned short)a << b;
        }
    }
    for(unsigned short a = 0; a < 65535; a = a + 1){
        for(unsigned short b = 0; b < 16; b = b + 1){
            ans = (unsigned short)a >> b;
        }
    }
    for(unsigned short a = 0; a < 65535; a = a + 1){
        for(unsigned short b = 0; b < 16; b = b + 1){
            ans = (signed short)a >> b;
        }
    }
    return 0;
}