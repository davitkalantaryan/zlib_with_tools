
// http://www.zlib.net/zlib_how.html


#include <zlib_with_tools/zlibwt_compression_routines.h>
#include <zlib_with_tools/utils/stdio_zlibandtls.h>
#include <zlib_with_tools/utils/string_zlibandtls.h>
#include <zlib_with_tools/common/util/common_argument_parser.hpp>
#include <zlib_with_tools/zlibwt_zlib_h_wrapper.h>
#include <cinternal/disable_compiler_warnings.h>
#include <iostream>
#ifdef _MSC_VER
#pragma comment (lib,"zlib.lib")
#if defined(_WIN32) && !defined(_M_ARM)
#include <conio.h>
#endif
#endif
#include <cinternal/undisable_compiler_warnings.h>


static void PrintHelp(void);
static void CompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData) CPPUTILS_NOEXCEPT;
static int  DirCompressFilterFunction(const char*, void*, const DirIterFileData*) CPPUTILS_NOEXCEPT;


/* compress or decompress from stdin to stdout */
int main(int a_argc, char * a_argv[])
{
	char** argv2 = a_argv + 1;
	FILE *fpOut=NULL;
	common::argument_parser aParser;
	int ret=1;
	int argc2 = a_argc-1;

	aParser.
		AddOption("--output-file,-of:Prvide output file name").
		AddOption("--config-file,-cf:Provide config file for input file tree").
		AddOption("--input-folder,-if:The name of the input folder")
		<<"--help,-h:To display this help";

	aParser.ParseCommandLine(argc2, argv2);

	if(aParser["-h"]){
		PrintHelp();
		std::cout<<aParser.HelpString()<<std::endl;
		ret = 0;
		goto returnPoint;
	}

	if (!aParser["-of"]) {
		fprintf(stderr, "Output file should be specified!\n");
		PrintHelp();
		std::cout << aParser.HelpString() << std::endl;
		goto returnPoint;
	}

	if (!aParser["-if"] && !aParser["-cf"] && (argc2==0)) {
		fprintf(stderr, "Input folder should be specified!\n");
		PrintHelp();
		std::cout << aParser.HelpString() << std::endl;
		goto returnPoint;
	}

	fpOut = fopen_zlibandtls(aParser["-of"], "wb");
	if (!fpOut) { 
		fprintf(stderr, "Unable to open output file %s\n", aParser["-of"]);
		goto returnPoint; 
	}

	if(aParser["-if"]){
		//ret = ZlibCompressFolderEx(aParser["-if"], fpOut, Z_DEFAULT_COMPRESSION,NULL,NULL);
		ret = ZlibWtCompressDirectoryEx(aParser["-if"], Z_BEST_COMPRESSION, &CompressFileAndBlobCallback, &DirCompressFilterFunction, fpOut);
	}
	else if(argc2){ 
		//ret = ZlibCompressFolder(argv2[0], fpOut, Z_DEFAULT_COMPRESSION,NULL,NULL);
		ret = ZlibWtCompressDirectoryEx(argv2[0], Z_BEST_COMPRESSION, &CompressFileAndBlobCallback, &DirCompressFilterFunction, fpOut);
	}
	else {
		//ret = CompressBasedOnConfig(aParser["-cf"], fpOut, Z_DEFAULT_COMPRESSION);
		ret = 0;
	}

	//ret = 0;
returnPoint:
	if (fpOut) { fclose(fpOut); }

	printf("return=%d\n",ret);
	if ((ret != Z_OK)&&(ret!=0)){/*zerr(ret);*/}

#if defined(_WIN32) & !defined(_M_ARM)
	printf("Press any key to exit!"); fflush(stdout);
	_getch();
	printf("\n");
#endif

	return ret;
}


static void PrintHelp(void)
{
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
