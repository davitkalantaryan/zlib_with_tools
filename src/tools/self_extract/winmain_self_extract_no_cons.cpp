
#define  WIN_MAIN_APP
#define WAIT_DEBUGGER

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <shellapi.h>
#include "zlib_with_tools/zlib_compression_routines.h"
#include "zlib_with_tools/zlib_decompress_routines.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define OUT_FILE_NAME_01	"__out.exe"
#define OUT_FOLDER_NAME_01	"out_dir"

#ifdef _MSC_VER
#pragma comment (lib,"zlib.lib")
#endif

#ifdef _DEBUG
#define MAX_EXE_SIZE	2097152
#else
#define MAX_EXE_SIZE	524288
#endif

#define VC_BUFFER_SIZE		4192

static const char* s_cpcExeName = nullptr;

static int FilterFunction(const char*, void*, const DirIterFileData* a_data)
{
	if ((a_data->pFileName[0] == '_')&& (a_data->pFileName[1] == '_')) {
		return 1;
	}

	if (strcmp(s_cpcExeName, a_data->pFileName) == 0) {
		return 1;
	}

	return 0;
}


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
	DWORD dwErrCode;
	STARTUPINFOA aStartInfo;
	PROCESS_INFORMATION aProcInf;
	char vcBuffer[VC_BUFFER_SIZE];
	char vcExePathThenDir[VC_BUFFER_SIZE];
	//char vcMainPath[VC_BUFFER_SIZE];
	char* pcDelimer;
	FILE *fpExe = NULL, *fpOut=NULL;
	size_t unNextRead, unRemainingBytes, unRWcount;
	size_t fileSize;
	errno_t errFl;

#if defined(_DEBUG)
#ifdef WAIT_DEBUGGER
	while (!IsDebuggerPresent()) {
		SleepEx(10, TRUE);
	}
#endif
#else
	SHFILEOPSTRUCTA shfo = {
	NULL,
	FO_DELETE,
	OUT_FOLDER_NAME_01,
	NULL,
	FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION,
	FALSE,
	NULL,
	NULL };

#endif

#ifdef WIN_MAIN_APP
	//(void)a_hInstance;
	(void)a_hPrevInstance;
	(void)a_lpCmdLine;
	(void)a_nShowCmd;
	GetModuleFileNameA(a_hInstance, vcExePathThenDir, VC_BUFFER_SIZE - 1);
#else
	(void)a_argc;
	strncpy_s(vcExePathThenDir, VC_BUFFER_SIZE - 1,a_argv[0], VC_BUFFER_SIZE - 1);
#endif

	pcDelimer = strrchr(vcExePathThenDir, '\\');
	if (pcDelimer) {
		s_cpcExeName = _strdup(pcDelimer + 1);
	}
	else {
		pcDelimer = strrchr(vcExePathThenDir, '/');
		if (pcDelimer) {
			s_cpcExeName = _strdup(pcDelimer + 1);
		}
		else {
			s_cpcExeName = _strdup(vcExePathThenDir);
		}
	}

	errFl = fopen_s(&fpExe, vcExePathThenDir, "rb");
	if (errFl) {
		return 1;
	}

	fseek(fpExe, 0L, SEEK_END);
	fileSize = (size_t)ftell(fpExe);

	if (fileSize > MAX_EXE_SIZE) {
		fseek(fpExe, MAX_EXE_SIZE, SEEK_SET);
		nReturn = ZlibDecompressFolder(fpExe, OUT_FOLDER_NAME_01);

		if (nReturn) { goto returnPoint; }

		ZeroMemory(&aStartInfo, sizeof(aStartInfo));
		//aStartInfo.dwFlags = STARTF_USESTDHANDLES;
		aStartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		aStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		aStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
		aStartInfo.wShowWindow = SW_HIDE;

		ZeroMemory(&aProcInf, sizeof(PROCESS_INFORMATION));

		//strcat_s(vcMainPath, VC_BUFFER_SIZE)

		dwErrCode = CreateProcessA(
			OUT_FOLDER_NAME_01 "\\main.exe",
			//const_cast<char*>(a_cpcExecute),
			a_lpCmdLine,
			NULL,
			NULL,
			TRUE,
			NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
			NULL,
			NULL,
			&aStartInfo,
			&aProcInf
		);

		if (dwErrCode) {
			WaitForSingleObject(aProcInf.hProcess, INFINITE);
			CloseHandle(aProcInf.hThread);
			CloseHandle(aProcInf.hProcess);
		}

#ifndef _DEBUG
		//RemoveDirectoryA(OUT_FOLDER_NAME_01);
		SHFileOperationA(&shfo);
#endif

	}
	else {
		errFl = fopen_s(&fpOut, OUT_FILE_NAME_01, "wb");
		if (errFl) {
			fpOut = NULL;
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

		nReturn = ZlibCompressFolder(vcExePathThenDir, fpOut, Z_DEFAULT_COMPRESSION,&FilterFunction,CPPUTILS_NULL);
	}
	
returnPoint:
	if (fpOut) {
		fclose(fpOut);
	}
	if (fpExe) {
		fclose(fpExe);
	}
	free((char*)s_cpcExeName);

	return nReturn;
}
