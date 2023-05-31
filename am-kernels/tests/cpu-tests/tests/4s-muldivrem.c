#include "trap.h"

int main(){
    signed int ans = 0;
    for(signed int a = -2147483648; a < -2147483632; a = a + 1){
        for(signed int b = -2147483648; b < -2147483632; b = b + 1){
            ans = a * b;
        }
        for(signed int b = 0; b < 16; b = b + 1){
            ans = a * b;
        }
    }
    for(signed int a = 2147483631; a < 2147483647; a = a + 1){
        for(signed int b = 2147483631; b < 2147483647; b = b + 1){
            ans = a * b;
        }
        for(signed int b = 0; b < 16; b = b + 1){
            ans = a * b;
        }
    }
    for(signed int a = -2147483648; a < -2147483632; a = a + 1){
        for(signed int b = -2147483648; b < -2147483632; b = b + 1){
            ans = a / b;
        }
        for(signed int b = 0; b < 16; b = b + 1){
            ans = a / b;
        }
    }
    for(signed int a = 2147483631; a < 2147483647; a = a + 1){
        for(signed int b = 2147483631; b < 2147483647; b = b + 1){
            ans = a / b;
        }
        for(signed int b = 0; b < 16; b = b + 1){
            ans = a / b;
        }
    }
    for(signed int a = -2147483648; a < -2147483632; a = a + 1){
        for(signed int b = -2147483648; b < -2147483632; b = b + 1){
            ans = a % b;
        }
        for(signed int b = 0; b < 16; b = b + 1){
            ans = a % b;
        }
    }
    for(signed int a = 2147483631; a < 2147483647; a = a + 1){
        for(signed int b = 2147483631; b < 2147483647; b = b + 1){
            ans = a % b;
        }
        for(signed int b = 0; b < 16; b = b + 1){
            ans = a % b;
        }
    }
    for(signed int a = -2147483648; a < -2147483632; a = a + 1){
        for(signed int b = -2147483648; b < -2147483632; b = b + 1){
            ans = b / a;
        }
        for(signed int b = 0; b < 16; b = b + 1){
            ans = b / a;
        }
    }
    for(signed int a = 2147483631; a < 2147483647; a = a + 1){
        for(signed int b = 2147483631; b < 2147483647; b = b + 1){
            ans = b / a;
        }
        for(signed int b = 0; b < 16; b = b + 1){
            ans = b / a;
        }
    }
    for(signed int a = -2147483648; a < -2147483632; a = a + 1){
        for(signed int b = -2147483648; b < -2147483632; b = b + 1){
            ans = b % a;
        }
        for(signed int b = 0; b < 16; b = b + 1){
            ans = b % a;
        }
    }
    for(signed int a = 2147483631; a < 2147483647; a = a + 1){
        for(signed int b = 2147483631; b < 2147483647; b = b + 1){
            ans = b % a;
        }
        for(signed int b = 0; b < 16; b = b + 1){
            ans = b % a;
        }
    }
    return 0;
}