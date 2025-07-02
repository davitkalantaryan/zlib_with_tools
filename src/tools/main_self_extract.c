
//#define WAIT_DEBUGGER

#include <zlib_with_tools/zlibwt_compression_routines.h>
#include <zlib_with_tools/zlibwt_decompress_routines.h>
#include <zlib_with_tools/utils/string_zlibandtls.h>
#include <zlib_with_tools/utils/stdio_zlibandtls.h>
#include <zlib_with_tools/utils/io_zlibandtls.h>
#include <zlib_with_tools/zlibwt_zlib_h_wrapper.h>
#include <resource_handler/resource_handler.h>
#include <system/create_process.h>
#include <cinternal/disable_compiler_warnings.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <shellapi.h>
#else
#include <unistd.h>
#include <directory_iterator/directory_iterator.h>
#endif
#include <cinternal/undisable_compiler_warnings.h>

#ifdef WAIT_DEBUGGER
#define ZLIBWT_GETCH(...)	printf("press any key then enter to exit");fflush(stdout);getchar();
#define ZLIBWT_SE_DEBUG(...)	printf(__VA_ARGS__); printf("\n")
#else
#define ZLIBWT_GETCH(...)
#define ZLIBWT_SE_DEBUG(...)
#endif


#define ZLIBWT_MAIN_ICON_FILE	"main.ico"
#define OUT_FILE_NAME_01		"__out.exe"
#define OUT_FOLDER_NAME_01		".out_dir"
#define ZLIBWT_SE_OUT_DIR_STR_LEN_PLUS1   9

#ifdef _MSC_VER
#pragma comment (lib,"zlib_st.lib")
#endif

#ifdef NDEBUG
#define MAX_EXE_SIZE	524288
#else
#define MAX_EXE_SIZE	2097152
#endif

#define VC_BUFFER_SIZE		4192

static const char* s_cpcExeName = CPPUTILS_NULL;

static void CompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData);
static int  DirCompressFilterFunction(const char*, void*, const DirIterFileData* a_data);
static void RemoveNonEmptyDirectory(const char* a_dirPath);


CPPUTILS_CODE_INITIALIZER(main_self_extract_common_init) {
#ifdef _WIN32
	FILE* fpFile;
	FreeConsole();
	AttachConsole(ATTACH_PARENT_PROCESS);
	if (GetConsoleWindow()) {
		freopen_s(&fpFile, "CONOUT$", "w", stdout); // redirect stdout to console
		freopen_s(&fpFile, "CONOUT$", "w", stderr); // redirect stderr to console
		freopen_s(&fpFile, "CONIN$", "r", stdin); // redirect stdin to console
	}
#endif
}

struct CPPUTILS_DLL_PRIVATE SCompressData {
	int			fd;
	uint32_t	outFileNotFound;
	uint32_t	ownFileNotFound;
	int			reserved01;
	const char* outFileName;
	const char* reserved02;
};


