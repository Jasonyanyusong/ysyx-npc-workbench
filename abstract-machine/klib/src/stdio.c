#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  va_list args;
  va_start(args, *fmt);
  while(*fmt != '\0'){
    if(*fmt == '%'){
      fmt ++;
      switch(*fmt){
        case 's':
          char* printCharPtr = va_arg(args, char*);
          while(*printCharPtr != '\0'){
            putch(*printCharPtr);
            printCharPtr ++;
          }
          break;
        /*case 'c':
          char printCharVal = va_arg(args, char);
          char printChar[4] = {};
          itoa((long long)printCharVal, printChar, 10);
          int ci = 0;
          while(printChar[ci] != '\0'){
            putch(printChar[ci]);
            ci ++;
          }
          break;*/
        case 'd':
          int printIntVal = va_arg(args, int);
          char printInt[12] = {};
          itoa(printIntVal, printInt, 10);
          int di = 0;
          while(printInt[di] != '\0'){
            putch(printInt[di]);
            di ++;
          }
          break;
      }
      fmt ++;
    }
    putch(*fmt);
    fmt ++;
  }
  va_end(args);
  return 0;
  //panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  char* charout = (char *) out;
  va_list args;
  va_start(args, *fmt);
  while(*fmt != '\0'){
    if(*fmt == '%'){
      fmt++;
      switch(*fmt){
        case 's':
          char * processChar = va_arg(args, char *);
          memcpy(charout, processChar, strlen(processChar));
          charout += strlen(processChar);
          break;
        case 'd':
          char processDecimal[8] = {};
          int processVal = va_arg(args, int);
          itoa(processVal, processDecimal, 10);
          memcpy(charout, processDecimal, strlen(processDecimal));
          charout += strlen(processDecimal);
          break;
        default: break;
      }
    }
    else{
      *charout = *fmt;
      charout ++;
    }
    fmt++;
  }
  va_end(args);
  *charout = '\0';
  return 0;
  //panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
