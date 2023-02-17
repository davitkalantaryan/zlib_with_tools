
//#define WAIT_DEBUGGER

#include <zlib_with_tools/zlibwt_compression_routines.h>
#include <zlib_with_tools/zlibwt_decompress_routines.h>
#include <zlib_with_tools/utils/string_zlibandtls.h>
#include <zlib_with_tools/utils/stdio_zlibandtls.h>
#include <system/exe/parent.hpp>
#include <zlib.h>
#include <stdlib.h>

#ifdef _WIN32
#include <shellapi.h>
#else
#endif

#define OUT_FILE_NAME_01	"__out.exe"
#define OUT_FOLDER_NAME_01	"out_dir"

#ifdef _MSC_VER
#pragma comment (lib,"zlib_st.lib")
#endif

#ifdef _DEBUG
#define MAX_EXE_SIZE	2097152
#else
#define MAX_EXE_SIZE	524288
#endif

#define VC_BUFFER_SIZE		4192

static const char* s_cpcExeName = nullptr;

static void CompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData);
static int  DirCompressFilterFunction(const char*, void*, const DirIterFileData* a_data);

#ifdef WIN_MAIN_APP
int APIENTRY WinMain(
	HINSTANCE a_hInstance,
	HINSTANCE a_hPrevInstance,
	LPSTR     a_lpCmdLine,
	int       a_nShowCmd)
#else
int main(int a_argc, char* a_argv[])
#endif
{
	int nReturn = 1;
	char vcBuffer[VC_BUFFER_SIZE];
	char vcExePathThenDir[VC_BUFFER_SIZE];
	char* pcDelimer;
	FILE *fpExe = NULL, *fpOut=NULL;
	size_t unNextRead, unRemainingBytes, unRWcount;
	size_t fileSize;
	char* vcpArgv[2] = {CPPUTILS_NULL,CPPUTILS_NULL };

#if !defined(NDEBUG) && defined(WAIT_DEBUGGER)
	while (!IsDebuggerPresent()) {
		SleepEx(10, TRUE);
	}
#endif

#ifdef _WIN32

	SHFILEOPSTRUCTA shfo = {
	NULL,
	FO_DELETE,
	OUT_FOLDER_NAME_01,
	NULL,
	FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION,
	FALSE,
	NULL,
	NULL };

#else

#endif


#ifdef WIN_MAIN_APP
	//(void)a_hInstance;
	(void)a_hPrevInstance;
	(void)a_lpCmdLine;
	(void)a_nShowCmd;
	GetModuleFileNameA(a_hInstance, vcExePathThenDir, VC_BUFFER_SIZE - 1);
#else
	(void)a_argc;
	strncpy_zlibandtls(vcExePathThenDir, a_argv[0], VC_BUFFER_SIZE - 1);
#endif

	pcDelimer = strrchr(vcExePathThenDir, '\\');
	if (pcDelimer) {
		s_cpcExeName = strdup_zlibandtls(pcDelimer + 1);
	}
	else {
		pcDelimer = strrchr(vcExePathThenDir, '/');
		if (pcDelimer) {
			s_cpcExeName = strdup_zlibandtls(pcDelimer + 1);
		}
		else {
			s_cpcExeName = strdup_zlibandtls(vcExePathThenDir);
		}
	}

	fpExe = fopen_zlibandtls(vcExePathThenDir, "rb");
	if (!fpExe) {
		return 1;
	}

	fseek(fpExe, 0L, SEEK_END);
	fileSize = (size_t)ftell(fpExe);

	if (fileSize > MAX_EXE_SIZE) {
		int nRet;
		::systemN::exe::parent::THandle procHandle;
		TypeOfCompressedContent dcmprsRet;

		vcpArgv[0] = strdup_zlibandtls(OUT_FOLDER_NAME_01 "\\main.exe");
		if (!vcpArgv[0]) { goto returnPoint; }

		fseek(fpExe, MAX_EXE_SIZE, SEEK_SET);
		dcmprsRet = ZlibWtDecompressFileOrDirEx(fpExe, OUT_FOLDER_NAME_01);
		nReturn = (dcmprsRet == CompressedContentDirectory) ? 0 : 1;

		if (nReturn) { goto returnPoint; }

#ifdef _WIN32
		procHandle = ::systemN::exe::parent::RunNoWaitW(0,0, vcpArgv[0]);
#else
		procHandle = ::systemN::exe::parent::RunNoWaitU(0,0, vcpArgv);
#endif
		if (!procHandle) {goto returnPoint;}

		systemN::exe::parent::WaitAndClear(procHandle, -1, &nRet);

#ifdef _WIN32
		//RemoveDirectoryA(OUT_FOLDER_NAME_01);
		SHFileOperationA(&shfo);
#else
#endif

	}
	else {
		fpOut = fopen_zlibandtls(OUT_FILE_NAME_01, "wb");
		if (!fpOut) {
			goto returnPoint;
		}

		fseek(fpExe, 0L, SEEK_SET);
		for (unRemainingBytes = fileSize; unRemainingBytes>0;) {
			unNextRead = (unRemainingBytes < VC_BUFFER_SIZE) ? unRemainingBytes : VC_BUFFER_SIZE;
			unRWcount = fread(vcBuffer, 1, unNextRead, fpExe);
			if (unRWcount > 0) {
				fwrite(vcBuffer, 1, unRWcount, fpOut);
				unRemainingBytes -= unRWcount;
			}
		}  // for (unRemainingBytes = fileSize;;) {

		fseek(fpOut, MAX_EXE_SIZE, SEEK_SET);

		if (pcDelimer) {
			*pcDelimer = 0;
		}

		//nReturn = ZlibCompressFolder(vcExePathThenDir, fpOut, Z_DEFAULT_COMPRESSION,&FilterFunction,CPPUTILS_NULL);
		nReturn = ZlibWtCompressDirectoryEx(vcExePathThenDir, Z_BEST_COMPRESSION, &CompressFileAndBlobCallback, &DirCompressFilterFunction, fpOut);
	}
	
returnPoint:
	free(vcpArgv[0]);
	if (fpOut) {
		fclose(fpOut);
	}
	if (fpExe) {
		fclose(fpExe);
	}
	free((char*)s_cpcExeName);

	return nReturn;
}


static void CompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	FILE* fpFileOut = (FILE*)a_userData;
	fwrite(a_buffer, 1, a_bufLen, fpFileOut);
}


static int DirCompressFilterFunction(const char*, void*, const DirIterFileData* a_data)
{
	if ((a_data->pFileName[0] == '_') && (a_data->pFileName[1] == '_')) {
		return 1;
	}

	if (strcmp(s_cpcExeName, a_data->pFileName) == 0) {
		return 1;
	}

	return 0;
}
