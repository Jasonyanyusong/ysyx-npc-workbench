#include "trap.h"

int main(){
    unsigned short ans = 0;
    for(unsigned short a = 0; a < 65535; a = a + 1){
        for(unsigned short b = 0; b < 65535; b = b + 1){
            ans = (unsigned short)a << (unsigned short)b;
            //printf("%d, ", ans);
        }
    }
    for(unsigned short a = 0; a < 65535; a = a + 1){
        for(unsigned short b = 0; b < 65535; b = b + 1){
            ans = (unsigned short)a >> (unsigned short)b;
            //printf("%d, ", ans);
        }
    }
    for(unsigned short a = 0; a < 65535; a = a + 1){
        for(unsigned short b = 0; b < 65535; b = b + 1){
            ans = (signed short)a >> (signed short)b;
            //printf("%d, ", ans);
        }
    }
    //printf("\n");
    return 0;
}