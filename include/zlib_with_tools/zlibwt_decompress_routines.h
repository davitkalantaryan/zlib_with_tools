
// zlib_decompress_routines.h
// to include ->  #include "zlib_decompress_routines.h"
// 2018 Feb 12

//
// file:			zlibwt_decompress_routines.h
// path:			include/zlib_with_tools/zlibwt_decompress_routines.h
// created on:		2023 Feb 11
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIBWT_DECOMPRESS_ROUTINES_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIBWT_DECOMPRESS_ROUTINES_H

#include <zlib_with_tools/export_symbols.h>
#include <zlib_with_tools/zlibwt_decompress_data.h>
#include <stddef.h>


CPPUTILS_BEGIN_C

struct ZlibWtDecompressSession;
typedef struct ZlibWtDecompressSession* ZlibWtDecompressSessionPtr;


ZLIBANDTLS_EXPORT ZlibWtDecompressSessionPtr ZlibWtCreateDecompressSession(
	ZlibWtTypeDecompressCallback a_clbk,
	void* userData,
	void* a_bufferForDecompressedData,
	size_t a_sizeForBufferForDecompressedData);

ZLIBANDTLS_EXPORT void ZlibWtDestroyDecompressSession(ZlibWtDecompressSessionPtr a_session);

ZLIBANDTLS_EXPORT int ZlibWtDecompressBufferToCallback(
	ZlibWtDecompressSessionPtr a_session,
	int a_flush,
	const void* a_compressedInputData,
	size_t a_sizeForCompressedInputData);


ZLIBANDTLS_EXPORT void ZlibWtSetBufferForDecompressSession(
	ZlibWtDecompressSessionPtr a_session,
	void* a_bufferForDecompressedData,
	size_t a_sizeForBufferForDecompressedData);


CPPUTILS_END_C


#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIBWT_DECOMPRESS_ROUTINES_H
