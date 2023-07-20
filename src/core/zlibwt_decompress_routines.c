
// zlib_decompress_routines.cpp
// 2018 Feb 11
//
// docs
// https://www.installsetupconfig.com/win32programming/windowsdiskapis2_3.html 
// https://github.com/pbatard/rufus/blob/master/src/drive.c
//

//
// file:			zlibwt_decompress_routines.cpp
// path:			src/core/zlibwt_decompress_routines.cpp
// created on:		2023 Feb 10
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <zlib_with_tools/zlibwt_decompress_routines.h>
#include <zlib_with_tools/zlibwt_ll_decompress_routines.h>
#include <private/zlib_with_tools/zlibwt_decompress_data.h>
#include <cinternal/endian.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>



CPPUTILS_BEGIN_C

CPPUTILS_DLL_PRIVATE void ZlibWtDecompressCallbackStatData_version_0000(const void* a_buffer, size_t a_bufLen, void* a_userData);


static void ZlibWtDecompressCallbackStatHeader(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
    ZlibWtDecompressSessionPtr pSession = (ZlibWtDecompressSessionPtr)a_userData;
    ZlibWtLLDecompressSessionPtr pSessionLL = (ZlibWtLLDecompressSessionPtr)a_userData;

    if (pSessionLL->z_str.total_out >= ZLIBWT_MAIN_HEADER_SIZE) {
        ZlibWtTypeLLDecompressCallback clbk;
        pSession->isInited = 1;
        pSession->hasError = 0;
        pSession->header.used.version = le32toh(pSession->header.used.version);
        pSession->header.used.typeOfCompressedContent = le32toh(pSession->header.used.typeOfCompressedContent);

        switch (pSession->header.used.version) {
        case 0:
            clbk = &ZlibWtDecompressCallbackStatData_version_0000;
            break;
        default:
            //ZLIBWT_THROW "Wrong Version";
            return;
        }

        ZlibWtSetCallbackForLLDecompressSession(pSessionLL, clbk, pSession);
        ZlibWtSetBufferForLLDecompressSession(pSessionLL, pSession->bufferForDecompressedDataTmp, pSession->sizeForBufferForDecompressedDataTmp);
        if(CPPUTILS_UNLIKELY(pSessionLL->z_str.total_out > ZLIBWT_MAIN_HEADER_SIZE)){
            const size_t cunRemaining = CPPUTILS_STATIC_CAST(size_t,pSessionLL->z_str.total_out) - ZLIBWT_MAIN_HEADER_SIZE;
            assert(a_bufLen>cunRemaining);
            (*clbk)(((char*)a_buffer)+(a_bufLen-cunRemaining),cunRemaining,pSession->userData);
        }
        ZlibWtLLDecompressBufferToCallbackReadOldIn(pSessionLL, 0);
    }
}


#ifdef __INTELLISENSE__

typedef void (*ZlibWtTypeLLDecompressCallback)(const void* buffer, size_t bufLen, void* userData);

typedef ZlibWtTypeLLDecompressCallback ZlibWtTypeDecompressFileAndBlobCallback;

typedef ZlibWtTypeLLDecompressCallback ZlibWtTypeDecompressDirFileAndBlobReadCallback;
typedef void (*ZlibWtTypeDecompressDirFileOrDirStartCallback)(const DirIterFileData* a_pFileData, const struct SFileItem* a_pExtraData, void* userData);
typedef void (*ZlibWtTypeDecompressDirFileOrDirEndCallback)(void* userData);


struct SZlibWtDecompressDirCallbacks {
    ZlibWtTypeDecompressFileAndBlobCallback			singleBlobRead;
    ZlibWtTypeDecompressDirFileOrDirStartCallback	dirDirOrFileStart;
    ZlibWtTypeDecompressDirFileAndBlobReadCallback	dirFileRead;
    ZlibWtTypeDecompressDirFileOrDirEndCallback		dirFileEnd;
    ZlibWtTypeDecompressDirFileOrDirEndCallback		dirDirEnd;
    size_t											reserved01[3];
};

