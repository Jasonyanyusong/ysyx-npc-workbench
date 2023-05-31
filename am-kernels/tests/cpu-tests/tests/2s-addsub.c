#include "trap.h"

int main(){
    signed short ans = 0;
    for(signed short a = -32768; a < 32767; a = a + 1){
        ans = a + 1;
    }
    for(signed short a = -32768; a < 32767; a = a + 1){
        ans = a - 1;
    }
    return 0;
}