
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
#include <cpputils/endian.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

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



struct CPPUTILS_DLL_PRIVATE ZlibWtDecompressSession {
    struct ZlibWtLLDecompressSession        ll;
    struct SCompressDecompressHeader        header;
    struct SFileItem                        fileItem;
    struct SDirIterFileData                 fileData;
    struct SZlibWtDecompressDirCallbacks    clbks;
    void*                                   userData;
    Bytef*                                  bufferForDecompressedDataTmp;

    char*                                   pcFileName;
    size_t									stage;  // 0 - reading header, 1 - reading file name, 2 - reding file
    size_t									offsetInTheSection;
    size_t                                  sizeOfSection;

    size_t                                  reserved01;
    size_t                                  reserved02;
    uInt                                    sizeForBufferForDecompressedDataTmp;
    uInt                                    isInited : 1;
    uInt                                    reserved02 : 31;
};


#endif

CPPUTILS_DLL_PRIVATE void ZlibWtDecompressCallbackStatData_version_0000(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	ZlibWtTypeLLDecompressCallback clbk;
	ZlibWtDecompressSessionPtr pSession = (ZlibWtDecompressSessionPtr)a_userData;
	ZlibWtLLDecompressSessionPtr pSessionLL = (ZlibWtLLDecompressSessionPtr)a_userData;

	pSession->fileItem.fileSize = 0;
	pSession->fileItem.fileSizeNorm = 0;
	pSession->fileItem.fileNameLen = 0;
	pSession->fileItem.fileNameLenNorm = 0;
	pSession->fileItem.mode = 0;
	pSession->fileItem.contentType = ZLIBWT_DIR_CONTENT_SINGLE_BLOB;
	pSession->fileData.deepness = 0;
	pSession->fileData.isDir = 0;
	pSession->fileData.pSystemData = CPPUTILS_NULL;

	switch (pSession->header.used.typeOfCompressedContent) {
	case CompressedContentDirectory:
		pSession->stage = ZLIBWT_DECOMPR_STAGE_HEADER;
		pSession->offsetInTheSection = 0;
		pSession->sizeOfSection = sizeof(struct SFileItem);
		clbk = &ZlibWtDecompressCallbackStatData_dir;
		break;
	case CompressedContentFile: case CompressedContentBlob:
		clbk = &ZlibWtDecompressCallbackStatData_blob_and_file;
		break;
	default:
		return;
	}  //  switch (pSession->header.used.typeOfCompressedContent) {

	ZlibWtSetCallbackForLLDecompressSession(pSessionLL, clbk, pSession);
	(*clbk)(a_buffer, a_bufLen, a_userData);
}


static inline void FileHeaderClosingStat(ZlibWtDecompressSessionPtr a_pSession)
{
	char* pcFileName;
	a_pSession->fileItem.fileSize = le64toh(a_pSession->fileItem.fileSize);
	a_pSession->fileItem.fileSizeNorm = le64toh(a_pSession->fileItem.fileSizeNorm);
	a_pSession->fileItem.fileNameLen = le32toh(a_pSession->fileItem.fileNameLen);
	a_pSession->fileItem.fileNameLenNorm = le32toh(a_pSession->fileItem.fileNameLenNorm);
	a_pSession->fileItem.mode = le32toh(a_pSession->fileItem.mode);
	a_pSession->stage = ZLIBWT_DECOMPR_STAGE_FILENAME;
	pcFileName = (char*)realloc((char*)a_pSession->fileData.pFileName, (size_t)a_pSession->fileItem.fileNameLenNorm);
	if (!pcFileName) {
		a_pSession->hasError = 1;
	}
	a_pSession->fileData.pFileName = pcFileName;
	a_pSession->sizeOfSection = (size_t)a_pSession->fileItem.fileNameLenNorm;
}


