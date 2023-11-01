//
// file:			zlibwt_compression_routines.cpp
// path:			src/core/zlibwt_compression_routines.cpp
// created on:		2023 Feb 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//
// http://www.zlib.net/zlib_how.html
//

#include <zlib_with_tools/zlibwt_compression_routines.h>
#include <zlib_with_tools/zlibwt_ll_compression_routines.h>
#include <zlib_with_tools/utils/stdio_zlibandtls.h>
#include <private/zlib_with_tools/zlibwt_compress_decompress_common.h>
#include <cinternal/endian.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


CPPUTILS_BEGIN_C


struct SDirectoryCompressData{
    uint32_t                    hasNotAnyFile;
	union {
		uint32_t all;
		struct {
			uint32_t            hasError : 1;
			uint32_t            shouldExit : 1;
			uint32_t			reserved01 : 30;
		}b;
	}fl;
    void*                       userData;
    ZlibWtTypeCompressCallback  clbk;
    TypeDirIterFunc             filter;
	ZlibWtCompressSessionPtr    pSession;
	char*						pcBufferIn;
	size_t						reserved01;
};

typedef int (*TypeCompressSingleFile)(FILE*, ZlibWtCompressSessionPtr, char* a_pcBuffer, size_t);

static void ZlibWtFolderCompressCallbackStatic(const void* buffer, size_t bufLen, void* userData);
static int  ZlibWtFolderCompressDirIterCallbackStatic(const char*,void*, const DirIterFileData*);
static int CompressSingleFileFromFile(FILE* a_pFile, ZlibWtCompressSessionPtr a_pSession, char* a_pcBuffer, size_t a_bufferSize);
static int CompressSingleFileFromBuffer(FILE* a_pFile, ZlibWtCompressSessionPtr a_pSession, char* a_pcBuffer, size_t a_bufferSize);


static const char s_vcDummyBuffer[9] = { 0,0,0,0,0,0,0,0,0 };

static inline int CompressArbitraryBufferAsFileInline(
	struct SFileItem* CPPUTILS_ARG_NN a_pItem,
	struct SDirectoryCompressData* CPPUTILS_ARG_NN a_pUserData,
	const char* CPPUTILS_ARG_NN a_fileName,
	FILE* CPPUTILS_ARG_NN a_pFile, void* a_pBuffer,
	TypeCompressSingleFile CPPUTILS_ARG_NN a_compressFnc,
	uint32_t a_fileNameLen, uint32_t a_fileNameLenNorm, int a_mode, size_t a_size)
{
	size_t dummyLen;
	uint64_t fileSize, fileSizeNorm;
	a_pItem->contentType = ZLIBWT_DIR_CONTENT_FILE;
	a_pItem->mode = htole32(CPPUTILS_STATIC_CAST(uint64_t, a_mode));
	fileSize = CPPUTILS_STATIC_CAST(uint64_t, a_size);
	fileSizeNorm = ZLIBWT_NORM_LEN(fileSize);
	a_pItem->fileSize = htole64(fileSize);
	a_pItem->fileSizeNorm = htole64(fileSizeNorm);
	ZlibWtCompressBufferToCallback(a_pUserData->pSession, 1, a_pItem, sizeof(struct SFileItem));

	// write file name
	ZlibWtCompressBufferToCallback(a_pUserData->pSession, 0, a_fileName, CPPUTILS_STATIC_CAST(size_t, a_fileNameLen));
	dummyLen = CPPUTILS_STATIC_CAST(size_t, a_fileNameLenNorm - a_fileNameLen);
	assert(dummyLen < 9);
	ZlibWtCompressBufferToCallback(a_pUserData->pSession, 0, s_vcDummyBuffer, dummyLen);

	if ((*a_compressFnc)(a_pFile, a_pUserData->pSession, a_pBuffer, ZLIBWT_DEF_CHUNK_SIZE)) {
		a_pUserData->fl.b.hasError = 1;
		return DIRITER_EXIT_ALL;
	}

	dummyLen = CPPUTILS_STATIC_CAST(size_t, fileSizeNorm - fileSize);
	assert(dummyLen < 9);
	ZlibWtCompressBufferToCallback(a_pUserData->pSession, 0, s_vcDummyBuffer, dummyLen);
	return 0;
}


