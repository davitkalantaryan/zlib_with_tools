//
// file:			zlibwt_ll_decompress_routines.h
// path:			src/include/private/zlib_with_tools/zlibwt_ll_decompress_routines.h
// created on:		2023 Feb 10
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIBWT_LL_DECOMPRESS_ROUTINES_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIBWT_LL_DECOMPRESS_ROUTINES_H

#include <zlib_with_tools/export_symbols.h>
#include <zlib_with_tools/zlibwt_decompress_data.h>


CPPUTILS_BEGIN_C

struct ZlibWtLLDecompressSession;
typedef struct ZlibWtLLDecompressSession* ZlibWtLLDecompressSessionPtr;

ZLIBANDTLS_LL_EXPORT ZlibWtLLDecompressSessionPtr ZlibWtCreateLLDecompressSession(
	size_t a_size, // in case if a_size<sizeof(ZlibWtLLDecompressSession), this will be replaced by sizeof(ZlibWtLLDecompressSession)
	ZlibWtTypeLLDecompressCallback a_clbk,
	void* userData,
	void* a_bufferForDecompressedData,
	size_t a_sizeForBufferForDecompressedData);

ZLIBANDTLS_LL_EXPORT void ZlibWtDestroyLLDecompressSession(ZlibWtLLDecompressSessionPtr a_session);

ZLIBANDTLS_LL_EXPORT int ZlibWtLLDecompressBufferToCallback(
	ZlibWtLLDecompressSessionPtr a_session,
	int a_flush,
	const void* a_compressedInputData,
	size_t a_sizeForCompressedInputData);

ZLIBANDTLS_LL_EXPORT int ZlibWtLLDecompressBufferToCallbackNoTouchOut(
	ZlibWtLLDecompressSessionPtr a_session,
	int a_flush,
	const void* a_compressedInputData,
	size_t a_sizeForCompressedInputData);

ZLIBANDTLS_LL_EXPORT int ZlibWtLLDecompressBufferToCallbackReadOldIn(ZlibWtLLDecompressSessionPtr a_session,int a_flush);

ZLIBANDTLS_LL_EXPORT void ZlibWtSetBufferForLLDecompressSession(
	ZlibWtLLDecompressSessionPtr a_session,
	void* a_bufferForDecompressedData,
	size_t a_sizeForBufferForDecompressedData);

ZLIBANDTLS_LL_EXPORT void ZlibWtSetCallbackForLLDecompressSession(
	ZlibWtLLDecompressSessionPtr a_session,
	ZlibWtTypeLLDecompressCallback a_clbk,
	void* userData);

ZLIBANDTLS_LL_EXPORT void ZlibWtSetupBufferForLLDecompressSession(ZlibWtLLDecompressSessionPtr a_session);

CPPUTILS_END_C



#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIBWT_LL_DECOMPRESS_ROUTINES_H
