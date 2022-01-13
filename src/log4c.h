enum LOGLEVEL{
  LEVEL_VERBOSE = 0,
  LEVEL_DEBUG,
  LEVEL_INFO,
  LEVEL_WARN,
  LEVEL_ERROR,
  LEVEL_FATAL,
  LEVEL_DISABLED,
};

#ifndef LEVEL_PRECOMPILED
#define LEVEL_PRECOMPILED LEVEL_VERBOSE
#endif

#define TRACE(level, ...) \
    do { \
        if (level >= LEVEL_PRECOMPILED) \
             fprintf(stderr, "[%d:%s:%d->%s]" __VA_ARGS__, level, __FILE__, __LINE__, __func__); \
    } while(0)



// trace log MACROs for users
#define DEBUG(...) \
    do { \
        if (LEVEL_PRECOMPILED >= LEVEL_DEBUG) \
          TRACE(LEVEL_PRECOMPILED, __VA_ARGS__); \
        else fprintf(stdout, __VA_ARGS__); \
    } while(0)

#define ERROR(...) \
    do { \
        if (LEVEL_PRECOMPILED >= LEVEL_ERROR) \
          TRACE(LEVEL_PRECOMPILED, __VA_ARGS__); \
        else fprintf(stdout, __VA_ARGS__); \
    } while(0)

#define FATAL(...) \
    do { \
        if (LEVEL_PRECOMPILED >= LEVEL_FATAL) \
          TRACE(LEVEL_PRECOMPILED, __VA_ARGS__); \
        else fprintf(stdout, __VA_ARGS__); \
    } while(0)

#define DISABLED(...) \
    do { \
        if (LEVEL_PRECOMPILED >= LEVEL_DISABLED) \
          TRACE(LEVEL_PRECOMPILED, __VA_ARGS__); \
        else fprintf(stdout, __VA_ARGS__); \
    } while(0)
