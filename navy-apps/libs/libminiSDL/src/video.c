#include <NDL.h>
#include <sdl-video.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
  assert(dst && src);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);
  //printf("[miniSDL-BlitSurface] BitsPerPixel = %d\n", src->format->BitsPerPixel);

  int copyH = -1;
  int copyW = -1;

  int copySrcX = -1;
  int copySrcY = -1;

  if(srcrect == NULL){
    copyH = src -> h;
    copyW = src -> w;
    copySrcX = 0;
    copySrcY = 0;
  }else{
    copyH = srcrect -> h;
    copyW = srcrect -> w;
    copySrcX = srcrect -> x;
    copySrcY = srcrect -> y;
  }

  assert(copyH >= 0);
  assert(copyW >= 0);
  assert(copySrcX >= 0);
  assert(copySrcY >= 0);

  int copyDstX = -1;
  int copyDstY = -1;

  //assert(dstrect != NULL);

  if(dstrect == NULL){
    copyDstX = 0;
    copyDstY = 0;
  }else{
    copyDstX = dstrect -> x;
    copyDstY = dstrect -> y;
  }

  assert(copyDstX >= 0);
  assert(copyDstY >= 0);

  if(src->format->BitsPerPixel == 32){
    uint32_t* srcPixels = (uint32_t*)src -> pixels;
    uint32_t* dstPixels = (uint32_t*)dst -> pixels;

    for(int row = 0; row < copyH; row = row + 1){
      for(int col = 0; col < copyW; col = col + 1){
        dstPixels[(copyDstY + row) * (dst -> w) + (copyDstX + col)] = srcPixels[(copySrcY + row) * (src -> w) + (copySrcX + col)];
      }
    }
  }else if(src->format->BitsPerPixel == 8){
    //dst->format->BitsPerPixel = 8;
    for(int row = 0; row < copyH; row = row + 1){
      for(int col = 0; col < copyW; col = col + 1){
        dst -> pixels[(copyDstY + row) * (dst -> w) + (copyDstX + col)] = src -> pixels[(copySrcY + row) * (src -> w) + (copySrcX + col)];
      }
    }
  }else{
    assert(0);
  }
}

void SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, uint32_t color) {

  if(dstrect != NULL){
    //printf("[miniSDL-FillRect] dstrect->w = %d, dstrect->h = %d, dstrect->x = %d, dstrect->y = %d, color = 0x%x\n", dstrect -> w, dstrect -> h, dstrect -> x, dstrect -> y, color);
  }else{
    //printf("[miniSDL-FillRect] dstrect is NULL, color = 0x%x\n", color);
  }

  int fillX = -1;
  int fillY = -1;
  int fillW = -1;
  int fillH = -1;

  if(dstrect == NULL){
    fillX = 0;
    fillY = 0;
    fillW = dst -> w;
    fillH = dst -> h;
  }else{
    fillX = dstrect -> x;
    fillY = dstrect -> y;
    fillW = dstrect -> w;
    fillH = dstrect -> h;
  }

  assert(fillX >= 0);
  assert(fillY >= 0);
  assert(fillW >= 0);
  assert(fillH >= 0);

  //printf("[miniSDL-FillRect] fillX = %d, fillY = %d, fillW = %d, fillH = %d\n", fillX, fillY, fillW, fillH);

  uint32_t* pixels = (uint32_t*)dst->pixels;
  for(int row = 0; row < fillH; row = row + 1){
    for(int col = 0; col < fillW; col = col + 1){
      pixels[((fillY + row) * (dst->w)) + (fillX + col)] = color;
    }
  }
  assert(dst);
}

