
// zlib_compression_routines.cpp
// 2017 Feb 12


// http://www.zlib.net/zlib_how.html

#include <zlib_with_tools/zlibwt_compression_routines.h>
#include <zlib_with_tools/zlibwt_ll_compression_routines.h>
#include <zlib_with_tools/stdio_zlibandtls.h>
#include <private/zlib_with_tools/zlibwt_compress_decompress_common.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <zlib_with_tools/stdio_zlibandtls.h>
//#include <zlib.h>
//#include <memory>
//#include <string.h>
//#include <assert.h>
//#include <stdint.h>



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
};

static void ZlibWtFolderCompressCallbackStatic(const void* buffer, size_t bufLen, void* userData);
static int  ZlibWtFolderCompressDirIterCallbackStatic(const char*,void*, const DirIterFileData*);


static inline int DecompressSingleFile(FILE* a_pFile, ZlibWtCompressSessionPtr a_pSession)
{
	int isFileof;
	size_t freadRet;
	char vcBuffer[ZLIBWT_DEF_CHUNK_SIZE];

	do {
		freadRet = fread(vcBuffer, 1, ZLIBWT_DEF_CHUNK_SIZE, a_pFile);
		if (ferror(a_pFile)) {
			return 1;
		}
		ZlibWtCompressBufferToCallback(a_pSession, 0, vcBuffer, freadRet);
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
	char vcBuffer[ZLIBWT_DEF_CHUNK_SIZE];
	ZlibWtCompressSessionPtr pSession = ZlibWtCreateTypedCompressSession(CompressedContentDirectory, a_compressionLevel,
		&ZlibWtFolderCompressCallbackStatic, a_userData, vcBuffer, ZLIBWT_DEF_CHUNK_SIZE);
	nRet = DecompressSingleFile(a_pFile, pSession);
	ZlibWtDestroyCompressSession(pSession);
	return nRet;
}



// if a_filter returns non zero, then file is skipped, if returns DIRITER_EXIT_ALL(=387), then compression stopped
ZLIBANDTLS_EXPORT int ZlibWtCompressDirectoryEx(
	const char* a_directoryPath, 
	int a_compressionLevel,
	ZlibWtTypeCompressCallback a_clbk, TypeDirIterFunc a_filter, 
	void* a_userData)
{
	char vcBuffer[ZLIBWT_DEF_CHUNK_SIZE];
    struct SDirectoryCompressData aUserData;

    aUserData.hasNotAnyFile = 1;
	aUserData.fl.all = 0;
    aUserData.userData = a_userData;
    aUserData.clbk = a_clbk;
    aUserData.filter = a_filter;
	aUserData.pSession = ZlibWtCreateTypedCompressSession(CompressedContentDirectory, a_compressionLevel,
		&ZlibWtFolderCompressCallbackStatic,&aUserData,vcBuffer, ZLIBWT_DEF_CHUNK_SIZE);
    if(!(aUserData.pSession)){return 1;}

	IterateOverDirectoryFilesNoRecurse(a_directoryPath,&ZlibWtFolderCompressDirIterCallbackStatic,&aUserData);
	ZlibWtDestroyCompressSession(aUserData.pSession);
    if(aUserData.hasNotAnyFile){
        // ZLIBWT_MAKE_WARNING("Unable to ");
    }

    return aUserData.fl.b.hasError ? 2 : 0;
}


static void ZlibWtFolderCompressCallbackStatic(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
    struct SDirectoryCompressData* pUserData = CPPUTILS_STATIC_CAST(struct SDirectoryCompressData*,a_userData);
    (*(pUserData->clbk))(a_buffer,a_bufLen,pUserData->userData);
}


static int ZlibWtFolderCompressDirIterCallbackStatic(const char* a_sourceDirectory,void* a_userData, const DirIterFileData* a_pFileData)
{
	static const char svcDummyBuffer[8] = { 0,0,0,0,0,0,0,0 };
	struct stat fStat;
	size_t dummyLen;
	int nFilterResult;
	char  vcStrFilePath[ZLIBWT_MAX_PATH];
	SFileItem aItem;
    struct SDirectoryCompressData* pUserData = CPPUTILS_STATIC_CAST(struct SDirectoryCompressData*, a_userData);

	if (pUserData->fl.all) {
		return DIRITER_EXIT_ALL;
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

	aItem.fileNameLen = CPPUTILS_STATIC_CAST(uint32_t,strlen(a_pFileData->pFileName));
	aItem.fileNameLenNorm = ZLIBWT_NORM_LEN(aItem.fileNameLen);
	
	snprintf_di(vcStrFilePath, ZLIBWT_MAX_PATH_MIN1, "%s/%s", a_sourceDirectory, a_pFileData->pFileName);

	if (a_pFileData->isDir) {
		stat(vcStrFilePath, &fStat);
		aItem.contentType = ZLIBWT_DIR_CONTENT_DIR_START;
		aItem.mode = CPPUTILS_STATIC_CAST(uint64_t, fStat.st_mode);
		ZlibWtCompressBufferToCallback(pUserData->pSession, 0, &aItem, sizeof(SFileItem));
		
		// write file name
		ZlibWtCompressBufferToCallback(pUserData->pSession, 0, a_pFileData->pFileName, CPPUTILS_STATIC_CAST(size_t, aItem.fileNameLen));
		dummyLen = CPPUTILS_STATIC_CAST(size_t, aItem.fileNameLenNorm - aItem.fileNameLen);
		assert(dummyLen < 8);
		ZlibWtCompressBufferToCallback(pUserData->pSession, 0, svcDummyBuffer, dummyLen);

		IterateOverDirectoryFilesNoRecurse(vcStrFilePath, &ZlibWtFolderCompressDirIterCallbackStatic, a_userData);
		aItem.contentType = ZLIBWT_DIR_CONTENT_DIR_END;
		ZlibWtCompressBufferToCallback(pUserData->pSession, 1, &aItem, sizeof(SFileItem));
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
		aItem.mode = CPPUTILS_STATIC_CAST(uint64_t, fStat.st_mode);
		aItem.fileSize = CPPUTILS_STATIC_CAST(uint64_t,fStat.st_size);
		aItem.fileSizeNorm = ZLIBWT_NORM_LEN(aItem.fileSize);
		ZlibWtCompressBufferToCallback(pUserData->pSession, 1, &aItem, sizeof(SFileItem));

		// write file name
		ZlibWtCompressBufferToCallback(pUserData->pSession, 0, a_pFileData->pFileName, CPPUTILS_STATIC_CAST(size_t, aItem.fileNameLen));
		dummyLen = CPPUTILS_STATIC_CAST(size_t, aItem.fileNameLenNorm - aItem.fileNameLen);
		assert(dummyLen < 8);
		ZlibWtCompressBufferToCallback(pUserData->pSession, 0, svcDummyBuffer, dummyLen);

		if (DecompressSingleFile(pFile, pUserData->pSession)) {
			fclose(pFile);
			pUserData->fl.b.hasError = 1;
			return DIRITER_EXIT_ALL;
		}

		dummyLen = CPPUTILS_STATIC_CAST(size_t, aItem.fileSizeNorm - aItem.fileSize);
		assert(dummyLen <8);
		ZlibWtCompressBufferToCallback(pUserData->pSession, 0, svcDummyBuffer, dummyLen);

		fclose(pFile);
	}

    return 0;
}


CPPUTILS_END_C
