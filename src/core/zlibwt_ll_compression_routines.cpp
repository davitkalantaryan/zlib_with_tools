//
// file:			zlibwt_ll_compression_routines.cpp
// path:			src/core/zlibwt_ll_compression_routines.cpp
// created on:		2023 Feb 08
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <zlib_with_tools/zlibwt_ll_compression_routines.h>
#include <zlib.h>
#include <stdlib.h>


CPPUTILS_BEGIN_C

struct CPPUTILS_DLL_PRIVATE ZlibWtCompressSession {
	z_stream	z_str;
    void*       userData;
    size_t      reserved01;
};


ZLIBANDTLS_LL_EXPORT ZlibWtCompressSessionPtr ZlibWtCreateCompressSession(void* a_userData, int a_compressionLevel)
{
    int nReturn;
    struct ZlibWtCompressSession* pSession = CPPUTILS_STATIC_CAST(struct ZlibWtCompressSession*,malloc(sizeof(struct ZlibWtCompressSession)));
    if(!pSession){return CPPUTILS_NULL;}

    pSession->z_str.zalloc = Z_NULL;
    pSession->z_str.zfree = Z_NULL;
    pSession->z_str.opaque = Z_NULL;
    nReturn = deflateInit(&(pSession->z_str), a_compressionLevel);
    if (nReturn != Z_OK){
        free(pSession);
        return CPPUTILS_NULL;
    }

    pSession->userData = a_userData;
    CPPUTILS_STATIC_CAST(void,pSession->reserved01);

    return pSession;
}


ZLIBANDTLS_LL_EXPORT void ZlibWtDestroyCompressSession(ZlibWtCompressSessionPtr a_compressSession)
{
    CPPUTILS_STATIC_CAST(void,deflateEnd(&(a_compressSession->z_str)));
    free(a_compressSession);
}


ZLIBANDTLS_EXPORT int ZlibWtCompressBufferToCallback(
	ZlibWtCompressSessionPtr a_compressSession,
	int a_flush,
	const void* a_uncompressedInputData,
	size_t a_sizeForUncompressedInputData,
	void* a_bufferForCompressedData,
	size_t a_sizeForBufferForCompressedData,
    ZlibWtTypeCompressCallback a_clbk)
{
    int retZlib;
    //char* pcOutToSend = CPPUTILS_STATIC_CAST(char*,a_bufferForCompressedData);
    a_compressSession->z_str.next_in = CPPUTILS_STATIC_CAST(const Bytef*,a_uncompressedInputData);
    a_compressSession->z_str.avail_in = CPPUTILS_STATIC_CAST(uInt,a_sizeForUncompressedInputData);

    a_compressSession->z_str.avail_out = CPPUTILS_STATIC_CAST(uInt,a_sizeForBufferForCompressedData);
    a_compressSession->z_str.next_out = CPPUTILS_STATIC_CAST(Bytef*,a_bufferForCompressedData);
    retZlib = deflate(&(a_compressSession->z_str), a_flush);
    if(retZlib==Z_STREAM_ERROR){return Z_STREAM_ERROR;}
    (*a_clbk)(a_bufferForCompressedData,a_sizeForBufferForCompressedData-CPPUTILS_STATIC_CAST(size_t,a_compressSession->z_str.avail_out),a_compressSession->userData);

    while((a_compressSession->z_str.avail_in>0)){
        a_compressSession->z_str.avail_out = CPPUTILS_STATIC_CAST(uInt,a_sizeForBufferForCompressedData);
        a_compressSession->z_str.next_out = CPPUTILS_STATIC_CAST(Bytef*,a_bufferForCompressedData);
        retZlib = deflate(&(a_compressSession->z_str), 0);
        if(retZlib==Z_STREAM_ERROR){return Z_STREAM_ERROR;}
        (*a_clbk)(a_bufferForCompressedData,a_sizeForBufferForCompressedData-CPPUTILS_STATIC_CAST(size_t,a_compressSession->z_str.avail_out),a_compressSession->userData);
    }

    return 0;
}


CPPUTILS_END_C
