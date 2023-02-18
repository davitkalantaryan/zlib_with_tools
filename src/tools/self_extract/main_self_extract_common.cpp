
#define WAIT_DEBUGGER

#include <zlib_with_tools/zlibwt_compression_routines.h>
#include <zlib_with_tools/zlibwt_decompress_routines.h>
#include <zlib_with_tools/utils/string_zlibandtls.h>
#include <zlib_with_tools/utils/stdio_zlibandtls.h>
#include <zlib_with_tools/utils/io_zlibandtls.h>
#include <system/create_process.h>
#include <zlib.h>
#include <stdlib.h>

#ifdef _WIN32
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
    int fdOut=-1;
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
        pcArg0 = a_argv[0] = strdup_zlibandtls(OUT_FOLDER_NAME_01 ZLIBWT_FILE_DELIM "main.exe");
        if (!a_argv[0]) { goto returnPoint; }
#endif

		fseek(fpExe, MAX_EXE_SIZE, SEEK_SET);
		dcmprsRet = ZlibWtDecompressFileOrDirEx(fpExe, OUT_FOLDER_NAME_01);
		nReturn = (dcmprsRet == CompressedContentDirectory) ? 0 : 1;

		if (nReturn) { goto returnPoint; }

#ifdef WIN_MAIN_APP
        procHandle = SystemCreateProcessU(OUT_FOLDER_NAME_01 ZLIBWT_FILE_DELIM "main.exe",a_lpCmdLine);
#else
        procHandle = SystemCreateProcessU(a_argv);
#endif
		if (!procHandle) {goto returnPoint;}

        SystemWaitndClearProcess(procHandle,&nRet);

        shouldRemoveDirectory= (nRet==0); // later on this will be done on mre fancy way

	}
	else {
        //fdOut = fopen_zlibandtls(OUT_FILE_NAME_01, "wb");
        sopen_zlibandtls(&fdOut,OUT_FILE_NAME_01,O_CREAT|O_WRONLY, S_IRWXU | S_IRWXG |  S_IROTH|S_IXOTH);
        if (fdOut<0) {
			goto returnPoint;
		}

		fseek(fpExe, 0L, SEEK_SET);
		for (unRemainingBytes = fileSize; unRemainingBytes>0;) {
			unNextRead = (unRemainingBytes < VC_BUFFER_SIZE) ? unRemainingBytes : VC_BUFFER_SIZE;
			unRWcount = fread(vcBuffer, 1, unNextRead, fpExe);
			if (unRWcount > 0) {
                unWrRet = (size_t)write_zlibandtls(fdOut,vcBuffer, unRWcount);
                (void)unWrRet;
				unRemainingBytes -= unRWcount;
			}
		}  // for (unRemainingBytes = fileSize;;) {

        lseek(fdOut, MAX_EXE_SIZE, SEEK_SET);

		if (pcDelimer) {
			*pcDelimer = 0;
		}

		//nReturn = ZlibCompressFolder(vcExePathThenDir, fpOut, Z_DEFAULT_COMPRESSION,&FilterFunction,CPPUTILS_NULL);
        nReturn = ZlibWtCompressDirectoryEx(vcExePathThenDir, Z_BEST_COMPRESSION, &CompressFileAndBlobCallback, &DirCompressFilterFunction, (void*)((size_t)fdOut));
	}
	
returnPoint:
    if(shouldRemoveDirectory){
        RemoveNonEmptyDirectory(OUT_FOLDER_NAME_01);
    }    
#ifdef WIN_MAIN_APP
#else
    free(pcArg0);
#endif
    if (fdOut>=0) {
        close_zlibandtls(fdOut);
	}
	if (fpExe) {
		fclose(fpExe);
	}
	free((char*)s_cpcExeName);

	return nReturn;
}


static void CompressFileAndBlobCallback(const void* a_buffer, size_t a_bufLen, void* a_userData)
{
    int fdOut = (int)((size_t)a_userData);
    size_t dummy = (size_t)write_zlibandtls(fdOut,a_buffer, a_bufLen);
    (void)dummy;
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
