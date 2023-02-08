//
// file:			zlibwt_ll_compression_routines.cpp
// path:			src/core/zlibwt_ll_compression_routines.cpp
// created on:		2023 Feb 08
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <zlib_with_tools/zlibwt_ll_compression_routines.h>
#include <zlib.h>


CPPUTILS_BEGIN_C

struct CPPUTILS_DLL_PRIVATE ZlibWtCompressSession {
	z_stream	z_str;
};


ZLIBANDTLS_LL_EXPORT ZlibWtCompressSessionPtr ZlibWtCreateCompressSession(void)
{
	struct ZlibWtCompressSession* pSession = CPPU
}
ZLIBANDTLS_LL_EXPORT void ZlibWtDestroyCompressSession(ZlibWtCompressSessionPtr a_session);

ZLIBANDTLS_EXPORT int ZlibWtCompressBufferToCallback(
	ZlibWtCompressSessionPtr a_compressSession,
	int a_flush,
	const void* a_uncompressedInputData,
	size_t a_sizeForUncompressedInputData,
	void* a_bufferForCompressedData,
	size_t a_sizeForBufferForCompressedData,
	ZlibWtTypeCompressCallback a_clbk,
	void* a_userData);

CPPUTILS_END_C
