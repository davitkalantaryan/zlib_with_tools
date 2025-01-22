//
// file:			zlibwt_ll_compression_routines.cpp
// path:			src/core/zlibwt_ll_compression_routines.cpp
// created on:		2023 Feb 08
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <zlib_with_tools/zlibwt_ll_compression_routines.h>
#include <private/zlib_with_tools/zlibwt_compress_decompress_common.h>
#include <zlib_with_tools/zlibwt_zlib_h_wrapper.h>
#include <cinternal/endian.h>
#include <cinternal/disable_compiler_warnings.h>
#include <stdlib.h>
#include <cinternal/undisable_compiler_warnings.h>


CPPUTILS_BEGIN_C


struct CPPUTILS_DLL_PRIVATE ZlibWtCompressSession {
    z_stream                    z_str;
    ZlibWtTypeCompressCallback  clbk;
    void*                       userData;
    Bytef*                      bufferForCompressedData;
    uInt                        sizeForBufferForCompressedData;
    uInt                        reserved01;
};



ZLIBANDTLS_LL_EXPORT ZlibWtCompressSessionPtr ZlibWtCreateTypedCompressSession(
    enum TypeOfCompressedContent a_type,
    int a_compressionLevel,
    ZlibWtTypeCompressCallback a_clbk,
    void* a_userData,
    void* a_bufferForCompressedData,
    size_t a_sizeForBufferForCompressedData)
{
    int nReturn;
    struct SCompressDecompressHeader aHeader;
    ZlibWtCompressSessionPtr pSession = CPPUTILS_STATIC_CAST(ZlibWtCompressSessionPtr, malloc(sizeof(struct ZlibWtCompressSession)));
    if (!pSession) { return CPPUTILS_NULL; }

    pSession->z_str.zalloc = Z_NULL;
    pSession->z_str.zfree = Z_NULL;
    pSession->z_str.opaque = Z_NULL;
    nReturn = deflateInit(&(pSession->z_str), a_compressionLevel);
    if (nReturn != Z_OK) {
        free(pSession);
        return CPPUTILS_NULL;
    }

    ZlibWtSetCallbackForCompressSession(pSession, a_clbk, a_userData);
    ZlibWtSetBufferForCompressSession(pSession, a_bufferForCompressedData, a_sizeForBufferForCompressedData);

    aHeader.used.version = htole32(ZLIBWT_VERSION);
    aHeader.used.typeOfCompressedContent = htole32(CPPUTILS_STATIC_CAST(uint32_t, a_type));

    nReturn = ZlibWtCompressBufferToCallback(pSession, 0, &aHeader, sizeof(struct SCompressDecompressHeader));
    if (nReturn) {
        ZlibWtDestroyCompressSession(pSession);
        return CPPUTILS_NULL;
    }

    return pSession;
}


ZLIBANDTLS_LL_EXPORT void ZlibWtDestroyCompressSession(ZlibWtCompressSessionPtr a_session)
{
    // todo:
    //CPPUTILS_STATIC_CAST(void, deflate(&(a_session->z_str), 1));
    //CPPUTILS_STATIC_CAST(void,deflate(&(a_session->z_str), Z_FULL_FLUSH));
    //CPPUTILS_STATIC_CAST(void,deflate(&(a_session->z_str), Z_FINISH));
    CPPUTILS_STATIC_CAST(void,deflateEnd(&(a_session->z_str)));
    free(a_session);
}


ZLIBANDTLS_LL_EXPORT int ZlibWtCompressBufferToCallback(
    ZlibWtCompressSessionPtr a_session,
    int a_flush,
    const void* a_uncompressedInputData,
    size_t a_sizeForUncompressedInputData)
{
    int retZlib;

    // todo:
    (void)a_flush;

    a_session->z_str.next_in = CPPUTILS_STATIC_CAST(const Bytef*,a_uncompressedInputData);
    a_session->z_str.avail_in = CPPUTILS_STATIC_CAST(uInt,a_sizeForUncompressedInputData);

    while(a_session->z_str.avail_in>0){
        a_session->z_str.avail_out = a_session->sizeForBufferForCompressedData;
        a_session->z_str.next_out = a_session->bufferForCompressedData;
        // todo:
        //retZlib = deflate(&(a_session->z_str), a_flush);
        //a_flush = Z_NO_FLUSH;
        retZlib = deflate(&(a_session->z_str), 1);
        if(retZlib==Z_STREAM_ERROR){return Z_STREAM_ERROR;}
        (*(a_session->clbk))(a_session->bufferForCompressedData,
            CPPUTILS_STATIC_CAST(size_t, a_session->sizeForBufferForCompressedData - a_session->z_str.avail_out), a_session->userData);
    }

    return 0;
}


ZLIBANDTLS_LL_EXPORT void ZlibWtSetBufferForCompressSession(
    ZlibWtCompressSessionPtr a_session,
    void* a_bufferForCompressedData,
    size_t a_sizeForBufferForCompressedData)
{
    a_session->bufferForCompressedData = CPPUTILS_STATIC_CAST(Bytef*, a_bufferForCompressedData);
    a_session->sizeForBufferForCompressedData = CPPUTILS_STATIC_CAST(uInt, a_sizeForBufferForCompressedData);
}


ZLIBANDTLS_LL_EXPORT void ZlibWtSetCallbackForCompressSession(
    ZlibWtCompressSessionPtr a_session,
    ZlibWtTypeCompressCallback a_clbk,
    void* a_userData)
{
    a_session->userData = a_userData;
    a_session->clbk = a_clbk;
}


CPPUTILS_END_C
