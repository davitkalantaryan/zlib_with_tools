
// zlib_decompress_routines.h
// to include ->  #include "zlib_decompress_routines.h"
// 2018 Feb 12

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_DECOMPRESS_ROUTINES_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_DECOMPRESS_ROUTINES_H

#include <zlib_with_tools/export_symbols.h>
#include <zlib_with_tools/zlib_compress_decompress_common.h>
#if defined(_WIN32) && defined(ZLIB_DECOMPRESS_FROM_WEB)
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#else
#endif


CPPUTILS_BEGIN_C

// return 0, continues, non 0 stops
#ifndef typeDecompressCallback_defined
typedef int(*typeDecompressCallback)(const void*buffer, int bufLen, void*userData);
#define typeDecompressCallback_defined
#endif

ZLIBANDTLS_EXPORT int ZlibDecompressBufferToCallback(
	z_stream* a_strm,
	void* a_out, int a_outBufferSize,
	typeDecompressCallback a_clbk, void* a_userData);
ZLIBANDTLS_EXPORT int ZlibDecompressBufferToFile(
	z_stream* a_strm,
	void* a_out, int a_outBufferSize,
	FILE *a_dest);
ZLIBANDTLS_EXPORT int ZlibDecompressFile(FILE *source, FILE *dest);
ZLIBANDTLS_EXPORT int ZlibDecompressFolder(FILE *a_source, const char* a_outDirectoryPath);

#ifdef ZLIB_DECOMPRESS_FROM_WEB

ZLIBANDTLS_EXPORT int ZlibDecompressFromWeb(const char *a_webUri, const char* a_outDirectoryPath);

#ifdef _WIN32
ZLIBANDTLS_EXPORT int ZlibBurnImageFromWeb(const char *a_cpcUrl, HANDLE a_drive, __int64 a_nDiskSize);
#else
#endif

#endif


CPPUTILS_END_C


#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_DECOMPRESS_ROUTINES_H
