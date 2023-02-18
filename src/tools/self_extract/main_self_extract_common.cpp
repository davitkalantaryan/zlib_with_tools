
//#define WAIT_DEBUGGER

#include <zlib_with_tools/zlibwt_compression_routines.h>
#include <zlib_with_tools/zlibwt_decompress_routines.h>
#include <zlib_with_tools/utils/string_zlibandtls.h>
#include <zlib_with_tools/utils/stdio_zlibandtls.h>
#include <zlib_with_tools/utils/io_zlibandtls.h>
#include <system/create_process.h>
#include <zlib.h>
#include <stdlib.h>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <shellapi.h>
#else
#include <unistd.h>
#include <directory_iterator/directory_iterator.h>
#endif

#define OUT_FILE_NAME_01	"__out.exe"
#define OUT_FOLDER_NAME_01	"out_dir"

#ifdef _MSC_VER
#pragma comment (lib,"zlib_st.lib")
#endif

#ifdef NDEBUG
#define MAX_EXE_SIZE	524288
#else
#define MAX_EXE_SIZE	2097152
#endif

#define VC_BUFFER_SIZE		4192

static const char* s_cpcExeName = nullptr;

static void CompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData);
static int  DirCompressFilterFunction(const char*, void*, const DirIterFileData* a_data);
static void RemoveNonEmptyDirectory(const char* a_dirPath);

struct SCompressData {
	int fd;
	uint32_t outFileNotFound;
	uint32_t ownFileNotFound;
	const char* outFileName;
	const char* reserved02;
};

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
    bool shouldRemoveDirectory = false;
	int nReturn = 1;
	char vcBuffer[VC_BUFFER_SIZE];
	char vcExePathThenDir[VC_BUFFER_SIZE];
	char* pcDelimer;
    FILE *fpExe = CPPUTILS_NULL;
    size_t unWrRet;
	size_t unNextRead, unRemainingBytes, unRWcount;
	size_t fileSize;
#ifdef WIN_MAIN_APP
#else
    char* pcArg0 = CPPUTILS_NULL;
#endif

#if !defined(NDEBUG) && defined(WAIT_DEBUGGER)
#ifdef _WIN32
	while (!IsDebuggerPresent()) {
		SleepEx(10, TRUE);
	}
#else
    fprintf(stdout,"press any key then press enter to continue! ");
    fflush(stdout);
    getchar();
#endif
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
        TSystemProcessHandlePtr procHandle;
		TypeOfCompressedContent dcmprsRet;

#ifndef WIN_MAIN_APP
        pcArg0 = a_argv[0] = strdup_zlibandtls(OUT_FOLDER_NAME_01 ZLIBWT_FILE_DELIM "maind.exe");
        if (!a_argv[0]) { goto returnPoint; }
#endif

		fseek(fpExe, MAX_EXE_SIZE, SEEK_SET);
		dcmprsRet = ZlibWtDecompressFileOrDirEx(fpExe, OUT_FOLDER_NAME_01);
		nReturn = (dcmprsRet == CompressedContentDirectory) ? 0 : 1;

		if (nReturn) { goto returnPoint; }

#ifdef WIN_MAIN_APP
        procHandle = SystemCreateProcessW(OUT_FOLDER_NAME_01 ZLIBWT_FILE_DELIM "maind.exe",a_lpCmdLine);
#else
        procHandle = SystemCreateProcessU(a_argv);
#endif
		if (procHandle) {
			SystemWaitAndClearProcess(procHandle, &nRet);
			procHandle = CPPUTILS_NULL;
		}
		else {
			nRet = 1;
		}


		if (nRet) {
			shouldRemoveDirectory = false; // later on this will be done on mre fancy way
#ifdef WIN_MAIN_APP
			procHandle = SystemCreateProcessW(OUT_FOLDER_NAME_01 ZLIBWT_FILE_DELIM "main.exe", a_lpCmdLine);
#else
			pcArg0[4] = '.';
			pcArg0[5] = 'e';
			pcArg0[6] = 'x';
			pcArg0[7] = 'e';
			pcArg0[8] = 0;
			procHandle = SystemCreateProcessU(a_argv);
#endif
		}
		else {
			shouldRemoveDirectory = true;
		}

		if (procHandle) {
			SystemWaitAndClearProcess(procHandle, &nRet);
		}

	}
	else {
		struct SCompressData aCmprsData;

		aCmprsData.fd = -1;
		aCmprsData.outFileNotFound = 1;
		aCmprsData.ownFileNotFound = 1;
		aCmprsData.outFileName = OUT_FILE_NAME_01;

#ifdef _WIN32
		sopen_zlibandtls(&(aCmprsData.fd), OUT_FILE_NAME_01, O_CREAT | O_WRONLY | O_BINARY, _S_IREAD | _S_IWRITE | _S_IREAD);
#else
        sopen_zlibandtls(&(aCmprsData.fd),OUT_FILE_NAME_01,O_CREAT|O_WRONLY, S_IRWXU | S_IRWXG |  S_IROTH|S_IXOTH);
#endif
        if ((aCmprsData.fd) <0) {
			goto returnPoint;
		}

		fseek(fpExe, 0L, SEEK_SET);
		for (unRemainingBytes = fileSize; unRemainingBytes>0;) {
			unNextRead = (unRemainingBytes < VC_BUFFER_SIZE) ? unRemainingBytes : VC_BUFFER_SIZE;
			unRWcount = fread(vcBuffer, 1, unNextRead, fpExe);
			if (unRWcount > 0) {
                unWrRet = (size_t)write_zlibandtls((aCmprsData.fd),vcBuffer, unRWcount);
                (void)unWrRet;
				unRemainingBytes -= unRWcount;
			}
		}  // for (unRemainingBytes = fileSize;;) {

        lseek_zlibandtls((aCmprsData.fd), MAX_EXE_SIZE, SEEK_SET);

		if (pcDelimer) {
			*pcDelimer = 0;
		}

		//nReturn = ZlibCompressFolder(vcExePathThenDir, fpOut, Z_DEFAULT_COMPRESSION,&FilterFunction,CPPUTILS_NULL);
        nReturn = ZlibWtCompressDirectoryEx(vcExePathThenDir, Z_BEST_COMPRESSION, &CompressFileAndBlobCallback, &DirCompressFilterFunction, &aCmprsData);
		close_zlibandtls(aCmprsData.fd);
	}
	
