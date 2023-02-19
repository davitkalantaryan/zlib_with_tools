
// http://www.zlib.net/zlib_how.html


#include <zlib_with_tools/zlibwt_compression_routines.h>
#include <zlib_with_tools/zlibwt_decompress_routines.h>
#include <zlib_with_tools/utils/stdio_zlibandtls.h>
#include <zlib_with_tools/utils/io_zlibandtls.h>
#include <zlib_with_tools/utils/string_zlibandtls.h>
#include <zlib_with_tools/zlibwt_zlib_h_wrapper.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef _MSC_VER
#pragma comment (lib,"zlib.lib")
#endif

static void CompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData) CPPUTILS_NOEXCEPT;
static int  DirCompressFilterFunction(const char*, void*, const DirIterFileData*) CPPUTILS_NOEXCEPT;
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////*/
static void DecompressFileStartCallback(void* a_userData);
static void DecompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData);
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////*/
static void DecompressDirStartCallback(void* a_userData);
static void DecompressDirFileOrDirStartCallback(const DirIterFileData* a_pFileData, const struct SFileItem* a_pExtraData, void* a_userData);
static void DecompressDirFileReadCallback(const void* a_buffer, size_t a_bufLen, void* a_userData);
static void DecompressDirFileEndCallback(void* a_userData);
static void DecompressDirDirEndCallback(void* a_userData);

struct SDecompressData {
	FILE*		fpFileOut;
	const char* cpcFileOrFolderNameOut;
	char*		directoryPath;
	size_t		directoryPathLen;
	size_t		hasError : 1;
	size_t		reserved01 : (sizeof(size_t) - 1);
	int			fd;
	int			reserved02;
};

// a -> compress file
// b -> decompress
// c -> compress directory

int main(int a_argc, char* a_argv[])
{
	if (a_argc < 3) {
		fprintf(stderr,"Function and file should be provided\n");
		return 1;
	}

	int nReturn = 1;
	const char* cpcFileNameIn = a_argv[2];
	const char* cpcFileNameOut = "out.zlib";

	if (a_argc > 3) {
		cpcFileNameOut = a_argv[3];
	}


	switch (a_argv[1][0]) {
	case 'a': {
		FILE* fpFileIn = fopen_zlibandtls(cpcFileNameIn, "rb");
		if (!fpFileIn) {
			fprintf(stderr, "Unable to open the file with the name \"%s\"\n", cpcFileNameIn);
			return 1;
		}
		FILE* fpFileOut = fopen_zlibandtls(cpcFileNameOut, "wb");
		if (!fpFileOut) {
			fclose(fpFileIn);
			fprintf(stderr, "Unable to open the file with the name \"%s\"\n", cpcFileNameOut);
			return 1;
		}
		nReturn = ZlibWtCompressFileEx(fpFileIn, Z_BEST_COMPRESSION, &CompressFileAndBlobCallback, fpFileOut);
		fclose(fpFileOut);
		fclose(fpFileIn);
	}break;
	case 'b': {
		FILE* fpFileIn = fopen_zlibandtls(cpcFileNameIn, "rb");
		if (!fpFileIn) {
			fprintf(stderr, "Unable to open the file with the name \"%s\"\n", cpcFileNameIn);
			return 1;
		}

		SDecompressData aData{nullptr,cpcFileNameOut,nullptr,0,0,0,-1,0};

		char vcBufferOut[4096];
		const struct SZlibWtDecompressCallbacks clbks = {
			{
				& DecompressFileStartCallback,
				& DecompressFileAndBlobCallback,
				{0,0}
			},
			{
				& DecompressDirStartCallback,
				& DecompressDirFileOrDirStartCallback,
				& DecompressDirFileReadCallback,
				& DecompressDirFileEndCallback,
				& DecompressDirDirEndCallback,
				{0,0,0}
			}
		};
		ZlibWtDecompressSessionPtr pSession = ZlibWtCreateDecompressSession(&clbks, &aData, vcBufferOut, 4096);
		if (!pSession) {
			if (aData.fpFileOut) { fclose(aData.fpFileOut); };
			fclose(fpFileIn);
			fprintf(stderr, "Unable to create decompress session \n");
			return 1;
		}

		char vcBufferIn[4096];
		int isFileof;
		int nIndex = 0;
		size_t freadRet;

		do {
			freadRet = fread(vcBufferIn, 1, 4096, fpFileIn);
			if (ferror(fpFileIn)) {
				ZlibWtDestroyDecompressSession(pSession);
				if (aData.fpFileOut) { fclose(aData.fpFileOut); };
				fclose(fpFileIn);
				return 1;
			}
			ZlibWtDecompressBufferToCallback(pSession, ((++nIndex) & 0x10) >> 4, vcBufferIn, freadRet);
			isFileof = feof(fpFileIn);
		} while ((!isFileof)&&(!aData.hasError));

		ZlibWtDestroyDecompressSession(pSession);
		if (aData.fpFileOut) { fclose(aData.fpFileOut); }
		free(aData.directoryPath);
		fclose(fpFileIn);
	}break;
	case 'c': {
		FILE* fpFileOut = fopen_zlibandtls(cpcFileNameOut, "wb");
		if (!fpFileOut) {
			fprintf(stderr, "Unable to open the file with the name \"%s\"\n", cpcFileNameOut);
			return 1;
		}
		nReturn = ZlibWtCompressDirectoryEx(cpcFileNameIn, Z_BEST_COMPRESSION, &CompressFileAndBlobCallback, &DirCompressFilterFunction,fpFileOut);
		fclose(fpFileOut);
	}break;
	default:
		fprintf(stderr, "Wrong function is provided\n");
		return 1;
	}

    return nReturn;
}


