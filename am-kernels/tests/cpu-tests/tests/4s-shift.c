#include "trap.h"

int main(){
    signed int ans = 0;
    for(signed int a = -2147483648; a < 2147483647; a = a + 1){
        for(signed int b = -2147483648; b < 2147483647; b = b + 1){
            ans = (unsigned int)a << (unsigned int)b;
            //printf("%d, ", ans);
        }
    }
    for(signed int a = -2147483648; a < 2147483647; a = a + 1){
        for(signed int b = -2147483648; b < 2147483647; b = b + 1){
            ans = (unsigned int)a >> (unsigned int)b;
            //printf("%d, ", ans);
        }
    }
    for(signed int a = -2147483648; a < 2147483647; a = a + 1){
        for(signed int b = -2147483648; b < 2147483647; b = b + 1){
            ans = (signed int)a >> (signed int)b;
            //printf("%d, ", ans);
        }
    }
    //printf("\n");
    return 0;
}