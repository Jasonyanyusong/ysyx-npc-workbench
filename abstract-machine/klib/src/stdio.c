#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define BITMASK(bits) ((1ull << (bits)) - 1)
#define BITS(x, hi, lo) (((x) >> (lo)) & BITMASK((hi) - (lo) + 1))

char BITSLookUp(int val){
  switch(val){
    case 0 : return '0';
    case 1 : return '1';
    case 2 : return '2';
    case 3 : return '3';
    case 4 : return '4';
    case 5 : return '5';
    case 6 : return '6';
    case 7 : return '7';
    case 8 : return '8';
    case 9 : return '9';
    case 10: return 'a';
    case 11: return 'b';
    case 12: return 'c';
    case 13: return 'd';
    case 14: return 'e';
    case 15: return 'f';
  }
  return 'N';
}

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
        case 'd':
          int64_t printIntVal = va_arg(args, int);
          char printInt[21] = {};
          itoa(printIntVal, printInt, 10);
          int di = 0;
          while(printInt[di] != '\0'){
            putch(printInt[di]);
            di ++;
          }
          break;
        case 'p':
        case 'x':
          int64_t printIntHexVal = va_arg(args, int);
          for(int i = 15; i >= 0; i = i - 1){
            putch(BITSLookUp(BITS(printIntHexVal, 4 * i + 3, 4 * i)));
          }
          break;
        case 'o':
          int64_t printIntOctVal = va_arg(args, int);
          for(int i = 22; i >= 0; i = i - 1){
            putch(BITSLookUp(BITS(printIntHexVal, 3 * i + 2, 3 * i)));
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
