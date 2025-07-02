//
// file:			zlibwt_hl_decompress_routines.cpp
// path:			src/core/zlibwt_hl_decompress_routines.cpp
// created on:		2023 Feb 14
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <zlib_with_tools/zlibwt_decompress_routines.h>
#include <zlib_with_tools/utils/stdio_zlibandtls.h>
#include <zlib_with_tools/utils/io_zlibandtls.h>
#include <zlib_with_tools/utils/string_zlibandtls.h>
#include <zlib_with_tools/utils/memory_zlibandtls.h>
#include <private/zlib_with_tools/zlibwt_decompress_data.h>
#include <cinternal/disable_compiler_warnings.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <cinternal/undisable_compiler_warnings.h>


CPPUTILS_BEGIN_C

#ifndef ZLIBWT_ERROR_REPORT
#define ZLIBWT_ERROR_REPORT(...) do{}while(0)
#endif


static void DecompressFileStartCallback(void* a_userData);
static void DecompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData);
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////*/
static void DecompressDirStartCallback(void* a_userData);
static void DecompressDirFileOrDirStartCallback(const DirIterFileData* a_pFileData, const struct SFileItem* a_pExtraData, void* a_userData);
static void DecompressDirFileReadCallback(const void* a_buffer, size_t a_bufLen, void* a_userData);
static void DecompressDirFileEndCallback(void* a_userData);
static void DecompressDirDirEndCallback(void* a_userData);


static inline size_t NextSizeToReadInline(size_t a_archiveSize, size_t a_fReadRetTotal) CPPUTILS_NOEXCEPT{
    if (a_archiveSize > 0) {
        if (a_archiveSize > a_fReadRetTotal) {
            const size_t unRemainingBytes = a_archiveSize - a_fReadRetTotal;
            return (unRemainingBytes < ZLIBWT_DEF_CHUNK_SIZE) ? unRemainingBytes : ZLIBWT_DEF_CHUNK_SIZE;
        }  //  if (a_archiveSize > a_fReadRetTotal) {
        else {
            return 0;
        }
    }  //  if (a_archiveSize > 0) {
    return ZLIBWT_DEF_CHUNK_SIZE;
}


struct CPPUTILS_DLL_PRIVATE SDecompressData {
	FILE*							fpFileOut;
	const char*						cpcFileOrFolderNameOut;
	char*							directoryPath;
	size_t							directoryPathLen;
	uint32_t						hasError;
	uint32_t						reserved01;
	int								fd;
	int								reserved02;
	enum TypeOfCompressedContent	retValue;
	enum TypeOfCompressedContent	reserved03;
};


ZLIBANDTLS_EXPORT enum TypeOfCompressedContent ZlibWtDecompressFileOrDirEx(
	FILE* a_fpInpCompressedFile,
	const char* a_cpcOutDecompressedFileOrDir,
    size_t a_archiveSize)
{
	ZlibWtDecompressSessionPtr pSession;
	char* pcBufferIn;
	int isFileof;
	int nIndex = 0;
	size_t fReadRet, unSizeToRead;
    size_t fReadRetTotal=0;

	const struct SZlibWtDecompressCallbacks clbks = {
			{
				&DecompressFileStartCallback,
				&DecompressFileAndBlobCallback,
				{0,0}
			},
			{
				&DecompressDirStartCallback,
				&DecompressDirFileOrDirStartCallback,
				&DecompressDirFileReadCallback,
				&DecompressDirFileEndCallback,
				&DecompressDirDirEndCallback,
				{0,0,0}
			}
	};
	struct SDecompressData aData = {
		CPPUTILS_NULL,
		a_cpcOutDecompressedFileOrDir,
		CPPUTILS_NULL,
		0,
		0,
		0,
		-1,
		0,
		CompressedContentNone,
		CompressedContentNone
	};

	char* pcBufferOut = (char*)malloc(ZLIBWT_DEF_CHUNK_SIZE);
	if (!pcBufferOut) {
		ZLIBWT_ERROR_REPORT("Unable create buffer");
		return CompressedContentNone;
	}

	pcBufferIn = (char*)malloc(ZLIBWT_DEF_CHUNK_SIZE);
	if (!pcBufferIn) {
		free(pcBufferOut);
		ZLIBWT_ERROR_REPORT("Unable create buffer");
		return CompressedContentNone;
	}

	pSession = ZlibWtCreateDecompressSession(&clbks, &aData, pcBufferOut, ZLIBWT_DEF_CHUNK_SIZE);
	if (!pSession) {
		free(pcBufferIn);
		free(pcBufferOut);
		ZLIBWT_ERROR_REPORT("Unable to create decompress session \n");
		return CompressedContentNone;
	}

	do {
        unSizeToRead = NextSizeToReadInline(a_archiveSize, fReadRetTotal);
        if (unSizeToRead < 1) {
            break;
        }
		fReadRet = fread(pcBufferIn, 1, unSizeToRead, a_fpInpCompressedFile);
		if (ferror(a_fpInpCompressedFile)) {
			if (aData.fpFileOut) { fclose(aData.fpFileOut); }
			ZlibWtDestroyDecompressSession(pSession);
			free(pcBufferIn);
			free(pcBufferOut);
			return CompressedContentNone;
		}
        fReadRetTotal += fReadRet;
		ZlibWtDecompressBufferToCallback(pSession, ((++nIndex) & 0x10) >> 4, pcBufferIn, fReadRet);
		isFileof = feof(a_fpInpCompressedFile);
	} while ((!isFileof) && (!aData.hasError));


	// cleanup
	if (aData.fpFileOut) { fclose(aData.fpFileOut); }
	ZlibWtDestroyDecompressSession(pSession);
	free(pcBufferIn);
	free(pcBufferOut);

	return aData.retValue;
}


