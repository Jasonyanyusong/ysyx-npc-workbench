#include <am.h>
#include <nemu.h>

uint64_t start_time = 0;

uint64_t get_time(){
  uint32_t time_low = inl(RTC_ADDR);
  uint32_t time_high = inl(RTC_ADDR + 4);
  return (time_high << 32) + time_low;
}

void __am_timer_init() {
  start_time = get_time();
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = get_time() - start_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
