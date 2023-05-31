#include "trap.h"

int main(){
    unsigned char ans = 0;
    for(unsigned char a = 0; a < 255; a = a + 1){
        for(unsigned char b = 0; b < 255; b = b + 1){
            ans = a + b;
            printf("%d, ", ans);
        }
    }
    for(unsigned char a = 0; a < 255; a = a + 1){
        for(unsigned char b = 0; b < 255; b = b + 1){
            ans = a - b;
            printf("%d, ", ans);
        }
    }
    printf("\n");
    return 0;
}