void SDL_UpdateRect(SDL_Surface *s, int x, int y, int w, int h) {
  //printf("[miniSDL] x = %d, y = %d, w = %d, h = %d\n", x, y, w, h);
  //assert(0);
  //printf("[miniSDL-video] s -> format -> BitsPerPixel = %d\n", s -> format -> BitsPerPixel);

  assert(s);

  if(x == 0 && y == 0 && w == 0 && h == 0){
    x = 0;
    y = 0;
    w = s -> w;
    h = s -> h;
  }

  if(s -> format -> BitsPerPixel == 32){
    assert(s->pixels);
    NDL_DrawRect((uint32_t *)s->pixels, x, y, w, h);
    return;
  }

  if(s -> format -> BitsPerPixel == 8){
    int numPixels = w * h;
    //printf("numPixels = %d\n", numPixels);
    uint32_t* pixels = (uint32_t *)malloc(4 * numPixels);
    for(int i = 0; i < numPixels; i = i + 1){
      SDL_Color thisPixel = s -> format -> palette -> colors[s -> pixels[i]];
      pixels[i] = ((uint32_t)thisPixel.a << 24) | ((uint32_t)thisPixel.r << 16) | ((uint32_t)thisPixel.g << 8) | ((uint32_t)thisPixel.b);
      //printf("colors index = 0x%x -> color = 0x%8x\n", s -> pixels[i], pixels[i]);
    }
    assert(pixels);
    NDL_DrawRect(pixels, x, y, w, h);
    free(pixels);
    return;
  }

  printf("[miniSDL-video] format error (BitsPerPixel)\n");
  assert(0);
  
  //printf("[miniSDL] x = %d, y = %d, s->w = %d, s->h = %d\n", x, y, s->w, s->h);
}

// APIs below are already implemented.

static inline int maskToShift(uint32_t mask) {
  switch (mask) {
    case 0x000000ff: return 0;
    case 0x0000ff00: return 8;
    case 0x00ff0000: return 16;
    case 0xff000000: return 24;
    case 0x00000000: return 24; // hack
    default: assert(0);
  }
}

SDL_Surface* SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
    uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
  assert(depth == 8 || depth == 32);
  SDL_Surface *s = malloc(sizeof(SDL_Surface));
  assert(s);
  s->flags = flags;
  s->format = malloc(sizeof(SDL_PixelFormat));
  assert(s->format);
  if (depth == 8) {
    s->format->palette = malloc(sizeof(SDL_Palette));
    assert(s->format->palette);
    s->format->palette->colors = malloc(sizeof(SDL_Color) * 256);
    assert(s->format->palette->colors);
    memset(s->format->palette->colors, 0, sizeof(SDL_Color) * 256);
    s->format->palette->ncolors = 256;
  } else {
    s->format->palette = NULL;
    s->format->Rmask = Rmask; s->format->Rshift = maskToShift(Rmask); s->format->Rloss = 0;
    s->format->Gmask = Gmask; s->format->Gshift = maskToShift(Gmask); s->format->Gloss = 0;
    s->format->Bmask = Bmask; s->format->Bshift = maskToShift(Bmask); s->format->Bloss = 0;
    s->format->Amask = Amask; s->format->Ashift = maskToShift(Amask); s->format->Aloss = 0;
  }

  s->format->BitsPerPixel = depth;
  s->format->BytesPerPixel = depth / 8;

  s->w = width;
  s->h = height;
  s->pitch = width * depth / 8;
  assert(s->pitch == width * s->format->BytesPerPixel);

  if (!(flags & SDL_PREALLOC)) {
    s->pixels = malloc(s->pitch * height);
    assert(s->pixels);
  }

  return s;
}

SDL_Surface* SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth,
    int pitch, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
  SDL_Surface *s = SDL_CreateRGBSurface(SDL_PREALLOC, width, height, depth,
      Rmask, Gmask, Bmask, Amask);
  assert(pitch == s->pitch);
  s->pixels = pixels;
  return s;
}

void SDL_FreeSurface(SDL_Surface *s) {
  if (s != NULL) {
    if (s->format != NULL) {
      if (s->format->palette != NULL) {
        if (s->format->palette->colors != NULL) free(s->format->palette->colors);
        free(s->format->palette);
      }
      free(s->format);
    }
    if (s->pixels != NULL && !(s->flags & SDL_PREALLOC)) free(s->pixels);
    free(s);
  }
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags) {
  if (flags & SDL_HWSURFACE) NDL_OpenCanvas(&width, &height);
  return SDL_CreateRGBSurface(flags, width, height, bpp,
      DEFAULT_RMASK, DEFAULT_GMASK, DEFAULT_BMASK, DEFAULT_AMASK);
}

