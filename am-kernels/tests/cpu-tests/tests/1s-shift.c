#include "trap.h"

int main(){
    signed char ans = 0;
    for(signed char a = -128; a < 127; a = a + 1){
        for(signed char b = -128; b < 127; b = b + 1){
            ans = (unsigned char)a << (unsigned char)b;
            //printf("%d, ", ans);
        }
    }
    for(signed char a = -128; a < 127; a = a + 1){
        for(signed char b = -128; b < 127; b = b + 1){
            ans = (unsigned char)a >> (unsigned char)b;
            //printf("%d, ", ans);
        }
    }
    for(signed char a = -128; a < 127; a = a + 1){
        for(signed char b = -128; b < 127; b = b + 1){
            ans = (signed char)a >> (signed char)b;
            //printf("%d, ", ans);
        }
    }
    //printf("\n");
    return 0;
}