#endif


ZLIBANDTLS_EXPORT ZlibWtDecompressSessionPtr ZlibWtCreateDecompressSession(
    const struct SZlibWtDecompressCallbacks* a_clbks,
    void* a_userData,
    void* a_bufferForDecompressedData,
    size_t a_sizeForBufferForDecompressedData)
{
    ZlibWtLLDecompressSessionPtr pSessionLL = ZlibWtCreateLLDecompressSession(
        sizeof(struct ZlibWtDecompressSession), &ZlibWtDecompressCallbackStatHeader, CPPUTILS_NULL,
        CPPUTILS_NULL, 0);
    if (!pSessionLL) { return CPPUTILS_NULL; }
    ZlibWtDecompressSessionPtr pSession  = (ZlibWtDecompressSessionPtr)pSessionLL;
    ZlibWtSetCallbackForLLDecompressSession(pSessionLL, &ZlibWtDecompressCallbackStatHeader, pSession);
    ZlibWtSetBufferForLLDecompressSession(pSessionLL, &(pSession->header), sizeof(struct SCompressDecompressHeader));
    //pSession->clbk = a_clbk;
    memcpy(&(pSession->clbks), a_clbks, sizeof(struct SZlibWtDecompressCallbacks));
    //pSession->clbks.singleBlobRead = a_clbks->singleBlobRead;
    //pSession->clbks.dirFileStart = a_clbks->dirFileStart;
    //pSession->clbks.dirFileRead = a_clbks->dirFileRead;
    //pSession->clbks.dirFileEnd = a_clbks->dirFileEnd;

    pSession->fileData.pFileName = CPPUTILS_NULL;

    pSession->userData = a_userData;
    pSession->bufferForDecompressedDataTmp = (Bytef*)a_bufferForDecompressedData;
    pSession->sizeForBufferForDecompressedDataTmp = (uInt)a_sizeForBufferForDecompressedData;
    pSession->isInited = 0;
    ZlibWtSetupBufferForLLDecompressSession(pSessionLL);
    return pSession;
}


ZLIBANDTLS_EXPORT void ZlibWtDestroyDecompressSession(ZlibWtDecompressSessionPtr a_session)
{
    free((char*)a_session->fileData.pFileName);
    ZlibWtDestroyLLDecompressSession((ZlibWtLLDecompressSessionPtr)a_session);
}


ZLIBANDTLS_EXPORT void ZlibWtSetBufferForDecompressSession(
    ZlibWtDecompressSessionPtr a_session,
    void* a_bufferForDecompressedData,
    size_t a_sizeForBufferForDecompressedData)
{
    if (a_session->isInited) {
        ZlibWtLLDecompressSessionPtr pSession = (ZlibWtLLDecompressSessionPtr)a_session;
        ZlibWtSetBufferForLLDecompressSession(pSession, a_bufferForDecompressedData, a_sizeForBufferForDecompressedData);
    }
    else {
        a_session->bufferForDecompressedDataTmp = (Bytef*)a_bufferForDecompressedData;
        a_session->sizeForBufferForDecompressedDataTmp = (uInt)a_sizeForBufferForDecompressedData;
    }
}


ZLIBANDTLS_EXPORT int ZlibWtDecompressBufferToCallback(
    ZlibWtDecompressSessionPtr a_session,
    int a_flush,
    const void* a_compressedInputData,
    size_t a_sizeForCompressedInputData)
{
    ZlibWtLLDecompressSessionPtr pSessionLL = (ZlibWtLLDecompressSessionPtr)a_session;

    if (a_session->isInited) {
        return ZlibWtLLDecompressBufferToCallback(pSessionLL, 0, a_compressedInputData, a_sizeForCompressedInputData);
    }

    return ZlibWtLLDecompressBufferToCallbackNoTouchOut(pSessionLL, a_flush, a_compressedInputData, a_sizeForCompressedInputData);
}



CPPUTILS_END_C
