#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t ans = 0;
  while (*s != '\0'){
    ans ++;
    s ++;
  }
  return ans;
}

char *strcpy(char *dst, const char *src) {
  if(dst == NULL){
    return NULL;
  }
  char * ptr = dst;
  while(*src != '\0'){
    *dst = *src;
    dst ++;
    src ++;
  }
  *dst = '\0';
  return ptr;
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
  /*if(dst == NULL){
    return NULL;
  }
  char * ptr = dst;
  while(*src && n--){
    *dst = *src;
    dst ++;
    src ++;
  }
  *dst = '\0';
  return ptr;*/
}

char *strcat(char *dst, const char *src) {
  char * ptr = dst + strlen(dst);
  while(*src != '\0'){
    *ptr++ = *src++;
  }
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  while(*s1){
    if(*s1 != *s2){
      break;
    }
    if(*s1 == '\0'){
      return 0;
    }
    s1 ++;
    s2 ++;
  }
  return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  char * chars = (char *) s;
  for(int i = 0; i < n - 1; i = i + 1){
    if(*chars == '\0'){
      return chars;
    }
    *chars++ = c;
  }
  return chars;
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  char* charin = (char* ) in;
  char* charout = (char* ) out;
  while(n--)
  {
    *charout++ = *charin++;
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  panic("Not implemented");
}

#endif
