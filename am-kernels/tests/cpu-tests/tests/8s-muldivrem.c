#include "trap.h"

int main(){
    signed long long ans = 0;
    for(signed long long a = ~(9223372036854775807LL) + 1; a < ~(9223372036854775807LL) + 33; a = a + 1){
        for(signed long long b = ~(9223372036854775807LL) + 1; b < ~(9223372036854775807LL) + 17; b = b + 1){
            ans = a * b;
        }
    }
    for(signed long long a = 9223372036854775775LL; a < 9223372036854775807LL; a = a + 1){
        for(signed long long b = 9223372036854775775LL; b < 9223372036854775807LL; b = b + 1){
            ans = a * b;
        }
    }
    for(signed long long a = ~(9223372036854775807LL) + 1; a < ~(9223372036854775807LL) + 33; a = a + 1){
        for(signed long long b = ~(9223372036854775807LL) + 1; b < ~(9223372036854775807LL) + 17; b = b + 1){
            ans = a / b;
        }
    }
    for(signed long long a = 9223372036854775775LL; a < 9223372036854775807LL; a = a + 1){
        for(signed long long b = 9223372036854775775LL; b < 9223372036854775807LL; b = b + 1){
            ans = a / b;
        }
    }
    for(signed long long a = ~(9223372036854775807LL) + 1; a < ~(9223372036854775807LL) + 33; a = a + 1){
        for(signed long long b = ~(9223372036854775807LL) + 1; b < ~(9223372036854775807LL) + 17; b = b + 1){
            ans = b / a;
        }
    }
    for(signed long long a = 9223372036854775775LL; a < 9223372036854775807LL; a = a + 1){
        for(signed long long b = 9223372036854775775LL; b < 9223372036854775807LL; b = b + 1){
            ans = b / a;
        }
    }
    for(signed long long a = ~(9223372036854775807LL) + 1; a < ~(9223372036854775807LL) + 33; a = a + 1){
        for(signed long long b = ~(9223372036854775807LL) + 1; b < ~(9223372036854775807LL) + 17; b = b + 1){
            ans = a % b;
        }
    }
    for(signed long long a = 9223372036854775775LL; a < 9223372036854775807LL; a = a + 1){
        for(signed long long b = 9223372036854775775LL; b < 9223372036854775807LL; b = b + 1){
            ans = a % b;
        }
    }
    for(signed long long a = ~(9223372036854775807LL) + 1; a < ~(9223372036854775807LL) + 33; a = a + 1){
        for(signed long long b = ~(9223372036854775807LL) + 1; b < ~(9223372036854775807LL) + 17; b = b + 1){
            ans = b % a;
        }
    }
    for(signed long long a = 9223372036854775775LL; a < 9223372036854775807LL; a = a + 1){
        for(signed long long b = 9223372036854775775LL; b < 9223372036854775807LL; b = b + 1){
            ans = b % a;
        }
    }
    return 0;
}