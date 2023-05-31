#include "trap.h"

int main(){
    signed char ans = 0;
    for(signed char a = -32768; a < 32767; a = a + 1){
        for(signed char b = -32768; b < 32767; b = b + 1){
            ans = a * b;
            printf("%d, ", ans);
        }
    }
    for(signed char a = -32768; a < 32767; a = a + 1){
        for(signed char b = -32768; b < 32767; b = b + 1){
            ans = a / b;
            printf("%d, ", ans);
        }
    }
    for(signed char a = -32768; a < 32767; a = a + 1){
        for(signed char b = -32768; b < 32767; b = b + 1){
            ans = a % b;
            printf("%d, ", ans);
        }
    }
    printf("\n");
    return 0;
}