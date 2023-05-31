#include "trap.h"

int main(){
    signed long long ans = 0;
    for(signed long long a = ~(9223372036854775807LL) + 1; a < ~(9223372036854775807LL) + 33; a = a + 1){
        for(signed long long b = 0; b < 64; b = b + 1){
            ans = (unsigned long long)a << b;
        }
    }
    for(signed long long a = 9223372036854775775LL; a < 9223372036854775807LL; a = a + 1){
        for(signed long long b = 0; b < 64; b = b + 1){
            ans = (unsigned long long)a << b;
        }
    }
    for(signed long long a = ~(9223372036854775807LL) + 1; a < ~(9223372036854775807LL) + 33; a = a + 1){
        for(signed long long b = 0; b < 64; b = b + 1){
            ans = (unsigned long long)a >> b;
        }
    }
    for(signed long long a = 9223372036854775775LL; a < 9223372036854775807LL; a = a + 1){
        for(signed long long b = 0; b < 64; b = b + 1){
            ans = (unsigned long long)a >> b;
        }
    }
    for(signed long long a = ~(9223372036854775807LL) + 1; a < ~(9223372036854775807LL) + 33; a = a + 1){
        for(signed long long b = 0; b < 64; b = b + 1){
            ans = (signed long long)a >> b;
        }
    }
    for(signed long long a = 9223372036854775775LL; a < 9223372036854775807LL; a = a + 1){
        for(signed long long b = 0; b < 64; b = b + 1){
            ans = (signed long long)a >> b;
        }
    }
    return 0;
}