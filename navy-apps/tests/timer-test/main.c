#include <stdio.h>
#include <sys/time.h>
#include <stdbool.h>

int main(){
    struct timeval timeNow;
    long long timePassed = 0;
    timeNow.tv_sec = 0;
    timeNow.tv_usec = 0;
    while(true){
        while(timeNow.tv_sec < timePassed + 1){
            gettimeofday(&timeNow, NULL);
        }
        printf("[timer-test] 1 second passed, tv_sec = %d, tv_usec = %d\n", timeNow.tv_sec, timeNow.tv_usec);
        timePassed = timePassed + 1;
    }
    return 0;
}