
// http://www.zlib.net/zlib_how.html


#include <zlib_with_tools/zlibwt_compression_routines.h>
#include <zlib_with_tools/zlibwt_decompress_routines.h>
#include <zlib_with_tools/utils/stdio_zlibandtls.h>
#include <zlib_with_tools/zlibwt_zlib_h_wrapper.h>
#include <stdio.h>

#ifdef _MSC_VER
#pragma comment (lib,"zlib.lib")
#endif

static void CompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData) CPPUTILS_NOEXCEPT;
static int  DirCompressFilterFunction(const char*, void*, const DirIterFileData*) CPPUTILS_NOEXCEPT;

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
			fprintf(stderr,"Unable to open the file with the name \"%s\"\n", cpcFileNameIn);
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
		const TypeOfCompressedContent dcmprsRet = ZlibWtDecompressFileOrDir(cpcFileNameIn, cpcFileNameOut);
		fprintf(stdout, "dcmprsRet:%d\n",(int)dcmprsRet);
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