static inline int ZlibWtFolderCompressBufferAsDirRootFileInline(struct SDirectoryCompressData* CPPUTILS_ARG_NN a_pUserData, const struct SZlibWtExtraCompressionBuffer* CPPUTILS_ARG_NN a_extraBuffer){
	int nFileCompressReturn;
	uint32_t fileNameLen, fileNameLenNorm;
	struct SFileItem aItem;

	fileNameLen = CPPUTILS_STATIC_CAST(uint32_t, strlen(a_extraBuffer->ffileName));
	fileNameLenNorm = ZLIBWT_NORM_LEN(fileNameLen);

	aItem.fileNameLen = htole32(fileNameLen);
	aItem.fileNameLenNorm = htole32(fileNameLenNorm);

	nFileCompressReturn = CompressArbitraryBufferAsFileInline(
		&aItem, a_pUserData, a_extraBuffer->ffileName, CPPUTILS_NULL, CPPUTILS_CONST_CAST(char*, a_extraBuffer->buffer),
		&CompressSingleFileFromBuffer, fileNameLen, fileNameLenNorm, 0, a_extraBuffer->bufferSize);
	return nFileCompressReturn;
}


ZLIBANDTLS_EXPORT int ZlibWtCompressFileEx(
	FILE* a_pFile,
	int a_compressionLevel,
	ZlibWtTypeCompressCallback a_clbk,
	void* a_userData)
{
	int nRet;
	char* pcBufferOut;
	char* pcBufferIn = (char*)malloc(ZLIBWT_DEF_CHUNK_SIZE);
	if (!pcBufferIn) { return 1; }
	pcBufferOut = (char*)malloc(ZLIBWT_DEF_CHUNK_SIZE);
	if (!pcBufferOut) { free(pcBufferIn);  return 1; }
	ZlibWtCompressSessionPtr pSession = ZlibWtCreateTypedCompressSession(CompressedContentFile, a_compressionLevel,
		a_clbk, a_userData, pcBufferOut, ZLIBWT_DEF_CHUNK_SIZE);
	nRet = CompressSingleFileFromFile(a_pFile, pSession, pcBufferIn, ZLIBWT_DEF_CHUNK_SIZE);
	ZlibWtDestroyCompressSession(pSession);
	free(pcBufferOut);
	free(pcBufferIn);
	return nRet;
}


// if a_filter returns non zero, then file is skipped, if returns DIRITER_EXIT_ALL(=387), then compression stopped
ZLIBANDTLS_EXPORT int ZlibWtCompressDirectoryWithBufferEx(
	const char* a_directoryPath,
	size_t a_extraBuffersCount, const struct SZlibWtExtraCompressionBuffer* CPPUTILS_ARG_NN a_extraBuffers,
	int a_compressionLevel,
	ZlibWtTypeCompressCallback a_clbk, TypeDirIterFunc a_filter,
	void* a_userData)
{
	size_t i;
	struct SDirectoryCompressData aUserData;
	char* pcBufferOut = (char*)malloc(ZLIBWT_DEF_CHUNK_SIZE);
	if (!pcBufferOut) { return 1; }

	aUserData.pcBufferIn = (char*)malloc(ZLIBWT_DEF_CHUNK_SIZE);
	if (!(aUserData.pcBufferIn)) { free(pcBufferOut); return 1; }

	aUserData.hasNotAnyFile = 1;
	aUserData.fl.all = 0;
	aUserData.userData = a_userData;
	aUserData.clbk = a_clbk;
	aUserData.filter = a_filter;
	aUserData.pSession = ZlibWtCreateTypedCompressSession(CompressedContentDirectory, a_compressionLevel,
		&ZlibWtFolderCompressCallbackStatic, &aUserData, pcBufferOut, ZLIBWT_DEF_CHUNK_SIZE);
	if (!(aUserData.pSession)) { return 1; }

	for (i = 0; i < a_extraBuffersCount; ++i) {
		if (ZlibWtFolderCompressBufferAsDirRootFileInline(&aUserData, &a_extraBuffers[i])) {
			ZlibWtDestroyCompressSession(aUserData.pSession);
			free(aUserData.pcBufferIn);
			free(pcBufferOut);
			return 2;
		}
	}

	IterateOverDirectoryFilesNoRecurse(a_directoryPath, &ZlibWtFolderCompressDirIterCallbackStatic, &aUserData);
	ZlibWtDestroyCompressSession(aUserData.pSession);
	if (aUserData.hasNotAnyFile) {
		// ZLIBWT_MAKE_WARNING("Unable to ");
	}

	free(aUserData.pcBufferIn);
	free(pcBufferOut);
	return aUserData.fl.b.hasError ? 2 : 0;
}



