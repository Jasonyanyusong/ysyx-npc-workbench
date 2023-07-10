#include <stdio.h>
#include <sys/time.h>
#include <stdbool.h>
#include "NDL.h"

int main(){
    struct timeval timeNow;
    long long timePassed = 0;
    timeNow.tv_sec = 0;
    timeNow.tv_usec = 0;

    long long NDL_sec = 0;
    while(true){
        while(timeNow.tv_sec < timePassed + 1){
            gettimeofday(&timeNow, NULL);
        }
        printf("[timer-test: gettimeofday] %d second passed, tv_sec = %d, tv_usec = %d\n", timeNow.tv_sec, timeNow.tv_sec, timeNow.tv_usec);
        timePassed = timePassed + 1;

        while(NDL_sec < timePassed + 1){
            NDL_sec = NDL_GetTicks() / 1000;
            //printf("NDL sec = %d cond = %d\n", NDL_sec, timePassed + 1);
        }
        printf("[timer-test: NDL_GetTicks] %d second passed, NDL_GetTicks = %d\n", NDL_sec, NDL_GetTicks()); 
        timePassed = timePassed + 1;

        if(timePassed == 120){
            break;
        }
    }
    return 0;
}