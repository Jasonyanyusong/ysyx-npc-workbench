#include "trap.h"

int main(){
    unsigned long long ans = 0;
    for(unsigned long long a = 0; a < 18446744073709551615LL; a = a + 1){
        for(unsigned long long b = 0; b < 18446744073709551615LL; b = b + 1){
            ans = (unsigned long long)a << b;
            //printf("%d, ", ans);
        }
    }
    for(unsigned long long a = 0; a < 18446744073709551615LL; a = a + 1){
        for(unsigned long long b = 0; b < 18446744073709551615LL; b = b + 1){
            ans = (unsigned long long)a >> b;
            //printf("%d, ", ans);
        }
    }
    for(unsigned long long a = 0; a < 18446744073709551615LL; a = a + 1){
        for(unsigned long long b = 0; b < 18446744073709551615LL; b = b + 1){
            ans = (signed long long)a >> b;
            //printf("%d, ", ans);
        }
    }
    //printf("\n");
    return 0;
}