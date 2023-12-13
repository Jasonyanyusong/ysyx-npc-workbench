#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

uint32_t NDL_GetTicks() {
  struct timeval getTime;
  gettimeofday(&getTime, NULL);
  //assert(&getTime);
  //printf("gettime: %d, return: %d\n", getTime.tv_usec, (getTime.tv_usec / 1000));
  return (getTime.tv_usec / 1000);
  //return 0;
}

int NDL_PollEvent(char *buf, int len) {
  int eventFile = open("/dev/events", 0, 0);
  assert(eventFile >= 0);
  //printf("event file's fd is %d\n", eventFile); // This line is OK
  return read(eventFile, buf, len);
}

void NDL_OpenCanvas(int *w, int *h) {
  int dispinfoFile = open("/proc/dispinfo");
  assert(dispinfoFile >= 0);

  char readBuffer[64];
  read(dispinfoFile, readBuffer, sizeof(readBuffer));
  assert(readBuffer);

  //printf("[libndl] Read:\n\n%s\n", readBuffer);
  sscanf(readBuffer, "WIDTH:%d\nHEIGHT:%d\n", &screen_w, &screen_h);
  assert(&screen_w);
  assert(&screen_h);

  //printf("[libndl] width = %d, height = %d\n", screen_w, screen_h);

  if(*w == 0 || *h == 0){
    *w = screen_w;
    *h = screen_h;
  }


  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  //printf("[lib-ndl] w = %d, h = %d\n", w, h);
  int FBFile = open("/dev/fb", 0, 0);
  for(int i = 0; i < h; i = i + 1){
    lseek(FBFile, ((y + i) * screen_w + x) * 4, SEEK_SET);
    //printf("offset = %d\n", ((y + i) * screen_w + x) * 4);
    write(FBFile, pixels + (w * i), w * 4);
    //printf("data = %x\n", *(pixels + w * i));
  }

  /*assert(pixels);
  lseek(FBFile, (x * y), SEEK_SET);
  write(FBFile, pixels, (((uint64_t)w << 32) | ((uint64_t)h & 0x00000000FFFFFFFF)));*/
  close(FBFile);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
