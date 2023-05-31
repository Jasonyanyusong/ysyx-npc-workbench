#include "trap.h"

int main(){
    signed short ans = 0;
    for(signed short a = -32768; a < 32767; a = a + 1){
        for(signed short b = -32768; b < 32767; b = b + 1){
            ans = (unsigned short)a << b;
            //printf("%d, ", ans);
        }
    }
    for(signed short a = -32768; a < 32767; a = a + 1){
        for(signed short b = -32768; b < 32767; b = b + 1){
            ans = (unsigned short)a >> b;
            //printf("%d, ", ans);
        }
    }
    for(signed short a = -32768; a < 32767; a = a + 1){
        for(signed short b = -32768; b < 32767; b = b + 1){
            ans = (signed short)a >> b;
            //printf("%d, ", ans);
        }
    }
    //printf("\n");
    return 0;
}