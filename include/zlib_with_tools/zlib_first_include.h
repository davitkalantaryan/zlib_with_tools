
// zlib_first_include.h
// to include ->  #include "zlib_first_include.h"
// created on 2018 Feb 11

#ifndef zlib_first_include_h
#define zlib_first_include_h

#ifdef _WIN32
#include <io.h>
#define write _write
#else
#include <sys/types.h>
#include <unistd.h>
#endif


#if defined(_MSC_VER)

#ifdef ZLIB_WTOOLS_DISABLE_4996
#pragma warning (disable:4996)
#endif

// warning C4244: '=': conversion from 'ush' to 'uchf', possible loss of data
#ifdef ZLIB_WTOOLS_DISABLE_4244
#pragma warning (disable:4244)
#endif

#ifdef _WIN64
#ifdef ZLIB_WTOOLS_DISABLE_4267
#pragma warning (disable:4267)  // '=': conversion from 'size_t' to 'unsigned int', possible loss of data
#endif
#endif

#endif  //  #if defined(_MSC_VER)


#endif  // #ifndef zlib_first_include_h
