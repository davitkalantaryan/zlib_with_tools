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
#include <stddef.h>


CPPUTILS_BEGIN_C

struct ZlibWtCompressSession;
typedef struct ZlibWtCompressSession* ZlibWtCompressSessionPtr;

ZLIBANDTLS_LL_EXPORT ZlibWtCompressSessionPtr ZlibWtCreateCompressSessionRaw(ZlibWtTypeCompressCallback a_clbk,void* userData, int a_compressionLevel); // this will be declared as depricated
ZLIBANDTLS_LL_EXPORT ZlibWtCompressSessionPtr ZlibWtCreateTypedCompressSession(enum TypeOfCompressedContent a_type, ZlibWtTypeCompressCallback a_clbk,void* userData, int a_compressionLevel);
ZLIBANDTLS_LL_EXPORT void ZlibWtDestroyCompressSession(ZlibWtCompressSessionPtr a_session);

ZLIBANDTLS_LL_EXPORT int ZlibWtCompressBufferToCallback(
	ZlibWtCompressSessionPtr a_compressSession, 
	int a_flush,
	const void* a_uncompressedInputData,
	size_t a_sizeForUncompressedInputData,
	void* a_bufferForCompressedData, 
    size_t a_sizeForBufferForCompressedData);

CPPUTILS_END_C



#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIBWT_LL_COMPRESSION_ROUTINES_H
