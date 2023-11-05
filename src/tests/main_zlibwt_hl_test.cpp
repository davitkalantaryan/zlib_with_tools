
// http://www.zlib.net/zlib_how.html


#include <zlib_with_tools/zlibwt_compression_routines.h>
#include <zlib_with_tools/zlibwt_decompress_routines.h>
#include <zlib_with_tools/utils/stdio_zlibandtls.h>
#include <zlib_with_tools/zlibwt_zlib_h_wrapper.h>
#include <stdio.h>
#include <string.h>

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
		nReturn = (dcmprsRet == CompressedContentNone) ? 1 : 0;
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
	case 'd': {
		FILE* fpFileOut = fopen_zlibandtls(cpcFileNameOut, "wb");
		if (!fpFileOut) {
			fprintf(stderr, "Unable to open the file with the name \"%s\"\n", cpcFileNameOut);
			return 1;
		}
		struct SZlibWtExtraCompressionBuffer aExtraBufs;
		aExtraBufs.buffer = "mconfig.bconf";  // provide list with separated ';'
		aExtraBufs.bufferSize = strlen(aExtraBufs.buffer) + 1;
		aExtraBufs.ffilePath = "__not_updateabe_files";
		nReturn = ZlibWtCompressDirectoryWithBufferExRoot(cpcFileNameIn, 1, &aExtraBufs,Z_BEST_COMPRESSION, &CompressFileAndBlobCallback, &DirCompressFilterFunction, fpFileOut);
		fclose(fpFileOut);
	}break;
	case 'e': {
		FILE* fpFileOut = fopen_zlibandtls(cpcFileNameOut, "wb");
		if (!fpFileOut) {
			fprintf(stderr, "Unable to open the file with the name \"%s\"\n", cpcFileNameOut);
			return 1;
		}
		struct SZlibWtExtraCompressionBuffer vExtraBufs[3];
		vExtraBufs[0].buffer = "mconfig.bconf";  // provide list with separated ';'
		vExtraBufs[0].bufferSize = strlen(vExtraBufs[0].buffer) + 1;
		vExtraBufs[0].ffilePath = "a/b/__not_updateabe_files";
		vExtraBufs[1].buffer = "mconfig.bconf";  // provide list with separated ';'
		vExtraBufs[1].bufferSize = strlen(vExtraBufs[1].buffer) + 1;
		vExtraBufs[1].ffilePath = "plugins/a/b/__not_updateabe_files2";
		vExtraBufs[2].buffer = "mconfig.bconf";  // provide list with separated ';'
		vExtraBufs[2].bufferSize = strlen(vExtraBufs[2].buffer) + 1;
		vExtraBufs[2].ffilePath = "plugins/a/b/c/__not_updateabe_files3";
		nReturn = ZlibWtCompressDirectoryWithBufferEx(cpcFileNameIn, 3, vExtraBufs, Z_BEST_COMPRESSION, &CompressFileAndBlobCallback, &DirCompressFilterFunction, fpFileOut);
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