returnPoint:
    if(shouldRemoveDirectory){
        RemoveNonEmptyDirectory(OUT_FOLDER_NAME_01);
    }    
#ifdef WIN_MAIN_APP
#else
    free(pcArg0);
#endif
	if (fpExe) {
		fclose(fpExe);
	}
	free((char*)s_cpcExeName);

	return nReturn;
}


static void CompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	struct SCompressData* pCmprsData = (struct SCompressData*)a_userData;
    size_t dummy = (size_t)write_zlibandtls(pCmprsData->fd,a_buffer, a_bufLen);
    (void)dummy;
}


static int DirCompressFilterFunction(const char* a_sourceDirectory, void* a_userData, const DirIterFileData* a_data)
{
	struct SCompressData* pCmprsData = (struct SCompressData*)a_userData;

	if (pCmprsData->ownFileNotFound) {
		if (!(a_data->isDir)) {
			if (strcmp(s_cpcExeName, a_data->pFileName) == 0) {
				pCmprsData->ownFileNotFound = 0;
				return 1;
			}  //  if (strcmp(s_cpcExeName, a_data->pFileName) == 0) {
		}  //  if (!(a_data->isDir)) {
	}  //  if (pCmprsData->ownFileNotFound) {


	if (pCmprsData->outFileNotFound) {
		if (!(a_data->isDir)) {
			if (strcmp(pCmprsData->outFileName, a_data->pFileName) == 0) {
				pCmprsData->outFileNotFound = 0;
				return 1;
			}  //  if (strcmp(pCmprsData->outFileName, a_data->pFileName) == 0) {
		}  //  if (!(a_data->isDir)) {
	}  //  if (pCmprsData->outFileNotFound) {

	return 0;
}


#ifndef _WIN32

struct SDirIterData{
    size_t count;
};

static int DirIterFuncForRemovinDirectory(const char* a_sourceDirectory,void* a_pUd, const DirIterFileData* a_pData) CPPUTILS_NOEXCEPT
{
    struct SDirIterData* pDt = (struct SDirIterData*)a_pUd;

    if (a_pData->isDir) {
        struct SDirIterData aDt = {0};
        char  vcStrFilePath[4096];
        if (a_pData->pFileName[0] == '.') {
            if ((a_pData->pFileName[1] == 0) || ((a_pData->pFileName[1] == '.') && (a_pData->pFileName[2] == 0))) { return 0; }
        }
        snprintf_di(vcStrFilePath, 4095, "%s/%s", a_sourceDirectory, a_pData->pFileName);
        IterateOverDirectoryFilesNoRecurse(vcStrFilePath, &DirIterFuncForRemovinDirectory, &aDt);
        if(aDt.count){
            ++(pDt->count);
        }
        else{
            rmdir(vcStrFilePath);
        }
    }
    else{
        int nRet;
        char  vcStrFilePath[4096];
        snprintf_di(vcStrFilePath, 4095, "%s/%s", a_sourceDirectory, a_pData->pFileName);
        nRet = unlink(vcStrFilePath);
        if(nRet){  //  most probably EBUSY
            ++(pDt->count);
        }
    }

    return 0;
}

#endif  //  #ifndef _WIN32


static void RemoveNonEmptyDirectory(const char* a_dirPath)
{

#ifdef _WIN32

    SHFILEOPSTRUCTA shfo = {
    CPPUTILS_NULL,
    FO_DELETE,
    a_dirPath,
    CPPUTILS_NULL,
    FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION,
    FALSE,
    CPPUTILS_NULL,
    CPPUTILS_NULL };

    SHFileOperationA(&shfo);

#else   //  #ifdef _WIN32

    struct SDirIterData aDt = {0};
    IterateOverDirectoryFilesNoRecurse(a_dirPath, &DirIterFuncForRemovinDirectory, &aDt);
    rmdir(a_dirPath);

#endif  //  #ifdef _WIN32

}
