#include "trap.h"

int main(){
    unsigned long long ans = 0;
    for(unsigned long long a = 18446744073709551551LL; a < 18446744073709551615LL; a = a + 1){
        for(unsigned long long b = 0; b < 64; b = b + 1){
            ans = (unsigned long long)a << b;
        }
    }
    for(unsigned long long a = 18446744073709551551LL; a < 18446744073709551615LL; a = a + 1){
        for(unsigned long long b = 0; b < 64; b = b + 1){
            ans = (unsigned long long)a >> b;
        }
    }
    for(unsigned long long a = 18446744073709551551LL; a < 18446744073709551615LL; a = a + 1){
        for(unsigned long long b = 0; b < 64; b = b + 1){
            ans = (signed long long)a >> b;
        }
    }
    return 0;
}