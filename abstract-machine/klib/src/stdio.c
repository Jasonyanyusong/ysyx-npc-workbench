#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
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
          // TODO
          break;
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
