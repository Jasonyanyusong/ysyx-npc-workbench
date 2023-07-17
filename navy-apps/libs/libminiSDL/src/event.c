#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[64];
  int retNDL_PollEvent = -1;
  retNDL_PollEvent = NDL_PollEvent(buf, sizeof(buf));
  assert(retNDL_PollEvent >= 0);
  //printf("[miniSDL-event: PollEvent] NDL_PollEvent get %d\n", retNDL_PollEvent);
  if(retNDL_PollEvent == 0){
    return 0;
  }else{
    //printf("[miniSDL-event: PollEvent] receive event: %s\n", buf);
    char keyType[3];
    int keyCode= -1;
    sscanf(buf, "%2s %d\n", &keyType, &keyCode);
    assert(keyCode >= 0);
    if(strcmp(keyType, "kd") == 0){
      //printf("[miniSDL-event: PollEvent] KEYDOWN, keycode = %d\n", keyCode);
      ev -> type = SDL_KEYDOWN;
      ev -> key.keysym.sym = keyCode;
    }else if(strcmp(keyType, "ku") == 0){
      //printf("[miniSDL-event: PollEvent] KEY UP , keycode = %d\n", keyCode);
      ev -> type = SDL_KEYUP;
      ev -> key.keysym.sym = keyCode;
    }else{
      assert(0); // should not reach here!
    }
    return 1;
  }
  return -1;
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[64];
  if (NDL_PollEvent(buf, sizeof(buf))) {
    printf("receive event: %s\n", buf);
    char keyType[3];
    int keyCode = -1;
    sscanf(buf, "%2s %d\n", &keyType, &keyCode);
    assert(keyCode >= 0);
    if(strcmp(keyType, "kd") == 0){
      printf("[miniSDL-event] KEYDOWN, keycode = %d\n", keyCode);
      event -> type = SDL_KEYDOWN;
      event -> key.keysym.sym = keyCode;
    }else if(strcmp(keyType, "ku") == 0){
      printf("[miniSDL-event] KEY UP , keycode = %d\n", keyCode);
      event -> type = SDL_KEYUP;
      event -> key.keysym.sym = keyCode;
    }else{
      assert(0); // should not reach here!
    }
  }
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
