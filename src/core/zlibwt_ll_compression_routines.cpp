//
// file:			zlibwt_ll_compression_routines.cpp
// path:			src/core/zlibwt_ll_compression_routines.cpp
// created on:		2023 Feb 08
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <zlib_with_tools/zlibwt_ll_compression_routines.h>
#include <private/zlib_with_tools/zlibwt_compress_decompress_common.h>
#include <zlib.h>
#include <stdlib.h>


CPPUTILS_BEGIN_C


struct CPPUTILS_DLL_PRIVATE ZlibWtCompressSession {
    z_stream                    z_str;
    ZlibWtTypeCompressCallback  clbk;
    void*                       userData;
    Bytef*                      bufferForCompressedData;
    uInt                        sizeForBufferForCompressedData;
    uInt                        reserved01;
};


static inline ZlibWtCompressSessionPtr ZlibWtCreateCompressSessionRaw(
    int a_compressionLevel,
    ZlibWtTypeCompressCallback a_clbk,
    void* a_userData,
    void* a_bufferForCompressedData,
    size_t a_sizeForBufferForCompressedData)
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
    pSession->clbk = a_clbk;
    ZlibWtSetBufferForCompressSession(pSession, a_bufferForCompressedData, a_sizeForBufferForCompressedData);

    return pSession;
}


ZLIBANDTLS_LL_EXPORT ZlibWtCompressSessionPtr ZlibWtCreateTypedCompressSession(
    enum TypeOfCompressedContent a_type,
    int a_compressionLevel,
    ZlibWtTypeCompressCallback a_clbk,
    void* a_userData,
    void* a_bufferForCompressedData,
    size_t a_sizeForBufferForCompressedData)
{
    int retZlib;
    ZlibWtCompressSessionPtr pSession;
    SCompressDecompressHeader aHeader;

    pSession = ZlibWtCreateCompressSessionRaw(a_compressionLevel,a_clbk,a_userData, a_bufferForCompressedData, a_sizeForBufferForCompressedData);
    if(!pSession){return CPPUTILS_NULL;}

    aHeader.version = ZLIBWT_VERSION;
    aHeader.typeOfCompressedContent = CPPUTILS_STATIC_CAST(uint32_t,a_type);
    CPPUTILS_STATIC_CAST(void,aHeader.vReserved);

    retZlib = ZlibWtCompressBufferToCallback(pSession,0,&aHeader,sizeof(SCompressDecompressHeader));
    if(!retZlib){
        ZlibWtDestroyCompressSession(pSession);
        return CPPUTILS_NULL;
    }

    return pSession;
}


ZLIBANDTLS_LL_EXPORT void ZlibWtDestroyCompressSession(ZlibWtCompressSessionPtr a_compressSession)
{
    CPPUTILS_STATIC_CAST(void,deflateEnd(&(a_compressSession->z_str)));
    free(a_compressSession);
}


ZLIBANDTLS_LL_EXPORT int ZlibWtCompressBufferToCallback(
    ZlibWtCompressSessionPtr a_compressSession,
    int a_flush,
    const void* a_uncompressedInputData,
    size_t a_sizeForUncompressedInputData)
{
    int retZlib;

    a_compressSession->z_str.next_in = CPPUTILS_STATIC_CAST(const Bytef*,a_uncompressedInputData);
    a_compressSession->z_str.avail_in = CPPUTILS_STATIC_CAST(uInt,a_sizeForUncompressedInputData);

    a_compressSession->z_str.avail_out = a_compressSession->sizeForBufferForCompressedData;
    a_compressSession->z_str.next_out = a_compressSession->bufferForCompressedData;
    retZlib = deflate(&(a_compressSession->z_str), a_flush);
    if(retZlib==Z_STREAM_ERROR){return Z_STREAM_ERROR;}
    (*(a_compressSession->clbk))(a_compressSession->bufferForCompressedData,
        CPPUTILS_STATIC_CAST(size_t, a_compressSession->sizeForBufferForCompressedData-a_compressSession->z_str.avail_out),a_compressSession->userData);

    while((a_compressSession->z_str.avail_in>0)){
        a_compressSession->z_str.avail_out = a_compressSession->sizeForBufferForCompressedData;
        a_compressSession->z_str.next_out = a_compressSession->bufferForCompressedData;
        retZlib = deflate(&(a_compressSession->z_str), 0);
        if(retZlib==Z_STREAM_ERROR){return Z_STREAM_ERROR;}
        (*(a_compressSession->clbk))(a_compressSession->bufferForCompressedData,
            CPPUTILS_STATIC_CAST(size_t, a_compressSession->sizeForBufferForCompressedData - a_compressSession->z_str.avail_out), a_compressSession->userData);
    }

    return 0;
}


ZLIBANDTLS_LL_EXPORT void ZlibWtSetBufferForCompressSession(
    ZlibWtCompressSessionPtr a_compressSession,
    void* a_bufferForCompressedData,
    size_t a_sizeForBufferForCompressedData)
{
    a_compressSession->bufferForCompressedData = CPPUTILS_STATIC_CAST(Bytef*, a_bufferForCompressedData);
    a_compressSession->sizeForBufferForCompressedData = CPPUTILS_STATIC_CAST(uInt, a_sizeForBufferForCompressedData);
}


CPPUTILS_END_C
