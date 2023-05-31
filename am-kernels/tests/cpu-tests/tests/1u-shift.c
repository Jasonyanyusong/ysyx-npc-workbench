#include "trap.h"

int main(){
    unsigned char ans = 0;
    for(unsigned char a = 0; a < 255; a = a + 1){
        for(unsigned char b = 0; b < 8; b = b + 1){
            ans = (unsigned char)a << b;
        }
    }
    for(unsigned char a = 0; a < 255; a = a + 1){
        for(unsigned char b = 0; b < 8; b = b + 1){
            ans = (unsigned char)a >> b;
        }
    }
    for(unsigned char a = 0; a < 255; a = a + 1){
        for(unsigned char b = 0; b < 8; b = b + 1){
            ans = (signed char)a >> b;
        }
    }
    return 0;
}