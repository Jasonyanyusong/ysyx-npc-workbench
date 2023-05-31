#include "trap.h"

int main(){
    signed int ans = 0;
    for(signed int a = -2147483648; a < -2147483632; a = a + 1){
        ans = a + 1;
    }
    for(signed int a = 2147483631; a < 2147483647; a = a + 1){
        ans = a + 1;
    }
    for(signed int a = -2147483648; a < -2147483632; a = a + 1){
        ans = a - 1;
    }
    for(signed int a = 2147483631; a < 2147483647; a = a + 1){
        ans = a - 1;
    }
    return 0;
}