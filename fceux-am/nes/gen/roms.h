extern unsigned char rom_mario1_nes[];
extern unsigned int rom_mario1_nes_len;

struct rom {
  const char *name;
  void *body;
  unsigned int *size;
};

struct rom roms[] = {
  { .name = "mario1", .body = rom_mario1_nes, .size = &rom_mario1_nes_len, },
};
int nroms = 1;
