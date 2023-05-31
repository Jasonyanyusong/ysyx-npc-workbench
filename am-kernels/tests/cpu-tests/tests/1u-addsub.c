#include "trap.h"

int main(){
    unsigned char ans = 0;
    for(unsigned char a = 0; a < 255; a = a + 1){
        ans = a + 1;
    }
    for(unsigned char a = 0; a < 255; a = a + 1){
        ans = a - 1;
    }
    return 0;
}