int main(int a_argc, char* a_argv[])
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
    char* pcArg0 = CPPUTILS_NULL;


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

	(void)a_argc;
	strncpy_zlibandtls(vcExePathThenDir, a_argv[0], VC_BUFFER_SIZE - 1);

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
		enum TypeOfCompressedContent dcmprsRet;

        pcArg0 = a_argv[0] = strdup_zlibandtls(OUT_FOLDER_NAME_01 ZLIBWT_FILE_DELIM "maind.exe");
        if (!a_argv[0]) { goto returnPoint; }

		fseek(fpExe, MAX_EXE_SIZE, SEEK_SET);
        RemoveNonEmptyDirectory(OUT_FOLDER_NAME_01);
		dcmprsRet = ZlibWtDecompressFileOrDirEx(fpExe, OUT_FOLDER_NAME_01,0); // todo: track archive size
		nReturn = (dcmprsRet == CompressedContentDirectory) ? 0 : 1;

		if (nReturn) { goto returnPoint; }

        procHandle = SystemCreateProcessU(a_argv);

		if (procHandle) {
			SystemWaitAndClearProcess(procHandle, &nRet);
			procHandle = CPPUTILS_NULL;
		}
		else {
			nRet = 1;
		}


		if (nRet) {
			shouldRemoveDirectory = false; // later on this will be done on mre fancy way
            pcArg0[ZLIBWT_SE_OUT_DIR_STR_LEN_PLUS1+4] = '.';
            pcArg0[ZLIBWT_SE_OUT_DIR_STR_LEN_PLUS1+5] = 'e';
            pcArg0[ZLIBWT_SE_OUT_DIR_STR_LEN_PLUS1+6] = 'x';
            pcArg0[ZLIBWT_SE_OUT_DIR_STR_LEN_PLUS1+7] = 'e';
            pcArg0[ZLIBWT_SE_OUT_DIR_STR_LEN_PLUS1+8] = 0;
			procHandle = SystemCreateProcessU(a_argv);
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
		sopen_zlibandtls(&(aCmprsData.fd), OUT_FILE_NAME_01, _O_TRUNC| _O_CREAT|_O_WRONLY | _O_BINARY, _S_IREAD | _S_IWRITE | _S_IREAD);
#else
        sopen_zlibandtls(&(aCmprsData.fd),OUT_FILE_NAME_01, O_TRUNC|O_CREAT|O_WRONLY, S_IRWXU | S_IRWXG |  S_IROTH|S_IXOTH);
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

		close_zlibandtls(aCmprsData.fd);

		ResourceHandlerSecIconToApp(OUT_FILE_NAME_01, ZLIBWT_MAIN_ICON_FILE);

#ifdef _WIN32
		sopen_zlibandtls(&(aCmprsData.fd), OUT_FILE_NAME_01, _O_WRONLY | _O_BINARY, _S_IREAD | _S_IWRITE | _S_IREAD);
#else
		sopen_zlibandtls(&(aCmprsData.fd), OUT_FILE_NAME_01, O_WRONLY, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
		if ((aCmprsData.fd) < 0) {
			goto returnPoint;
		}

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
    free(pcArg0);
	if (fpExe) {
		fclose(fpExe);
	}
	free((char*)s_cpcExeName);

	ZLIBWT_GETCH()

	return nReturn;
}


static void CompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
	struct SCompressData* pCmprsData = (struct SCompressData*)a_userData;
    size_t dummy = (size_t)write_zlibandtls(pCmprsData->fd,a_buffer, a_bufLen);
    (void)dummy;
}


static int DirCompressFilterFunction(const char* a_dirFilePath, void* a_userData, const DirIterFileData* a_data)
{
	struct SCompressData* pCmprsData = (struct SCompressData*)a_userData;
	CPPUTILS_STATIC_CAST(void, a_dirFilePath);

	if (pCmprsData->ownFileNotFound) {
		//if (!(a_data->isDir)) 
		{
			if (strcmp(s_cpcExeName, a_data->pFileName) == 0) {
				pCmprsData->ownFileNotFound = 0;
				return 1;
			}  //  if (strcmp(s_cpcExeName, a_data->pFileName) == 0) {
		}  //  if (!(a_data->isDir)) {
	}  //  if (pCmprsData->ownFileNotFound) {


	if (pCmprsData->outFileNotFound) {
		//if (!(a_data->isDir)) 
		{
			if (strcmp(pCmprsData->outFileName, a_data->pFileName) == 0) {
				pCmprsData->outFileNotFound = 0;
				return 1;
			}  //  if (strcmp(pCmprsData->outFileName, a_data->pFileName) == 0) {
		}  //  if (!(a_data->isDir)) {
	}  //  if (pCmprsData->outFileNotFound) {

	ZLIBWT_SE_DEBUG("filename: %s", a_data->pFileName);

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