// if a_filter returns non zero, then file is skipped, if returns DIRITER_EXIT_ALL(=387), then compression stopped
ZLIBANDTLS_EXPORT int ZlibWtCompressDirectoryEx(
	const char* a_directoryPath, 
	int a_compressionLevel,
	ZlibWtTypeCompressCallback a_clbk, TypeDirIterFunc a_filter, 
	void* a_userData)
{
	struct SDirectoryCompressData aUserData;
	char* pcBufferOut = (char*)malloc(ZLIBWT_DEF_CHUNK_SIZE);
	if (!pcBufferOut) { return 1; }

	aUserData.pcBufferIn = (char*)malloc(ZLIBWT_DEF_CHUNK_SIZE);
	if (!(aUserData.pcBufferIn)) { free(pcBufferOut); return 1; }

    aUserData.hasNotAnyFile = 1;
	aUserData.fl.all = 0;
    aUserData.userData = a_userData;
    aUserData.clbk = a_clbk;
    aUserData.filter = a_filter;
	aUserData.pSession = ZlibWtCreateTypedCompressSession(CompressedContentDirectory, a_compressionLevel,
		&ZlibWtFolderCompressCallbackStatic,&aUserData, pcBufferOut, ZLIBWT_DEF_CHUNK_SIZE);
    if(!(aUserData.pSession)){return 1;}

	IterateOverDirectoryFilesNoRecurse(a_directoryPath,&ZlibWtFolderCompressDirIterCallbackStatic,&aUserData);
	ZlibWtDestroyCompressSession(aUserData.pSession);
    if(aUserData.hasNotAnyFile){
        // ZLIBWT_MAKE_WARNING("Unable to ");
    }

	free(aUserData.pcBufferIn);
	free(pcBufferOut);
    return aUserData.fl.b.hasError ? 2 : 0;
}


static int CompressSingleFileFromFile(FILE* a_pFile, ZlibWtCompressSessionPtr a_pSession, char* a_pcBuffer, size_t a_bufferSize)
{
	int isFileof;
	size_t freadRet;
	int nIndex = 0;

	do {
		freadRet = fread(a_pcBuffer, 1, a_bufferSize, a_pFile);
		if (ferror(a_pFile)) {
			return 1;
		}
		ZlibWtCompressBufferToCallback(a_pSession, ((++nIndex) & 0x10) >> 4, a_pcBuffer, freadRet);
		isFileof = feof(a_pFile);
	} while (!isFileof);

	return 0;
}


static int CompressSingleFileFromBuffer(FILE* a_pFile, ZlibWtCompressSessionPtr a_pSession, char* a_pcBuffer, size_t a_bufferSize)
{
	ZlibWtCompressBufferToCallback(a_pSession, (int)((size_t)a_pFile), a_pcBuffer, a_bufferSize);
	return 0;
}


static void ZlibWtFolderCompressCallbackStatic(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
    struct SDirectoryCompressData* pUserData = CPPUTILS_STATIC_CAST(struct SDirectoryCompressData*,a_userData);
    (*(pUserData->clbk))(a_buffer,a_bufLen,pUserData->userData);
}