void SDL_SoftStretch(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
  assert(src && dst);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);
  assert(dst->format->BitsPerPixel == 8);

  int x = (srcrect == NULL ? 0 : srcrect->x);
  int y = (srcrect == NULL ? 0 : srcrect->y);
  int w = (srcrect == NULL ? src->w : srcrect->w);
  int h = (srcrect == NULL ? src->h : srcrect->h);

  assert(dstrect);
  if(w == dstrect->w && h == dstrect->h) {
    /* The source rectangle and the destination rectangle
     * are of the same size. If that is the case, there
     * is no need to stretch, just copy. */
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_BlitSurface(src, &rect, dst, dstrect);
  }
  else {
    assert(0);
  }
}

void SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, int firstcolor, int ncolors) {
  assert(s);
  assert(s->format);
  assert(s->format->palette);
  assert(firstcolor == 0);

  s->format->palette->ncolors = ncolors;
  memcpy(s->format->palette->colors, colors, sizeof(SDL_Color) * ncolors);

  if(s->flags & SDL_HWSURFACE) {
    assert(ncolors == 256);
    for (int i = 0; i < ncolors; i ++) {
      uint8_t r = colors[i].r;
      uint8_t g = colors[i].g;
      uint8_t b = colors[i].b;
    }
    SDL_UpdateRect(s, 0, 0, 0, 0);
  }
}

static void ConvertPixelsARGB_ABGR(void *dst, void *src, int len) {
  int i;
  uint8_t (*pdst)[4] = dst;
  uint8_t (*psrc)[4] = src;
  union {
    uint8_t val8[4];
    uint32_t val32;
  } tmp;
  int first = len & ~0xf;
  for (i = 0; i < first; i += 16) {
#define macro(i) \
    tmp.val32 = *((uint32_t *)psrc[i]); \
    *((uint32_t *)pdst[i]) = tmp.val32; \
    pdst[i][0] = tmp.val8[2]; \
    pdst[i][2] = tmp.val8[0];

    macro(i + 0); macro(i + 1); macro(i + 2); macro(i + 3);
    macro(i + 4); macro(i + 5); macro(i + 6); macro(i + 7);
    macro(i + 8); macro(i + 9); macro(i +10); macro(i +11);
    macro(i +12); macro(i +13); macro(i +14); macro(i +15);
  }

  for (; i < len; i ++) {
    macro(i);
  }
}

SDL_Surface *SDL_ConvertSurface(SDL_Surface *src, SDL_PixelFormat *fmt, uint32_t flags) {
  assert(src->format->BitsPerPixel == 32);
  assert(src->w * src->format->BytesPerPixel == src->pitch);
  assert(src->format->BitsPerPixel == fmt->BitsPerPixel);

  SDL_Surface* ret = SDL_CreateRGBSurface(flags, src->w, src->h, fmt->BitsPerPixel,
    fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);

  assert(fmt->Gmask == src->format->Gmask);
  assert(fmt->Amask == 0 || src->format->Amask == 0 || (fmt->Amask == src->format->Amask));
  ConvertPixelsARGB_ABGR(ret->pixels, src->pixels, src->w * src->h);

  return ret;
}

uint32_t SDL_MapRGBA(SDL_PixelFormat *fmt, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  assert(fmt->BytesPerPixel == 4);
  uint32_t p = (r << fmt->Rshift) | (g << fmt->Gshift) | (b << fmt->Bshift);
  if (fmt->Amask) p |= (a << fmt->Ashift);
  return p;
}

int SDL_LockSurface(SDL_Surface *s) {
  return 0;
}

void SDL_UnlockSurface(SDL_Surface *s) {
}