static void CompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData) CPPUTILS_NOEXCEPT
{
	FILE* fpFileOut = (FILE*)a_userData;
	fwrite(a_buffer, 1, a_bufLen, fpFileOut);
}


static int DirCompressFilterFunction(const char*, void*, const DirIterFileData*) CPPUTILS_NOEXCEPT
{
	return 0;
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////*/
static void DecompressFileStartCallback(void* a_userData)
{
	SDecompressData* pData = (SDecompressData*)a_userData;
	pData->fpFileOut = fopen_zlibandtls(pData->cpcFileOrFolderNameOut, "wb");
	if (!pData->fpFileOut) {
		pData->hasError = 1;
	}
}


static void DecompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	SDecompressData* pData = (SDecompressData*)a_userData;
	fwrite(a_buffer, 1, a_bufLen, pData->fpFileOut);
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static void DecompressDirStartCallback(void* a_userData)
{
	SDecompressData* pData = (SDecompressData*)a_userData;
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
}

static void DecompressDirFileOrDirStartCallback(const DirIterFileData* a_pFileData, const struct SFileItem* a_pExtraData, void* a_userData)
{
	SDecompressData* pData = (SDecompressData*)a_userData;

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
		char vcFileNameBuffer[4096];
		snprintf_zlibandtls(vcFileNameBuffer, 4095, "%s/%s", pData->directoryPath, a_pFileData->pFileName);
		//sopen_zlibandtls(&(pData->fd),vcFileNameBuffer, _O_WRONLY  | _O_CREAT | _O_BINARY, (int)a_pExtraData->mode);
		//sopen_zlibandtls(&(pData->fd), vcFileNameBuffer, _O_WRONLY | _O_CREAT | _O_BINARY, (_S_IFREG|_S_IREAD | _S_IWRITE| _S_IEXEC)& a_pExtraData->mode);
		sopen_zlibandtls(&(pData->fd), vcFileNameBuffer, ZLIBWT_O_WRONLY | ZLIBWT_O_CREAT | ZLIBWT_O_BINARY, mode_fo_zlibandtls(a_pExtraData->mode));
		if (pData->fd < 0) {
			pData->hasError = 1;
			return;
		}
	}
}


static void DecompressDirFileReadCallback(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	SDecompressData* pData = (SDecompressData*)a_userData;
    size_t szRet = (size_t)write_zlibandtls(pData->fd, a_buffer, a_bufLen);
    (void)szRet;
}


static void DecompressDirFileEndCallback(void* a_userData)
{
	SDecompressData* pData = (SDecompressData*)a_userData;
	close_zlibandtls(pData->fd);
}


static void DecompressDirDirEndCallback(void* a_userData)
{
	SDecompressData* pData = (SDecompressData*)a_userData;
	char* cpcTerm = strrchr(pData->directoryPath, '/');
	assert(cpcTerm);
	const size_t newStrLen = (size_t)(cpcTerm-(pData->directoryPath));
	*cpcTerm = 0;
	pData->directoryPathLen = newStrLen;
}
