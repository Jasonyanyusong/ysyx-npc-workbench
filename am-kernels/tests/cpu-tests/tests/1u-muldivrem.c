#include "trap.h"

int main(){
    unsigned char ans = 0;
    for(unsigned char a = 0; a < 255; a = a + 1){
        for(unsigned char b = 0; b < 16; b = b + 1){
            ans = a * b;
        }
    }
    for(unsigned char a = 0; a < 255; a = a + 1){
        for(unsigned char b = 0; b < 16; b = b + 1){
            ans = a / b;
        }
    }
    for(unsigned char a = 0; a < 255; a = a + 1){
        for(unsigned char b = 0; b < 16; b = b + 1){
            ans = b / a;
        }
    }
    for(unsigned char a = 0; a < 255; a = a + 1){
        for(unsigned char b = 0; b < 16; b = b + 1){
            ans = a % b;
        }
    }
    for(unsigned char a = 0; a < 255; a = a + 1){
        for(unsigned char b = 0; b < 16; b = b + 1){
            ans = b % a;
        }
    }
    return 0;
}