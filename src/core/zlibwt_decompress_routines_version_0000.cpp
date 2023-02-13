
// zlib_decompress_routines.cpp
// 2018 Feb 11

//
// file:			zlibwt_decompress_routines_version_0000.cpp
// path:			src/core/zlibwt_decompress_routines_version_0000.cpp
// created on:		2023 Feb 10
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <zlib_with_tools/zlibwt_decompress_routines.h>
#include <zlib_with_tools/zlibwt_ll_decompress_routines.h>
#include <private/zlib_with_tools/zlibwt_decompress_data.h>

CPPUTILS_BEGIN_C

static void ZlibWtDecompressCallbackStatData_dir(const void* a_buffer, size_t a_bufLen, void* a_userData);
static void ZlibWtDecompressCallbackStatData_blob_and_file(const void* a_buffer, size_t a_bufLen, void* a_userData);

#ifdef __INTELLISENSE__

// size=40 B, padding=8 B
struct ZLIBANDTLS_LL_EXPORT SFileItem {
	uint64_t fileSize;
	uint64_t fileSizeNorm;
	uint32_t fileNameLen;
	uint32_t fileNameLenNorm;
	uint32_t mode;
	uint8_t  contentType;
	uint8_t  reserved01[3];
	uint8_t  reserved02[8];
};

typedef struct SDirIterFileData {
	uint32_t	deepness;  // in case of non recursive call this field is not used
	uint32_t	isDir : 1;
	uint32_t	reserved01 : 31;
	const char* pFileName;
	const void* pSystemData;
}DirIterFileData;

#endif

CPPUTILS_DLL_PRIVATE void ZlibWtDecompressCallbackStatData_version_0000(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	ZlibWtTypeLLDecompressCallback clbk;
	ZlibWtDecompressSessionPtr pSession = (ZlibWtDecompressSessionPtr)a_userData;
	ZlibWtLLDecompressSessionPtr pSessionLL = (ZlibWtLLDecompressSessionPtr)a_userData;

	switch (pSession->header.used.typeOfCompressedContent) {
	case CompressedContentDirectory:
		clbk = &ZlibWtDecompressCallbackStatData_dir;
		break;
	case CompressedContentFile: case CompressedContentBlob:
		pSession->fileItem.fileSize = 0;
		pSession->fileItem.fileSizeNorm = 0;
		pSession->fileItem.fileNameLen = 0;
		pSession->fileItem.fileNameLenNorm = 0;
		pSession->fileItem.mode = 0;
		pSession->fileItem.contentType = ZLIBWT_DIR_CONTENT_SINGLE_BLOB;
		pSession->fileData.deepness = 0;
		pSession->fileData.isDir = 0;
		pSession->fileData.pFileName = "";
		pSession->fileData.pSystemData = CPPUTILS_NULL;
		clbk = &ZlibWtDecompressCallbackStatData_blob_and_file;
		break;
	default:
		return;
	}  //  switch (pSession->header.used.typeOfCompressedContent) {

	ZlibWtSetCallbackForLLDecompressSession(pSessionLL, clbk, pSession);
	(*clbk)(a_buffer, a_bufLen, a_userData);
}


static void ZlibWtDecompressCallbackStatData_dir(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	ZlibWtDecompressSessionPtr pSession = (ZlibWtDecompressSessionPtr)a_userData;
}


static void ZlibWtDecompressCallbackStatData_blob_and_file(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	// typedef void (*ZlibWtTypeDecompressCallback)(const void* buffer, size_t bufLen, void* userData, const DirIterFileData* a_pFileData, const struct SFileItem* a_pExtraData);
	ZlibWtDecompressSessionPtr pSession = (ZlibWtDecompressSessionPtr)a_userData;
	(*(pSession->clbks.singleBlobRead))(a_buffer, a_bufLen, pSession->userData);
}


CPPUTILS_END_C
