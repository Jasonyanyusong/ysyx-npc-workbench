#include "trap.h"

int main(){
    signed long long ans = 0;
    for(signed long long a = ~(9223372036854775807LL) + 1; a < 9223372036854775807LL; a = a + 1){
        for(signed long long b = ~(9223372036854775807LL) + 1; b < 9223372036854775807LL; b = b + 1){
            ans = a + b;
            //printf("%d, ", ans);
        }
    }
    for(signed long long a = ~(9223372036854775807LL) + 1; a < 9223372036854775807LL; a = a + 1){
        for(signed long long b = ~(9223372036854775807LL) + 1; b < 9223372036854775807LL; b = b + 1){
            ans = a - b;
            //printf("%d, ", ans);
        }
    }
    //printf("\n");
    return 0;
}