#include "trap.h"

int main(){
    unsigned int ans = 0;
    for(unsigned int a = 0; a < 4294967295; a = a + 1){
        for(unsigned int b = 0; b < 4294967295; b = b + 1){
            ans = (unsigned int)a << (unsigned int)b;
            //printf("%d, ", ans);
        }
    }
    for(unsigned int a = 0; a < 4294967295; a = a + 1){
        for(unsigned int b = 0; b < 4294967295; b = b + 1){
            ans = (unsigned int)a >> (unsigned int)b;
            //printf("%d, ", ans);
        }
    }
    for(unsigned int a = 0; a < 4294967295; a = a + 1){
        for(unsigned int b = 0; b < 4294967295; b = b + 1){
            ans = (signed int)a >> (signed int)b;
            //printf("%d, ", ans);
        }
    }
    //printf("\n");
    return 0;
}