static void ZlibWtDecompressCallbackStatData_dir(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	ZlibWtDecompressSessionPtr pSession = (ZlibWtDecompressSessionPtr)a_userData;
	const size_t cunRemnBuffSize = pSession->sizeOfSection - pSession->offsetInTheSection;
	//const size_t cunRemnBuffSize = pSession->sizeOfSection - pSession->offsetInTheSection;

	if (pSession->hasError) { return; }

	switch (pSession->stage) {
	case ZLIBWT_DECOMPR_STAGE_HEADER: {
		if (a_bufLen >= cunRemnBuffSize) {
			memcpy(((char*)&(pSession->fileItem)) + pSession->offsetInTheSection, a_buffer, cunRemnBuffSize);
			switch (pSession->fileItem.contentType) {
			case ZLIBWT_DIR_CONTENT_DIR_START:
				FileHeaderClosingStat(pSession);
				pSession->fileData.isDir = 1;
				//(*(pSession->clbks.dirFileStart))(&(pSession->fileData), &(pSession->fileItem), pSession->userData);
				break;
			case ZLIBWT_DIR_CONTENT_FILE:
				FileHeaderClosingStat(pSession);
				pSession->fileData.isDir = 0;
				break;
			case ZLIBWT_DIR_CONTENT_DIR_END:
				pSession->fileData.isDir = 1;
				(*(pSession->clbks.dirDirEnd))(pSession->userData);
				break;
			default:
				break;
			}  //  switch (pSession->fileItem.contentType) {

			pSession->offsetInTheSection = 0;
			if (a_bufLen > cunRemnBuffSize) {
				ZlibWtDecompressCallbackStatData_dir(((char*)a_buffer) + cunRemnBuffSize, a_bufLen - cunRemnBuffSize, a_userData);
			}
		} // if (a_bufLen >= cunRemnBuffSize) {
		else {
			memcpy(((char*)&(pSession->fileItem)) + pSession->offsetInTheSection, a_buffer, a_bufLen);
			pSession->offsetInTheSection += a_bufLen;
		}
	}break;
	case ZLIBWT_DECOMPR_STAGE_FILENAME: {
		if (a_bufLen >= cunRemnBuffSize) {
			memcpy(((char*)pSession->fileData.pFileName) + pSession->offsetInTheSection, a_buffer, cunRemnBuffSize);
			switch (pSession->fileItem.contentType) {
			case ZLIBWT_DIR_CONTENT_FILE:
				pSession->stage = ZLIBWT_DECOMPR_STAGE_READFILE;
				pSession->sizeOfSection = (size_t)pSession->fileItem.fileSize;
				break;
			case ZLIBWT_DIR_CONTENT_DIR_START:
				pSession->stage = ZLIBWT_DECOMPR_STAGE_HEADER;
				pSession->sizeOfSection = sizeof(struct SFileItem);
				break;
			default:
				pSession->hasError = 1;
				return;
			}  //  switch (pSession->fileItem.contentType) {
			pSession->offsetInTheSection = 0;
			(*(pSession->clbks.dirDirOrFileStart))(&(pSession->fileData),&(pSession->fileItem),pSession->userData);
			if (a_bufLen > cunRemnBuffSize) {
				ZlibWtDecompressCallbackStatData_dir(((char*)a_buffer) + cunRemnBuffSize, a_bufLen - cunRemnBuffSize, a_userData);
			}
		}  //  if (a_bufLen >= cunRemnBuffSize) {
		else {
			memcpy(((char*)pSession->fileData.pFileName) + pSession->offsetInTheSection, a_buffer, a_bufLen);
			pSession->offsetInTheSection += a_bufLen;
		}
	}break;
	case ZLIBWT_DECOMPR_STAGE_READFILE: {
		if (a_bufLen >= cunRemnBuffSize) {
			(*(pSession->clbks.dirFileRead))(a_buffer, cunRemnBuffSize, pSession->userData);
			assert(pSession->fileItem.contentType== ZLIBWT_DIR_CONTENT_FILE);
			pSession->stage = ZLIBWT_DECOMPR_STAGE_FILE_GAP;
			pSession->sizeOfSection = (size_t)(pSession->fileItem.fileSizeNorm - pSession->fileItem.fileSize);
			pSession->offsetInTheSection = 0;
			(*(pSession->clbks.dirFileEnd))(pSession->userData);
			if (a_bufLen > cunRemnBuffSize) {
				ZlibWtDecompressCallbackStatData_dir(((char*)a_buffer) + cunRemnBuffSize, a_bufLen - cunRemnBuffSize, a_userData);
			}
		}  //  if (a_bufLen >= cunRemnBuffSize) {
		else {
			(*(pSession->clbks.dirFileRead))(a_buffer, a_bufLen, pSession->userData);
			pSession->offsetInTheSection += a_bufLen;
		}
	}break;
	case ZLIBWT_DECOMPR_STAGE_FILE_GAP: {
		if (a_bufLen >= cunRemnBuffSize) {
			assert(pSession->fileItem.contentType == ZLIBWT_DIR_CONTENT_FILE);
			pSession->stage = ZLIBWT_DECOMPR_STAGE_HEADER;
			pSession->sizeOfSection = sizeof(struct SFileItem);
			pSession->offsetInTheSection = 0;
			if (a_bufLen > cunRemnBuffSize) {
				ZlibWtDecompressCallbackStatData_dir(((char*)a_buffer) + cunRemnBuffSize, a_bufLen - cunRemnBuffSize, a_userData);
			}
		}  //  if (a_bufLen >= cunRemnBuffSize) {
		else {
			pSession->offsetInTheSection += a_bufLen;
		}
	}break;
	default:
		break;
	}  //  switch (1) {
}


static void ZlibWtDecompressCallbackStatData_blob_and_file(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	// typedef void (*ZlibWtTypeDecompressCallback)(const void* buffer, size_t bufLen, void* userData, const DirIterFileData* a_pFileData, const struct SFileItem* a_pExtraData);
	ZlibWtDecompressSessionPtr pSession = (ZlibWtDecompressSessionPtr)a_userData;
	(*(pSession->clbks.singleBlobRead))(a_buffer, a_bufLen, pSession->userData);
}


CPPUTILS_END_C
