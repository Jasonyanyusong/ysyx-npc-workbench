#include "trap.h"

int main(){
    signed char ans = 0;
    for(signed char a = -128; a < 127; a = a + 1){
        ans = a + 1;
    }
    for(signed char a = -128; a < 127; a = a + 1){
        ans = a - 1;
    }
    return 0;
}