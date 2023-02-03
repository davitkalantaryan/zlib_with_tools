//
// file:			stdlib_zltls.h
// path:			include/zlib_with_tools/stdlib_zltls.h
// created on:		2023 Feb 03
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_STDLIB_ZLIBANDTLS_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_STDLIB_ZLIBANDTLS_H

#include <zlib_with_tools/export_symbols.h>
#include <stdlib.h>


#ifdef _MSC_VER
#include <share.h>
#define fopen_zlibandtls(_filename,_mode)			_fsopen(_filename,_mode,_SH_DENYRW)
#define fileno_zlibandtls							_fileno
#define snprintf_zlibandtls(_buffer,_count,...)		_snprintf_s(_buffer,_count,_count,__VA_ARGS__)
#else
#define fopen_zlibandtls							fopen
#define fileno_zlibandtls							fileno
#define snprintf_zlibandtls							snprintf
#endif

#if defined(MSDOS) || defined(OS2) || defined(_WIN32) || defined(__CYGWIN__)
#include <fcntl.h>
#include <io.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#  define ZLIBANDTLS_SET_BINARY_MODE(file) _setmode(_fileno(file), O_BINARY)
#else
#  define ZLIBANDTLS_SET_BINARY_MODE(file)
#endif


#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_STDLIB_ZLIBANDTLS_H