ZLIBANDTLS_EXPORT enum TypeOfCompressedContent ZlibWtDecompressFileOrDir(
    const char* a_cpcInputCompressedFile,
    const char* a_cpcOutDecompressedFileOrDir,
    size_t a_archiveSize)
{
	enum TypeOfCompressedContent retVal;
	FILE* fpFileIn = fopen_zlibandtls(a_cpcInputCompressedFile, "rb");
	if (!fpFileIn) {
		ZLIBWT_ERROR_REPORT("Unable to open the file with the name \" %s\"\n", a_cpcInputCompressedFile);
		return CompressedContentNone;
	}

	retVal = ZlibWtDecompressFileOrDirEx(fpFileIn, a_cpcOutDecompressedFileOrDir, a_archiveSize);
	fclose(fpFileIn);
	return retVal;
}



static void DecompressFileStartCallback(void* a_userData)
{
	struct SDecompressData* pData = (struct SDecompressData*)a_userData;
	pData->fpFileOut = fopen_zlibandtls(pData->cpcFileOrFolderNameOut, "wb");
	if (!pData->fpFileOut) {
		pData->hasError = 1;
		return;
	}
	pData->retValue = CompressedContentFile;
}


static void DecompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	struct SDecompressData* pData = (struct SDecompressData*)a_userData;
	fwrite(a_buffer, 1, a_bufLen, pData->fpFileOut);
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static void DecompressDirStartCallback(void* a_userData)
{
	struct SDecompressData* pData = (struct SDecompressData*)a_userData;
	int nReturn = mkdir_zlibandtls(pData->cpcFileOrFolderNameOut, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (nReturn) {
		pData->hasError = 1;
		return;
	}

	pData->directoryPath = strdup_zlibandtls(pData->cpcFileOrFolderNameOut);
	if (!pData->directoryPath) {
		pData->hasError = 1;
		return;
	}

	pData->directoryPathLen = strlen(pData->cpcFileOrFolderNameOut);
	pData->retValue = CompressedContentDirectory;
}

static void DecompressDirFileOrDirStartCallback(const DirIterFileData* a_pFileData, const struct SFileItem* a_pExtraData, void* a_userData)
{
	struct SDecompressData* pData = (struct SDecompressData*)a_userData;

	if (a_pFileData->isDir) {
		const size_t newStrLen = pData->directoryPathLen + 1 + ((size_t)a_pExtraData->fileNameLen);
		char* directoryPathTmp = (char*)realloc(pData->directoryPath, newStrLen + 1);
		if (!directoryPathTmp) {
			pData->hasError = 1;
			return;
		}
		pData->directoryPath = directoryPathTmp;
		pData->directoryPath[pData->directoryPathLen] = '/';
		memcpy(pData->directoryPath + pData->directoryPathLen + 1, a_pFileData->pFileName, (size_t)a_pExtraData->fileNameLen);
		pData->directoryPath[newStrLen] = 0;
		pData->directoryPathLen = newStrLen;
		int nReturn = mkdir_zlibandtls(pData->directoryPath, a_pExtraData->mode);
		if (nReturn) {
			pData->hasError = 1;
			return;
		}
	}
	else {
		char* pcFileNameBuffer = (char*)alloca_zlibandtls(pData->directoryPathLen + 4 + ((size_t)a_pExtraData->fileNameLen));
		memcpy(pcFileNameBuffer, pData->directoryPath, pData->directoryPathLen);
		pcFileNameBuffer[pData->directoryPathLen] = '/';
		memcpy(pcFileNameBuffer + pData->directoryPathLen + 1, a_pFileData->pFileName, (size_t)a_pExtraData->fileNameLen);
		pcFileNameBuffer[pData->directoryPathLen+1+ ((size_t)a_pExtraData->fileNameLen)] = 0;
		sopen_zlibandtls(&(pData->fd), pcFileNameBuffer, ZLIBWT_O_WRONLY | ZLIBWT_O_CREAT | ZLIBWT_O_BINARY, mode_fo_zlibandtls(a_pExtraData->mode));
		if (pData->fd < 0) {
			pData->hasError = 1;
			return;
		}
	}
}


static void DecompressDirFileReadCallback(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	struct SDecompressData* pData = (struct SDecompressData*)a_userData;
    size_t unRet = (size_t)write_zlibandtls(pData->fd, a_buffer, a_bufLen);
    (void)unRet;
}


static void DecompressDirFileEndCallback(void* a_userData)
{
	struct SDecompressData* pData = (struct SDecompressData*)a_userData;
	close_zlibandtls(pData->fd);
}


static void DecompressDirDirEndCallback(void* a_userData)
{
	struct SDecompressData* pData = (struct SDecompressData*)a_userData;
	size_t newStrLen;
	char* cpcTerm = strrchr(pData->directoryPath, '/');
	assert(cpcTerm);
	newStrLen = (size_t)(cpcTerm - (pData->directoryPath));
	*cpcTerm = 0;
	pData->directoryPathLen = newStrLen;
}



CPPUTILS_END_C
