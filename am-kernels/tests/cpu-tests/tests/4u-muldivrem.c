#include "trap.h"

int main(){
    unsigned int ans = 0;
    for(unsigned int a = 4294967263; a < 4294967295; a = a + 1){
        for(unsigned int b = 0; b < 32; b = b + 1){
            ans = a * b;
        }
    }
    for(unsigned int a = 4294967263; a < 4294967295; a = a + 1){
        for(unsigned int b = 0; b < 32; b = b + 1){
            ans = a / b;
        }
    }
    for(unsigned int a = 4294967263; a < 4294967295; a = a + 1){
        for(unsigned int b = 0; b < 32; b = b + 1){
            ans = b / a;
        }
    }
    for(unsigned int a = 4294967263; a < 4294967295; a = a + 1){
        for(unsigned int b = 0; b < 32; b = b + 1){
            ans = a % b;
        }
    }
    for(unsigned int a = 4294967263; a < 4294967295; a = a + 1){
        for(unsigned int b = 0; b < 32; b = b + 1){
            ans = b % a;
        }
    }
    return 0;
}