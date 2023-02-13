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
#include <zlib_with_tools/stdio_zlibandtls.h>
#include <private/zlib_with_tools/zlibwt_compress_decompress_common.h>
#include <cpputils/endian.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


CPPUTILS_BEGIN_C


struct CPPUTILS_DLL_PRIVATE SDirectoryCompressData{
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

static void ZlibWtFolderCompressCallbackStatic(const void* buffer, size_t bufLen, void* userData);
static int  ZlibWtFolderCompressDirIterCallbackStatic(const char*,void*, const DirIterFileData*);


static inline int CompressSingleFile(FILE* a_pFile, ZlibWtCompressSessionPtr a_pSession, char* a_pcBuffer, size_t a_bufferSize)
{
	int isFileof;
	size_t freadRet;
	int nIndex = 0;

	do {
		freadRet = fread(a_pcBuffer, 1, a_bufferSize, a_pFile);
		if (ferror(a_pFile)) {
			return 1;
		}
		ZlibWtCompressBufferToCallback(a_pSession, ((++nIndex)&0x10)>>4, a_pcBuffer, freadRet);
		isFileof = feof(a_pFile);
	} while (!isFileof);

	return 0;
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
	nRet = CompressSingleFile(a_pFile, pSession, pcBufferIn, ZLIBWT_DEF_CHUNK_SIZE);
	ZlibWtDestroyCompressSession(pSession);
	free(pcBufferOut);
	free(pcBufferIn);
	return nRet;
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


static void ZlibWtFolderCompressCallbackStatic(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
    struct SDirectoryCompressData* pUserData = CPPUTILS_STATIC_CAST(struct SDirectoryCompressData*,a_userData);
    (*(pUserData->clbk))(a_buffer,a_bufLen,pUserData->userData);
}


static int ZlibWtFolderCompressDirIterCallbackStatic(const char* a_sourceDirectory,void* a_userData, const DirIterFileData* a_pFileData)
{
	static const char svcDummyBuffer[9] = { 0,0,0,0,0,0,0,0,0 };
	struct stat fStat;
	size_t dummyLen;
	uint32_t fileNameLen, fileNameLenNorm;
	uint64_t fileSize, fileSizeNorm;
	int nFilterResult;
	char  vcStrFilePath[ZLIBWT_MAX_PATH];
	struct SFileItem aItem;
    struct SDirectoryCompressData* pUserData = CPPUTILS_STATIC_CAST(struct SDirectoryCompressData*, a_userData);

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
		}
        return nFilterResult;
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
		ZlibWtCompressBufferToCallback(pUserData->pSession, 0, svcDummyBuffer, dummyLen);

		IterateOverDirectoryFilesNoRecurse(vcStrFilePath, &ZlibWtFolderCompressDirIterCallbackStatic, a_userData);
		aItem.contentType = ZLIBWT_DIR_CONTENT_DIR_END;
		ZlibWtCompressBufferToCallback(pUserData->pSession, 1, &aItem, sizeof(struct SFileItem));
	}
	else {
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

		aItem.contentType = ZLIBWT_DIR_CONTENT_FILE;
		aItem.mode = htole32(CPPUTILS_STATIC_CAST(uint64_t, fStat.st_mode));
		fileSize = CPPUTILS_STATIC_CAST(uint64_t,fStat.st_size);
		fileSizeNorm = ZLIBWT_NORM_LEN(fileSize);
		aItem.fileSize = htole64(fileSize);
		aItem.fileSizeNorm = htole64(fileSizeNorm);
		ZlibWtCompressBufferToCallback(pUserData->pSession, 1, &aItem, sizeof(struct SFileItem));

		// write file name
		ZlibWtCompressBufferToCallback(pUserData->pSession, 0, a_pFileData->pFileName, CPPUTILS_STATIC_CAST(size_t, fileNameLen));
		dummyLen = CPPUTILS_STATIC_CAST(size_t, fileNameLenNorm - fileNameLen);
		assert(dummyLen < 9);
		ZlibWtCompressBufferToCallback(pUserData->pSession, 0, svcDummyBuffer, dummyLen);

		if (CompressSingleFile(pFile, pUserData->pSession, pUserData->pcBufferIn, ZLIBWT_DEF_CHUNK_SIZE)) {
			fclose(pFile);
			pUserData->fl.b.hasError = 1;
			return DIRITER_EXIT_ALL;
		}

		dummyLen = CPPUTILS_STATIC_CAST(size_t, fileSizeNorm - fileSize);
		assert(dummyLen <9);
		ZlibWtCompressBufferToCallback(pUserData->pSession, 0, svcDummyBuffer, dummyLen);

		fclose(pFile);
	}

    return 0;
}


CPPUTILS_END_C
