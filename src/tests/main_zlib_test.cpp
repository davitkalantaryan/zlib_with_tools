
// http://www.zlib.net/zlib_how.html


#include <zlib_with_tools/zlibwt_compression_routines.h>
#include <zlib_with_tools/zlibwt_decompress_routines.h>
#include <zlib_with_tools/stdio_zlibandtls.h>
#include <zlib.h>
#include <iostream>
#include <stdio.h>

#ifdef _MSC_VER
#pragma comment (lib,"zlib.lib")
#include <conio.h>
#endif

static void CompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData);
static int  DirCompressFilterFunction(const char*, void*, const DirIterFileData*);
//
static void DecompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData);
static void DecompressDirFileOrDirStartCallback(const DirIterFileData* a_pFileData, const struct SFileItem* a_pExtraData, void* a_userData);
static void DecompressDirFileAndBlobReadCallback(const void* a_buffer, size_t a_bufLen, void* a_userData);
static void DecompressDirFileEndCallback(void* a_userData);
static void DecompressDirDirEndCallback(void* a_userData);

struct SDecompressData {
	FILE* fpFileOut;
	const char* cpcFileNameOut;
	size_t		hasError;
};

// a -> compress file
// b -> decompress
// c -> compress directory

int main(int a_argc, char* a_argv[])
{
	if (a_argc < 3) {
		::std::cerr << "Function and file should be provided\n";
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
			::std::cerr << "Unable to open the file with the name \"" << cpcFileNameIn << "\"\n";
			return 1;
		}
		FILE* fpFileOut = fopen_zlibandtls(cpcFileNameOut, "wb");
		if (!fpFileOut) {
			fclose(fpFileIn);
			::std::cerr << "Unable to open the file with the name \"" << cpcFileNameOut << "\"\n";
			return 1;
		}
		nReturn = ZlibWtCompressFileEx(fpFileIn, Z_BEST_COMPRESSION, &CompressFileAndBlobCallback, fpFileOut);
		fclose(fpFileOut);
		fclose(fpFileIn);
	}break;
	case 'b': {
		FILE* fpFileIn = fopen_zlibandtls(cpcFileNameIn, "rb");
		if (!fpFileIn) {
			::std::cerr << "Unable to open the file with the name \"" << cpcFileNameIn << "\"\n";
			return 1;
		}

		SDecompressData aData{nullptr,cpcFileNameOut,0};

		char vcBufferOut[4096];
		const struct SZlibWtDecompressDirCallbacks clbks = { 
			&DecompressFileAndBlobCallback,
			& DecompressDirFileOrDirStartCallback,
			& DecompressDirFileAndBlobReadCallback,
			& DecompressDirFileEndCallback,
			& DecompressDirDirEndCallback,
			{0,0,0}};
		ZlibWtDecompressSessionPtr pSession = ZlibWtCreateDecompressSession(&clbks, &aData, vcBufferOut, 4096);
		if (!pSession) {
			if (aData.fpFileOut) { fclose(aData.fpFileOut); };
			fclose(fpFileIn);
			::std::cerr << "Unable to create decompress session \n";
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
		if (aData.fpFileOut) { fclose(aData.fpFileOut); };
		fclose(fpFileIn);
	}break;
	case 'c': {
		FILE* fpFileOut = fopen_zlibandtls(cpcFileNameOut, "wb");
		if (!fpFileOut) {
			::std::cerr << "Unable to open the file with the name \"" << cpcFileNameOut << "\"\n";
			return 1;
		}
		nReturn = ZlibWtCompressDirectoryEx(cpcFileNameIn, Z_BEST_COMPRESSION, &CompressFileAndBlobCallback, &DirCompressFilterFunction,fpFileOut);
		fclose(fpFileOut);
	}break;
	default:
		::std::cerr << "Wrong function is provided\n";
		return 1;
	}

    return nReturn;
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


static void CompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	FILE* fpFileOut = (FILE*)a_userData;
	fwrite(a_buffer, 1, a_bufLen, fpFileOut);
}


static int DirCompressFilterFunction(const char*, void*, const DirIterFileData*)
{
	return 0;
}


//
static void DecompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	SDecompressData* pData = (SDecompressData*)a_userData;
	if (!pData->fpFileOut) {
		pData->fpFileOut = fopen_zlibandtls(pData->cpcFileNameOut, "wb");
		if (!pData->fpFileOut) {
			pData->hasError = 1;
			return;
		}
	}
	fwrite(a_buffer, 1, a_bufLen, pData->fpFileOut);
}


static void DecompressDirFileOrDirStartCallback(const DirIterFileData* a_pFileData, const struct SFileItem* a_pExtraData, void* a_userData)
{
	//
}


static void DecompressDirFileAndBlobReadCallback(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
}


static void DecompressDirFileEndCallback(void* a_userData)
{
	//
}


static void DecompressDirDirEndCallback(void* a_userData)
{
	//
}
