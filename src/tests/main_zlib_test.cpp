
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

static void FileCompressCallback(const void* a_buffer, size_t a_bufLen, void* a_userData);
static void ZlibWtDecompressCallbackExample(const void* a_buffer, size_t a_bufLen, void* a_userData, const DirIterFileData* a_pFileData, const struct SFileItem* a_pExtraData);

// a -> compress file
// b -> decompress

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
		nReturn = ZlibWtCompressFileEx(fpFileIn, Z_BEST_COMPRESSION, &FileCompressCallback, fpFileOut);
		fclose(fpFileOut);
		fclose(fpFileIn);
	}break;
	case 'b': {
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

		char vcBufferOut[4096];
		ZlibWtDecompressSessionPtr pSession = ZlibWtCreateDecompressSession(&ZlibWtDecompressCallbackExample, fpFileOut, vcBufferOut, 4096);
		if (!pSession) {
			fclose(fpFileOut);
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
				fclose(fpFileOut);
				fclose(fpFileIn);
				return 1;
			}
			ZlibWtDecompressBufferToCallback(pSession, ((++nIndex) & 0x10) >> 4, vcBufferIn, freadRet);
			isFileof = feof(fpFileIn);
		} while (!isFileof);

		ZlibWtDestroyDecompressSession(pSession);
		fclose(fpFileOut);
		fclose(fpFileIn);
	}break;
	default:
		::std::cerr << "Wrong function is provided\n";
		return 1;
	}

    return nReturn;
}


static void FileCompressCallback(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	FILE* fpFileOut = (FILE*)a_userData;
	fwrite(a_buffer, 1, a_bufLen, fpFileOut);
}


static void ZlibWtDecompressCallbackExample(const void* a_buffer, size_t a_bufLen, void* a_userData, const DirIterFileData* a_pFileData, const struct SFileItem* a_pExtraData)
{
	(void)a_pFileData;
	(void)a_pExtraData;
	FILE* fpFileOut = (FILE*)a_userData;
	fwrite(a_buffer, 1, a_bufLen, fpFileOut);
}
