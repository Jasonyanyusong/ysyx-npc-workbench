extern unsigned char rom_mario_nes[];
extern unsigned int rom_mario_nes_len;

struct rom {
  const char *name;
  void *body;
  unsigned int *size;
};

struct rom roms[] = {
  { .name = "mario", .body = rom_mario_nes, .size = &rom_mario_nes_len, },
};
int nroms = 1;
