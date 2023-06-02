extern unsigned char rom_bomberman_nes[];
extern unsigned int rom_bomberman_nes_len;
extern unsigned char rom_tetris_nes[];
extern unsigned int rom_tetris_nes_len;
extern unsigned char rom_exerion_nes[];
extern unsigned int rom_exerion_nes_len;
extern unsigned char rom_kungfu_nes[];
extern unsigned int rom_kungfu_nes_len;
extern unsigned char rom_castlevania_nes[];
extern unsigned int rom_castlevania_nes_len;
extern unsigned char rom_circus_nes[];
extern unsigned int rom_circus_nes_len;
extern unsigned char rom_skydestroyer_nes[];
extern unsigned int rom_skydestroyer_nes_len;
extern unsigned char rom_mario_nes[];
extern unsigned int rom_mario_nes_len;
extern unsigned char rom_starforce_nes[];
extern unsigned int rom_starforce_nes_len;
extern unsigned char rom_balloon_nes[];
extern unsigned int rom_balloon_nes_len;
extern unsigned char rom_100in1_nes[];
extern unsigned int rom_100in1_nes_len;
extern unsigned char rom_spartanx_nes[];
extern unsigned int rom_spartanx_nes_len;
extern unsigned char rom_lunarpool_nes[];
extern unsigned int rom_lunarpool_nes_len;
extern unsigned char rom_karateka_nes[];
extern unsigned int rom_karateka_nes_len;
extern unsigned char rom_battlecity_nes[];
extern unsigned int rom_battlecity_nes_len;
extern unsigned char rom_mario3_nes[];
extern unsigned int rom_mario3_nes_len;
extern unsigned char rom_pacman_nes[];
extern unsigned int rom_pacman_nes_len;
extern unsigned char rom_f1_nes[];
extern unsigned int rom_f1_nes_len;
extern unsigned char rom_pinball_nes[];
extern unsigned int rom_pinball_nes_len;
extern unsigned char rom_stargate_nes[];
extern unsigned int rom_stargate_nes_len;
extern unsigned char rom_loderunner_nes[];
extern unsigned int rom_loderunner_nes_len;

struct rom {
  const char *name;
  void *body;
  unsigned int *size;
};

struct rom roms[] = {
  { .name = "bomberman", .body = rom_bomberman_nes, .size = &rom_bomberman_nes_len, },
  { .name = "tetris", .body = rom_tetris_nes, .size = &rom_tetris_nes_len, },
  { .name = "exerion", .body = rom_exerion_nes, .size = &rom_exerion_nes_len, },
  { .name = "kungfu", .body = rom_kungfu_nes, .size = &rom_kungfu_nes_len, },
  { .name = "castlevania", .body = rom_castlevania_nes, .size = &rom_castlevania_nes_len, },
  { .name = "circus", .body = rom_circus_nes, .size = &rom_circus_nes_len, },
  { .name = "skydestroyer", .body = rom_skydestroyer_nes, .size = &rom_skydestroyer_nes_len, },
  { .name = "mario", .body = rom_mario_nes, .size = &rom_mario_nes_len, },
  { .name = "starforce", .body = rom_starforce_nes, .size = &rom_starforce_nes_len, },
  { .name = "balloon", .body = rom_balloon_nes, .size = &rom_balloon_nes_len, },
  { .name = "100in1", .body = rom_100in1_nes, .size = &rom_100in1_nes_len, },
  { .name = "spartanx", .body = rom_spartanx_nes, .size = &rom_spartanx_nes_len, },
  { .name = "lunarpool", .body = rom_lunarpool_nes, .size = &rom_lunarpool_nes_len, },
  { .name = "karateka", .body = rom_karateka_nes, .size = &rom_karateka_nes_len, },
  { .name = "battlecity", .body = rom_battlecity_nes, .size = &rom_battlecity_nes_len, },
  { .name = "mario3", .body = rom_mario3_nes, .size = &rom_mario3_nes_len, },
  { .name = "pacman", .body = rom_pacman_nes, .size = &rom_pacman_nes_len, },
  { .name = "f1", .body = rom_f1_nes, .size = &rom_f1_nes_len, },
  { .name = "pinball", .body = rom_pinball_nes, .size = &rom_pinball_nes_len, },
  { .name = "stargate", .body = rom_stargate_nes, .size = &rom_stargate_nes_len, },
  { .name = "loderunner", .body = rom_loderunner_nes, .size = &rom_loderunner_nes_len, },
};
int nroms = 21;
