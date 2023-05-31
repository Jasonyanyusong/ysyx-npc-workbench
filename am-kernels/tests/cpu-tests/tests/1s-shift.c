#include "trap.h"

int main(){
    signed char ans = 0;
    for(signed char a = -128; a < 127; a = a + 1){
        for(signed char b = 0; b < 8; b = b + 1){
            ans = (unsigned char)a << b;
        }
    }
    for(signed char a = -128; a < 127; a = a + 1){
        for(signed char b = 0; b < 8; b = b + 1){
            ans = (unsigned char)a >> b;
        }
    }
    for(signed char a = -128; a < 127; a = a + 1){
        for(signed char b = 0; b < 8; b = b + 1){
            ans = (signed char)a >> b;
        }
    }
    return 0;
}