#include "trap.h"

int main(){
    signed short ans = 0;
    for(signed short a = -32768; a < 32767; a = a + 1){
        for(signed short b = 0; b < 8; b = b + 1){
            ans = (unsigned short)a << b;
        }
    }
    for(signed short a = -32768; a < 32767; a = a + 1){
        for(signed short b = 0; b < 8; b = b + 1){
            ans = (unsigned short)a >> b;
        }
    }
    for(signed short a = -32768; a < 32767; a = a + 1){
        for(signed short b = 0; b < 8; b = b + 1){
            ans = (signed short)a >> b;
        }
    }
    return 0;
}