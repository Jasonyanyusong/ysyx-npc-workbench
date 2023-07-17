#ifndef __CONFIG_H__
#define __CONFIG_H__

#define HAS_GUI

#define SOUND_NONE 0
#define SOUND_LQ   1
#define SOUND_HQ   2

#define PERF_LOW    0
#define PERF_MIDDLE 1
#define PERF_HIGH   2

#if defined(__ARCH_NATIVE) || defined(__PLATFORM_QEMU)
# define PERF_CONFIG PERF_HIGH
#elif defined(__PLATFORM_NEMU)
# define PERF_CONFIG PERF_MIDDLE
#else
# define PERF_CONFIG PERF_LOW
#endif

#if PERF_CONFIG == PERF_HIGH
# define NR_FRAMESKIP 0
# define SOUND_CONFIG SOUND_HQ
#elif PERF_CONFIG == PERF_MIDDLE
# define NR_FRAMESKIP 1
# define SOUND_CONFIG SOUND_LQ
#else
# define NR_FRAMESKIP 2
# define SOUND_CONFIG SOUND_NONE
#endif

#endif
