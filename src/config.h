#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#if (defined(_WIN32) || defined(__WIN32__) || defined(_WIN32_WINNT)) && !defined(WIN32)
#define WIN32
#endif

#if (defined(_WIN64) || defined(__WIN64__) || defined(_WIN64_WINNT)) && !defined(WIN64)
#define WIN64
#endif

#if ITAG_STATICLIB
#define ITAG_EXPORT
#elif defined(WIN32) || defined(WIN64)

#  if ITAG_BUILD
#    define ITAG_EXPORT  __declspec(dllexport)
#  else
#    define ITAG_EXPORT  __declspec(dllimport)
#  endif
#else
#  define ITAG_EXPORT
#endif

#if ITAG_BUILD
#include "config.h"
#endif

#ifdef  __cplusplus
}
#endif

#endif //_CONFIG_H_
