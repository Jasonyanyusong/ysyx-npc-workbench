#include "trap.h"

int main(){
    signed short ans = 0;
    for(signed short a = -32768; a < 32767; a = a + 1){
        for(signed short b = -32768; b < -32760; b = b + 1){
            ans = a * b;
        }
        for(signed short b = 0; b < 8; b = b + 1){
            ans = a * b;
        }
    }
    for(signed short a = -32768; a < 32767; a = a + 1){
        for(signed short b = -32768; b < -32760; b = b + 1){
            ans = a / b;
        }
        for(signed short b = 0; b < 8; b = b + 1){
            ans = a / b;
        }
    }
    for(signed short a = -32768; a < 32767; a = a + 1){
        for(signed short b = -32768; b < -32760; b = b + 1){
            ans = b / a;
        }
        for(signed short b = 0; b < 8; b = b + 1){
            ans = b / a;
        }
    }
    for(signed short a = -32768; a < 32767; a = a + 1){
        for(signed short b = -32768; b < -32760; b = b + 1){
            ans = a % b;
        }
        for(signed short b = 0; b < 8; b = b + 1){
            ans = a % b;
        }
    }
    for(signed short a = -32768; a < 32767; a = a + 1){
        for(signed short b = -32768; b < -32760; b = b + 1){
            ans = b % a;
        }
        for(signed short b = 0; b < 8; b = b + 1){
            ans = b % a;
        }
    }
    return 0;
}