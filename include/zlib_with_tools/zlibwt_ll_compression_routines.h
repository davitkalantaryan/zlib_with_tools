//
// file:			zlibwt_ll_compression_routines.h
// path:			src/include/private/zlib_with_tools/zlibwt_ll_compression_routines.h
// created on:		2023 Feb 07
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIBWT_LL_COMPRESSION_ROUTINES_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIBWT_LL_COMPRESSION_ROUTINES_H

#include <zlib_with_tools/export_symbols.h>
#include <zlib_with_tools/zlibwt_compress_decompress_common.h>
#include <zlib_with_tools/zlibwt_compression_data.h>
#include <directory_iterator/directory_iterator.h>


CPPUTILS_BEGIN_C

struct ZlibWtCompressSession;
typedef struct ZlibWtCompressSession* ZlibWtCompressSessionPtr;

ZLIBANDTLS_LL_EXPORT ZlibWtCompressSessionPtr ZlibWtCreateTypedCompressSession(
	enum TypeOfCompressedContent a_type,
	int a_compressionLevel,
	ZlibWtTypeCompressCallback a_clbk,
	void* userData,
	void* a_bufferForCompressedData,
	size_t a_sizeForBufferForCompressedData);

ZLIBANDTLS_LL_EXPORT void ZlibWtDestroyCompressSession(ZlibWtCompressSessionPtr a_session);

ZLIBANDTLS_LL_EXPORT int ZlibWtCompressBufferToCallback(
	ZlibWtCompressSessionPtr a_compressSession, 
	int a_flush,
	const void* a_uncompressedInputData,
	size_t a_sizeForUncompressedInputData);

ZLIBANDTLS_LL_EXPORT void ZlibWtSetBufferForCompressSession(
	ZlibWtCompressSessionPtr a_compressSession,
	void* a_bufferForCompressedData,
	size_t a_sizeForBufferForCompressedData);

ZLIBANDTLS_LL_EXPORT void ZlibWtSetCallbackForCompressSession(
	ZlibWtCompressSessionPtr a_compressSession,
	ZlibWtTypeCompressCallback a_clbk,
	void* userData);

CPPUTILS_END_C



#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIBWT_LL_COMPRESSION_ROUTINES_H
