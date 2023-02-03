
// zlib_decompress_routines.cpp
// 2018 Feb 11
//
// docs
// https://www.installsetupconfig.com/win32programming/windowsdiskapis2_3.html 
// https://github.com/pbatard/rufus/blob/master/src/drive.c
//

#include <zlib_with_tools/stdlib_zlibandtls.h>
#include <zlib_with_tools/string_zlibandtls.h>
#include <zlib_with_tools/zlib_decompress_routines.h>
#include <zlib.h>
#include <memory>
#include <assert.h>
#include <stdint.h>
#include <sys/stat.h>
#include <direct.h>
#include <stdlib.h>


#define		END_OF_FILE		2018


CPPUTILS_BEGIN_C


typedef struct SFileItemList{
	SFileItem*		item;
	SFileItemList*	next;
}SFileItemList;


typedef struct SUserDataForClbk{
	SCompressDecompressHeader	headerBase;
	SCompressDecompressHeader*	headerPtr;
	FILE*						currentFile;
	SFileItemList				*first,*current;
	const char*					dirName;
	uint32_t					alreadyRead;
	uint32_t					readOnCurrentFile;
}SUserDataForClbk;


static int CallbackForDecompressToFile(const void*a_buffer, int a_bufLen, void*a_userData);
static int CallbackForDecompressToFolder(const void*a_buffer, int a_bufLen, void*a_userData);


ZLIBANDTLS_EXPORT int ZlibDecompressBufferToCallback(
	z_stream* a_strm,
	void* a_out, int a_outBufferSize,
	typeDecompressCallback a_clbk,void* a_userData)
{
	int retInf, retClbk;

	/* run inflate() on input until output buffer not full */
	do {

		a_strm->avail_out = a_outBufferSize;
		a_strm->next_out = (Bytef*)a_out;

		retInf = inflate(a_strm, Z_NO_FLUSH);
		assert(retInf != Z_STREAM_ERROR);  /* state not clobbered */
		switch (retInf) {
		case Z_NEED_DICT:
			retInf = Z_DATA_ERROR;     /* and fall through */
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			//(void)inflateEnd(&strm);
			return retInf;
		}

		retClbk = (*a_clbk)(a_out,a_outBufferSize-a_strm->avail_out,a_userData);
		if(retClbk<0){return retClbk;}

	} while (a_strm->avail_out == 0);

	return retInf;
}


ZLIBANDTLS_EXPORT int ZlibDecompressFileToCallback(
	z_stream* a_strm,
	FILE* a_source,
	void* a_in, int a_inBufferSize,
	void* a_out, int a_outBufferSize,
	typeDecompressCallback a_clbk, void* a_userData)
{
	int retInf;

	/* decompress until deflate stream ends or end of file */
	do {
		a_strm->avail_in = (uInt)fread(a_in, 1, a_inBufferSize, a_source);
		if (ferror(a_source)) {return Z_ERRNO; }
		if (a_strm->avail_in == 0){break;}
		a_strm->next_in = (Bytef*)a_in;

		retInf=ZlibDecompressBufferToCallback(a_strm,a_out,a_outBufferSize,a_clbk,a_userData);
		if(retInf<0){return retInf;}

	} while (retInf != Z_STREAM_END);

	return 0;

}


ZLIBANDTLS_EXPORT int ZlibDecompressBufferToFile(
	z_stream* a_strm,
	void* a_out, int a_outBufferSize,
	FILE *a_dest)
{
	return ZlibDecompressBufferToCallback(a_strm, a_out, a_outBufferSize, &CallbackForDecompressToFile, a_dest);
}




