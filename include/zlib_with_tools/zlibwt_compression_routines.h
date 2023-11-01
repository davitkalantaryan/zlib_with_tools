

// zlib_compression_routines.h
// to include ->  #include "zlib_compression_routines.h"
// 2018 Feb 12

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_COMPRESSION_ROUTINES_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_COMPRESSION_ROUTINES_H

#include <zlib_with_tools/export_symbols.h>
#include <zlib_with_tools/zlibwt_compression_data.h>
#include <directory_iterator/directory_iterator.h>
#include <stdio.h>


CPPUTILS_BEGIN_C

struct SZlibWtExtraCompressionBuffer {
	const char* ffileName;
	const char* buffer;
	size_t		bufferSize;
};

// if a_filter returns non zero, then file is skipped, if returns DIRITER_EXIT_ALL(=387), then compression stopped
ZLIBANDTLS_EXPORT int ZlibWtCompressDirectoryEx(
	const char* a_directoryPath,
	int a_compressionLevel,
	ZlibWtTypeCompressCallback a_clbk, TypeDirIterFunc a_filter,
	void* a_userData);

ZLIBANDTLS_EXPORT int ZlibWtCompressDirectoryWithBufferEx(
	const char* a_directoryPath,
	size_t a_extraBuffersCount, const struct SZlibWtExtraCompressionBuffer* CPPUTILS_ARG_NN a_extraBuffers,
	int a_compressionLevel,
	ZlibWtTypeCompressCallback a_clbk, TypeDirIterFunc a_filter,
	void* a_userData);

ZLIBANDTLS_EXPORT int ZlibWtCompressFileEx(
	FILE* a_pFile,
	int a_compressionLevel,
	ZlibWtTypeCompressCallback a_clbk,
	void* a_userData);


CPPUTILS_END_C



#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_COMPRESSION_ROUTINES_H
