#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t ans = 0;
  while (*s++ != '\0'){
    ans ++;
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
  int i, j;
  for(i = 0; dst[i] != '\0'; i ++);
  for(j = 0; src[j] != '\0'; j ++){
    dst[i + j] = src[j];
  }
  dst[i + j] = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  while(*s1){
    if(*s1 != *s2){
      break;
    }
    /*if(*s1 == '\0'){
      return 0;
    }*/
    s1 ++;
    s2 ++;
  }
  return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  /*char * chars = (char *) s;
  while(n-- > 0){
    if(*chars != '\0'){
      *chars++ = c;
    }
  }
  return s;*/
  unsigned char *p = (unsigned char *)s;
  for(size_t i = 0; i < n; i = i + 1){
    *(p + i) = c;
  }
  return s;
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
  char* chars1 = (char*) s1;
  char* chars2 = (char*) s2;
  while(n-- > 0){
    if(*chars1 == *chars2){
      chars1 ++;
      chars2 ++;
    }
    else
    {
      return *chars1 - *chars2;
    }
  }
  return 0;
}

#endif