/* Decompress from file source to file dest until stream ends or EOF.
inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
allocated for processing, Z_DATA_ERROR if the deflate data is
invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
the version of the library linked do not match, or Z_ERRNO if there
is an error reading or writing the files. */
ZLIBANDTLS_EXPORT int ZlibDecompressFile(FILE *a_source, FILE *a_dest)
{
	int ret;
	int nZstrInited = 0;
	unsigned have;
	z_stream strm;
	
	unsigned char* in = (unsigned char*)malloc(DEF_CHUNK_SIZE);
	if (!in) { return -1; }
	::std::unique_ptr<unsigned char, decltype(&::free) > in_up(in,&::free);

	unsigned char*  out = (unsigned char*)malloc(DEF_CHUNK_SIZE);
	if (!out) { return -1; }
	::std::unique_ptr<unsigned char, decltype(&::free) > out_up(out, &::free);

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK) { goto returnPoint; }
	nZstrInited = 1;

	/* decompress until deflate stream ends or end of file */
	do {

		strm.avail_in = (uInt)fread(in, 1, DEF_CHUNK_SIZE, a_source);
		if (ferror(a_source)) {
			(void)inflateEnd(&strm);
			return Z_ERRNO;
		}
		if (strm.avail_in == 0)
			break;
		strm.next_in = in;

		/* run inflate() on input until output buffer not full */
		do {

			strm.avail_out = DEF_CHUNK_SIZE;
			strm.next_out = out;

			ret = inflate(&strm, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret) {
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return ret;
			}

			have = DEF_CHUNK_SIZE - strm.avail_out;
			if (fwrite(out, 1, have, a_dest) != have || ferror(a_dest)) {
				(void)inflateEnd(&strm);
				return Z_ERRNO;
			}

		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	
returnPoint:
	if(nZstrInited){(void)inflateEnd(&strm);}
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}


ZLIBANDTLS_EXPORT int ZlibDecompressFolder(FILE *a_source, const char* a_outDirectoryPath)
{
	SFileItemList *pItem,*pItemNext;
	SUserDataForClbk aData;
	z_stream strm;
	int nReturn, nInited=0;
	
	
	unsigned char* in = (unsigned char*)malloc(DEF_CHUNK_SIZE);
	if (!in) { return -1; }
	::std::unique_ptr<unsigned char, decltype(&::free) > in_up(in, &::free);

	unsigned char* out = (unsigned char*)malloc(DEF_CHUNK_SIZE);
	if (!out) { return -1; }
	::std::unique_ptr<unsigned char, decltype(&::free) > out_up(out, &::free);

	memset(&aData, 0, sizeof(SUserDataForClbk));
	aData.dirName = a_outDirectoryPath;

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	nReturn = inflateInit(&strm);
	if (nReturn != Z_OK){return nReturn;}
	nInited = 1;

	nReturn = _mkdir(a_outDirectoryPath);
	if ((nReturn<0) && (errno == ENOENT)) { goto returnPoint; }

	nReturn=ZlibDecompressFileToCallback(&strm,a_source,in,DEF_CHUNK_SIZE,out,DEF_CHUNK_SIZE,CallbackForDecompressToFolder,&aData);


returnPoint:

	pItem = aData.first;
	while(pItem){
		pItemNext = pItem->next;
		free(pItem);
		pItem = pItemNext;
	}
	if(aData.headerPtr){DestroyCompressDecompressHeader(aData.headerPtr);}
	if(nInited){(void)inflateEnd(&strm);} // if we are here then init is ok

	return nReturn;
}


#ifdef ZLIB_DECOMPRESS_FROM_WEB

ZLIBANDTLS_EXPORT int ZlibDecompressWebToCallback(
	z_stream* a_strm,
	HINTERNET a_source,
	void* a_in, int a_inBufferSize,
	void* a_out, int a_outBufferSize,
	typeDecompressCallback a_clbk, void* a_userData)
{
	int retInf;
	DWORD dwRead;

	/* decompress until deflate stream ends or end of file */
	do {
		if (!InternetReadFile(a_source, a_in, a_inBufferSize, &dwRead)) { return Z_ERRNO; }
		a_strm->avail_in = dwRead;
		if (a_strm->avail_in == 0) { break; } // we are done
		a_strm->next_in = (Bytef*)a_in;

		retInf = ZlibDecompressBufferToCallback(a_strm, a_out, a_outBufferSize, a_clbk, a_userData);
		if (retInf < 0) { return retInf; }

	} while (retInf != Z_STREAM_END);

	return 0;

}


ZLIBANDTLS_EXPORT int ZlibDecompressFromWeb(const char *a_cpcUrl, const char* a_outDirectoryPath)
{
	HINTERNET	hSession = NULL, hURL = NULL;
	SFileItemList *pItem,*pItemNext;
	SUserDataForClbk aData;
	z_stream strm;
	int nReturn, nInited=0;
	unsigned char in[DEF_CHUNK_SIZE];
	unsigned char out[DEF_CHUNK_SIZE];

	memset(&aData, 0, sizeof(SUserDataForClbk));
	aData.dirName = a_outDirectoryPath;

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	nReturn = inflateInit(&strm);
	if (nReturn != Z_OK){return nReturn;}
	nInited = 1;

	hSession = InternetOpenA("DesyCloud", NULL, NULL, NULL, NULL);
	if (!hSession){goto returnPoint;}

	hURL = InternetOpenUrlA(
		hSession, a_cpcUrl,
		NULL, 0,
		INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE,
		NULL);
	if (!hURL) { goto returnPoint; }

	nReturn = _mkdir(a_outDirectoryPath);
	if ((nReturn<0) && (errno == ENOENT)) { goto returnPoint; }

	nReturn=ZlibDecompressWebToCallback(&strm,hURL,in,DEF_CHUNK_SIZE,out,DEF_CHUNK_SIZE,CallbackForDecompressToFolder,&aData);


returnPoint:

	pItem = aData.first;
	while(pItem){
		pItemNext = pItem->next;
		free(pItem);
		pItem = pItemNext;
	}
	if(aData.headerPtr){DestroyCompressDecompressHeader(aData.headerPtr);}
	if(nInited){(void)inflateEnd(&strm);} // if we are here then init is ok

	if (hURL) { InternetCloseHandle(hURL); }
	if (hSession) { InternetCloseHandle(hSession); }

	return nReturn;
}

#endif  // #ifdef ZLIB_DECOMPRESS_FROM_WEB


/*////////////////////////////////////////////////////////////////////////////*/

static int CallbackForDecompressToFile(const void*a_buffer, int a_bufLen, void*a_userData)
{
	FILE* fpOutFile = (FILE*)a_userData;

	if (fwrite(a_buffer, 1, a_bufLen, fpOutFile) != a_bufLen || ferror(fpOutFile)) {
		return Z_ERRNO;
	}

	return 0;
}


static int PrepareDirIfNeeded(char* a_cpcFilePath,int a_nIsDir);


static int CallbackForDecompressToFolder(const void*a_buffer, int a_bufLen, void*a_userData)
{
	char* pcBuffer;
	uint32_t unCpy;
	SUserDataForClbk* pUserData = (SUserDataForClbk*)a_userData;
	int nRetDir;
	char vcDirectoryName[MAX_PATH];
	char vcFileName[MAX_PATH];

	if(pUserData->alreadyRead<sizeof(SCompressDecompressHeader)){
		if((a_bufLen + pUserData->alreadyRead)>sizeof(SCompressDecompressHeader) ){
			pcBuffer = (char*)a_buffer;
			unCpy = sizeof(SCompressDecompressHeader) - pUserData->alreadyRead;
			memcpy(((char*)&(pUserData->headerBase))+pUserData->alreadyRead, a_buffer, unCpy);
			pUserData->headerPtr = ZlibCreateAndCopyComprDecomprHeader(&pUserData->headerBase,0);
			if(!pUserData->headerPtr){return -1;}
			memcpy(pUserData->headerPtr,&(pUserData->headerBase),sizeof(SCompressDecompressHeader));
			a_bufLen -= unCpy;
			pcBuffer += unCpy;
			a_buffer = pcBuffer;
			pUserData->alreadyRead += unCpy;

		}
		else {
			memcpy(((char*)&(pUserData->headerBase)) + pUserData->alreadyRead, a_buffer, a_bufLen);
			return 0;
		}
	}
	
	
	if (pUserData->alreadyRead<pUserData->headerBase.wholeHeaderSizeInBytes) {
		if ((a_bufLen + pUserData->alreadyRead)>pUserData->headerBase.wholeHeaderSizeInBytes) {
			pcBuffer = (char*)a_buffer;
			unCpy = pUserData->headerBase.wholeHeaderSizeInBytes- pUserData->alreadyRead;
			memcpy(((char*)(pUserData->headerPtr)) + pUserData->alreadyRead, a_buffer, unCpy);
			a_bufLen -= unCpy;
			pcBuffer += unCpy;
			a_buffer = pcBuffer;
			pUserData->alreadyRead += unCpy;
		}
		else{
			memcpy(((char*)(pUserData->headerPtr)) + pUserData->alreadyRead, a_buffer, a_bufLen);
			return 0;
		}
	}


	if(!pUserData->first){
		SFileItemList* pList=NULL, *pPrev = NULL;
		SFileItem* pItem0 = (SFileItem*)( (char*)(pUserData->headerPtr)+ sizeof(SCompressDecompressHeader)  );
		uint32_t i=0;
		for(; i<pUserData->headerBase.numberOfItems;++i){
			pList = (SFileItemList*)malloc(sizeof(SFileItemList));
			if(!pList){return -1;}
			pList->item = pItem0;
			if(pPrev){pPrev->next=pList;}
			else {pUserData->first= pList;}
			pItem0 = (SFileItem*)(  ((char*)pItem0)+ LEN_FROM_ITEM(pItem0)  );
			pPrev = pList;
		}
		if(pList){pList->next = NULL;}
		pUserData->current = pUserData->first;
	}


	while (pUserData->current) {
		if (pUserData->current->item->fileSize == 0) {  // this is a directory
			snprintf_zlibandtls(vcDirectoryName, MAX_PATH, "%s\\%s", pUserData->dirName, ITEM_NAME(pUserData->current->item));
			nRetDir=PrepareDirIfNeeded(vcDirectoryName,1);
			if ((nRetDir < 0) /*&& (errno == ENOENT)*/) { return -2; }

		}
		else if ((pUserData->readOnCurrentFile + a_bufLen) > pUserData->current->item->fileSize) {
			if (!pUserData->currentFile) {
				snprintf_zlibandtls(vcFileName, MAX_PATH, "%s\\%s", pUserData->dirName, ITEM_NAME(pUserData->current->item));
				//PrepareDirIfNeeded(vcFileName, 0);
				pUserData->currentFile = fopen_zlibandtls(vcFileName, "wb");
				if (!pUserData->currentFile) { return -3; }
				pUserData->readOnCurrentFile = 0;
			}

			unCpy = pUserData->current->item->fileSize - pUserData->readOnCurrentFile;

			if (fwrite(a_buffer, 1, unCpy, pUserData->currentFile) != unCpy) { fclose(pUserData->currentFile); pUserData->currentFile = NULL; return -4; }
			pcBuffer = (char*)a_buffer;
			a_bufLen -= unCpy;
			pcBuffer += unCpy;
			a_buffer = pcBuffer;
			fclose(pUserData->currentFile);
			pUserData->currentFile = NULL;
			pUserData->readOnCurrentFile = 0;

		}
		else {
			if (!pUserData->currentFile) {
				snprintf_zlibandtls(vcFileName, MAX_PATH, "%s\\%s", pUserData->dirName, ITEM_NAME(pUserData->current->item));
				//PrepareDirIfNeeded(vcFileName,0);
				pUserData->currentFile = fopen_zlibandtls(vcFileName, "wb");
				if (!pUserData->currentFile) { return -3; }
				pUserData->readOnCurrentFile = 0;
			}
			if (fwrite(a_buffer, 1, a_bufLen, pUserData->currentFile) != a_bufLen) { fclose(pUserData->currentFile); pUserData->currentFile = NULL; return -4; }
			pUserData->readOnCurrentFile += a_bufLen;
			return 0;
		}

		pUserData->current = pUserData->current->next;

	}  // while(pUserData->current){

	return 0;
}


static int PrepareDirIfNeeded(char* a_cpcFilePath,int a_nIsDir)
{
	int nRetDir = -1;

	if (a_nIsDir) {
		nRetDir=_mkdir(a_cpcFilePath);
		if (nRetDir < 0) {
			if (errno == ENOENT) { return -3; }
			else { nRetDir = 0; }
		}
	}

	return nRetDir;
}


CPPUTILS_END_C
