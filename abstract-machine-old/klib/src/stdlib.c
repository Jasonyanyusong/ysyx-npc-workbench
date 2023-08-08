#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <unistd.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;

void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}

char* itoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0';
    reverse(str, i);
    return str;
}

char* currentAddr = NULL;

void *malloc(size_t size) {
  // On native, malloc() will be called during initializaion of C runtime.
  // Therefore do not call panic() here, else it will yield a dead recursion:
  //   panic() -> putchar() -> (glibc) -> malloc() -> panic()
#if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
  if(currentAddr == NULL){
    currentAddr =(void *)ROUNDUP(heap.start, 8);
  }
  size = (size_t)ROUNDUP(size, 8);
  char* old = currentAddr;
  currentAddr += size;
  assert((uintptr_t)heap.start <= (uintptr_t)currentAddr && (uintptr_t)currentAddr < (uintptr_t)heap.end);
  for(uint64_t *p = (uint64_t*) old; p!= (uint64_t *)currentAddr; p ++){
    *p = 0;
  }
  return old;
  
#endif
  return NULL;
}

void free(void *ptr) {
}

#endif
