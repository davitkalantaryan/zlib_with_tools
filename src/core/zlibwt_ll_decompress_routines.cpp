//
// file:			zlibwt_ll_compression_routines.cpp
// path:			src/core/zlibwt_ll_compression_routines.cpp
// created on:		2023 Feb 08
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <zlib_with_tools/zlibwt_ll_decompress_routines.h>
#include <private/zlib_with_tools/zlibwt_decompress_data.h>
#include <stdlib.h>


CPPUTILS_BEGIN_C


ZLIBANDTLS_LL_EXPORT ZlibWtLLDecompressSessionPtr ZlibWtCreateLLDecompressSession(
    size_t a_size, // in case if a_size<sizeof(ZlibWtLLDecompressSession), this will be replaced by sizeof(ZlibWtLLDecompressSession)
    ZlibWtTypeLLDecompressCallback a_clbk,
    void* a_userData,
    void* a_bufferForDecompressedData,
    size_t a_sizeForBufferForDecompressedData)
{
    int nReturn;
    const size_t cunSize = (a_size < sizeof(struct ZlibWtLLDecompressSession)) ? sizeof(struct ZlibWtLLDecompressSession) : a_size;
    ZlibWtLLDecompressSessionPtr pSession = CPPUTILS_STATIC_CAST(ZlibWtLLDecompressSessionPtr, malloc(cunSize));
    if (!pSession) { return CPPUTILS_NULL; }

    pSession->z_str.zalloc = Z_NULL;
    pSession->z_str.zfree = Z_NULL;
    pSession->z_str.opaque = Z_NULL;
    pSession->z_str.avail_in = 0;
    pSession->z_str.next_in = Z_NULL;
    pSession->z_str.total_out = 0;
    nReturn = inflateInit(&(pSession->z_str));
    if (nReturn != Z_OK) { return CPPUTILS_NULL; }

    ZlibWtSetCallbackForLLDecompressSession(pSession,a_clbk, a_userData);
    ZlibWtSetBufferForLLDecompressSession(pSession, a_bufferForDecompressedData, a_sizeForBufferForDecompressedData);

    return pSession;
}


ZLIBANDTLS_LL_EXPORT void ZlibWtDestroyLLDecompressSession(ZlibWtLLDecompressSessionPtr a_session)
{
    CPPUTILS_STATIC_CAST(void,inflate(&(a_session->z_str), Z_FULL_FLUSH)); // this returns error, but doesn't matter
    CPPUTILS_STATIC_CAST(void,inflate(&(a_session->z_str), Z_FINISH));  // this returns error, but doesn't matter
    CPPUTILS_STATIC_CAST(void,inflateEnd(&(a_session->z_str)));
    free(a_session);
}


ZLIBANDTLS_LL_EXPORT void ZlibWtSetBufferForLLDecompressSession(
    ZlibWtLLDecompressSessionPtr a_session,
    void* a_bufferForDecompressedData,
    size_t a_sizeForBufferForDecompressedData)
{
    a_session->bufferForDecompressedData = CPPUTILS_STATIC_CAST(Bytef*, a_bufferForDecompressedData);
    a_session->sizeForBufferForDecompressedData = CPPUTILS_STATIC_CAST(uInt, a_sizeForBufferForDecompressedData);
}


ZLIBANDTLS_LL_EXPORT void ZlibWtSetCallbackForLLDecompressSession(
    ZlibWtLLDecompressSessionPtr a_session,
    ZlibWtTypeLLDecompressCallback a_clbk,
    void* a_userData)
{
    a_session->userData = a_userData;
    a_session->clbk = a_clbk;
}


ZLIBANDTLS_LL_EXPORT void ZlibWtSetupBufferForLLDecompressSession(ZlibWtLLDecompressSessionPtr a_session)
{
    a_session->z_str.avail_out = CPPUTILS_STATIC_CAST(uInt, a_session->sizeForBufferForDecompressedData);
    a_session->z_str.next_out = CPPUTILS_STATIC_CAST(Bytef*, a_session->bufferForDecompressedData);
}


ZLIBANDTLS_LL_EXPORT int ZlibWtLLDecompressBufferToCallback(
    ZlibWtLLDecompressSessionPtr a_session,
    int a_flush,
    const void* a_compressedInputData,
    size_t a_sizeForCompressedInputData)
{
    int retZlib;

    a_session->z_str.next_in = CPPUTILS_STATIC_CAST(const Bytef*, a_compressedInputData);
    a_session->z_str.avail_in = CPPUTILS_STATIC_CAST(uInt, a_sizeForCompressedInputData);

    while (a_session->z_str.avail_in > 0) {
        a_session->z_str.avail_out = CPPUTILS_STATIC_CAST(uInt, a_session->sizeForBufferForDecompressedData);
        a_session->z_str.next_out = CPPUTILS_STATIC_CAST(Bytef*, a_session->bufferForDecompressedData);
        retZlib = inflate(&(a_session->z_str), a_flush);
        if (retZlib != Z_OK) { return retZlib; }
        (*(a_session->clbk))(a_session->bufferForDecompressedData,
            CPPUTILS_STATIC_CAST(size_t, a_session->sizeForBufferForDecompressedData - a_session->z_str.avail_out), a_session->userData);
        a_flush = Z_NO_FLUSH;
    }

    return 0;
}


ZLIBANDTLS_LL_EXPORT int ZlibWtLLDecompressBufferToCallbackNoTouchOut(
    ZlibWtLLDecompressSessionPtr a_session,
    int a_flush,
    const void* a_compressedInputData,
    size_t a_sizeForCompressedInputData)
{
    int retZlib;

    a_session->z_str.next_in = CPPUTILS_STATIC_CAST(const Bytef*, a_compressedInputData);
    a_session->z_str.avail_in = CPPUTILS_STATIC_CAST(uInt, a_sizeForCompressedInputData);

    while ((a_session->z_str.avail_in > 0)&&(a_session->z_str.avail_out>0)) {
        retZlib = inflate(&(a_session->z_str), a_flush);
        if (retZlib != Z_OK) { return retZlib; }
        (*(a_session->clbk))(a_session->bufferForDecompressedData,
            CPPUTILS_STATIC_CAST(size_t, a_session->sizeForBufferForDecompressedData - a_session->z_str.avail_out), a_session->userData);
        a_flush = Z_NO_FLUSH;
    }

    return 0;
}


ZLIBANDTLS_LL_EXPORT int ZlibWtLLDecompressBufferToCallbackReadOldIn(ZlibWtLLDecompressSessionPtr a_session, int a_flush)
{
    int retZlib;

    while (a_session->z_str.avail_in > 0) {
        a_session->z_str.avail_out = CPPUTILS_STATIC_CAST(uInt, a_session->sizeForBufferForDecompressedData);
        a_session->z_str.next_out = CPPUTILS_STATIC_CAST(Bytef*, a_session->bufferForDecompressedData);
        retZlib = inflate(&(a_session->z_str), a_flush);
        if (retZlib != Z_OK) { return retZlib; }
        (*(a_session->clbk))(a_session->bufferForDecompressedData,
            CPPUTILS_STATIC_CAST(size_t, a_session->sizeForBufferForDecompressedData - a_session->z_str.avail_out), a_session->userData);
        a_flush = Z_NO_FLUSH;
    }

    return 0;
}



CPPUTILS_END_C