static int ZlibWtFolderCompressDirIterCallbackStatic(const char* a_sourceDirectory,void* a_userData, const DirIterFileData* a_pFileData)
{
	struct stat fStat;
	size_t dummyLen;
	uint32_t fileNameLen, fileNameLenNorm;
	int nFilterResult;
	char  vcStrFilePath[ZLIBWT_MAX_PATH];
	struct SFileItem aItem;
    struct SDirectoryCompressData* const pUserData = CPPUTILS_STATIC_CAST(struct SDirectoryCompressData*, a_userData);

	if (pUserData->fl.all) {
		return DIRITER_EXIT_ALL;
	}

	if (a_pFileData->pFileName[0] == '.') {
		if ((a_pFileData->pFileName[1] == 0) || ((a_pFileData->pFileName[1] == '.') && (a_pFileData->pFileName[2] == 0))) { return 0; }
	}

	nFilterResult = (*(pUserData->filter))(a_sourceDirectory,pUserData->userData,a_pFileData);
    if(nFilterResult){
        // we skip this file, in case if return is equal to `DIRITER_EXIT_ALL` (=387) iteration will be stopped at all
		if (nFilterResult == DIRITER_EXIT_ALL) {
			pUserData->fl.b.shouldExit = 1;
            return DIRITER_EXIT_ALL;
		}
        return 0;
    }

    pUserData->hasNotAnyFile = 0;

	fileNameLen = CPPUTILS_STATIC_CAST(uint32_t,strlen(a_pFileData->pFileName));
	fileNameLenNorm = ZLIBWT_NORM_LEN(fileNameLen);

	aItem.fileNameLen = htole32(fileNameLen);
	aItem.fileNameLenNorm = htole32(fileNameLenNorm);
	
	snprintf_di(vcStrFilePath, ZLIBWT_MAX_PATH_MIN1, "%s/%s", a_sourceDirectory, a_pFileData->pFileName);

	if (a_pFileData->isDir) {
		stat(vcStrFilePath, &fStat);
		aItem.contentType = ZLIBWT_DIR_CONTENT_DIR_START;
		aItem.mode = htole32(CPPUTILS_STATIC_CAST(uint32_t, fStat.st_mode));
		ZlibWtCompressBufferToCallback(pUserData->pSession, 0, &aItem, sizeof(struct SFileItem));
		
		// write file name
		ZlibWtCompressBufferToCallback(pUserData->pSession, 0, a_pFileData->pFileName, CPPUTILS_STATIC_CAST(size_t, fileNameLen));
		dummyLen = CPPUTILS_STATIC_CAST(size_t, fileNameLenNorm - fileNameLen);
		assert(dummyLen < 9);
		ZlibWtCompressBufferToCallback(pUserData->pSession, 0, s_vcDummyBuffer, dummyLen);

		IterateOverDirectoryFilesNoRecurse(vcStrFilePath, &ZlibWtFolderCompressDirIterCallbackStatic, a_userData);
		aItem.contentType = ZLIBWT_DIR_CONTENT_DIR_END;
		ZlibWtCompressBufferToCallback(pUserData->pSession, 1, &aItem, sizeof(struct SFileItem));
	}
	else {
		int nFileCompressReturn;
		FILE* pFile = fopen_zlibandtls(vcStrFilePath, "rb");
		if(!pFile){
			pUserData->fl.b.hasError = 1;
			return DIRITER_EXIT_ALL;
		}
		if (fstat(fileno_zlibandtls(pFile), &fStat)) {
			fclose(pFile);
			pUserData->fl.b.hasError = 1;
			return DIRITER_EXIT_ALL;
		}

		nFileCompressReturn = CompressArbitraryBufferAsFileInline(
			&aItem, pUserData, a_pFileData->pFileName, pFile, pUserData->pcBufferIn,
			&CompressSingleFileFromFile, fileNameLen, fileNameLenNorm, (int)fStat.st_mode, (size_t)fStat.st_size);
		fclose(pFile);
		return nFileCompressReturn;
	}

    return 0;
}


CPPUTILS_END_C
