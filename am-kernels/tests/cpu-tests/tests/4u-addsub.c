#include "trap.h"

int main(){
    unsigned int ans = 0;
    for(unsigned int a = 4294967263; a < 4294967295; a = a + 1){
        ans = a + 1;
    }
    for(unsigned int a = 4294967263; a < 4294967295; a = a + 1){
        ans = a - 1;
    }
    return 0;
}