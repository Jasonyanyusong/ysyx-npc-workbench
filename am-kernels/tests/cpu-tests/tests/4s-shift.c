#include "trap.h"

int main(){
    signed int ans = 0;
    for(signed int a = -2147483648; a < 2147483647; a = a + 1){
        for(signed int b = 0; b < 32; b = b + 1){
            ans = (unsigned int)a << b;
        }
    }
    for(signed int a = -2147483648; a < 2147483647; a = a + 1){
        for(signed int b = 0; b < 32; b = b + 1){
            ans = (unsigned int)a >> b;
        }
    }
    for(signed int a = -2147483648; a < 2147483647; a = a + 1){
        for(signed int b = 0; b < 32; b = b + 1){
            ans = (signed int)a >> b;
        }
    }
    return 0;
}