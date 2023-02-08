
// zlib_compression_routines.cpp
// 2017 Feb 12


// http://www.zlib.net/zlib_how.html

#include <zlib_with_tools/zlib_compression_routines.h>
#include <zlib_with_tools/stdio_zlibandtls.h>
#include <zlib.h>
#include <memory>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <sys/stat.h>


CPPUTILS_BEGIN_C

typedef struct SUserDataForDirCompress{
	TypeFilter		funcFilter;
	void*			userData;
	SCompressList	list;
	uint16_t		numberOfItems;
	uint16_t		headerSize;
	uint16_t		offsetToTakeRoot;
	uint16_t		lastFolderNum;
}SUserDataForDirCompress;

static int CallbackForCompressToFile(const void*a_buffer, int a_bufLen, void*a_userData);
static int DirectoryIterator(const char* a_sourceDirectory, void* a_pUd, const DirIterFileData* a_pData);


ZLIBANDTLS_EXPORT int ZlibCompressBufferToCallback(
	z_stream* a_strm, int a_flush,
	void* a_out, int a_outBufferSize,
	typeCompressCallback a_clbk,void* a_userData)
{
	int ret2, retZlib;

	/* run deflate() on input until output buffer not full, finish
	compression if all of source has been read in */
	do {

		a_strm->avail_out = a_outBufferSize;
		a_strm->next_out = (Bytef*)a_out;

		retZlib = deflate(a_strm, a_flush);		/* no bad return value */
		assert(retZlib != Z_STREAM_ERROR);		/* state not clobbered */

		ret2=(*a_clbk)(a_out,a_outBufferSize-a_strm->avail_out, a_userData);
		if(ret2){return ret2;}

	} while (a_strm->avail_out == 0);
	assert(a_strm->avail_in == 0);     /* all input will be used */
	return retZlib;
}


ZLIBANDTLS_EXPORT int ZlibCompressBufferToFile(
	z_stream* a_strm, int a_flush,
	void* a_out, int a_outBufferSize,
	FILE *a_dest)
{
	return ZlibCompressBufferToCallback(a_strm,a_flush,a_out,a_outBufferSize,&CallbackForCompressToFile,a_dest);
}


/* Compress from file source to file dest until EOF on source.
def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
allocated for processing, Z_STREAM_ERROR if an invalid compression
level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
version of the library linked do not match, or Z_ERRNO if there is
an error reading or writing the files. */
ZLIBANDTLS_EXPORT int ZlibCompressFileRawEx(
	z_stream* a_strm,
	FILE * a_source, FILE * a_dest,
	void* a_in, int a_inBufferSize,
	void* a_out, int a_outBufferSize,
	int a_nFlushInTheEnd)
{
    int ret, flush, isFileof=0;

	/* compress until end of file */
	do {
		a_strm->avail_in = (uInt)fread(a_in, 1, a_inBufferSize, a_source);
		if (ferror(a_source)) {return Z_ERRNO;}
		isFileof = feof(a_source);
		flush = (a_nFlushInTheEnd&&isFileof) ? Z_FINISH : Z_NO_FLUSH;
		a_strm->next_in = (Bytef*)a_in;

		ret = ZlibCompressBufferToFile(a_strm, flush,a_out,a_outBufferSize,a_dest);
        if(ret){return ret;}

	} while (!isFileof);
	
	CPPUTILS_DO_DEBUG_EXP(if(a_nFlushInTheEnd){assert(ret == Z_STREAM_END);})        /* stream will be complete */
	
	return Z_OK;
}



/* Compress from file source to file dest until EOF on source.
def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
allocated for processing, Z_STREAM_ERROR if an invalid compression
level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
version of the library linked do not match, or Z_ERRNO if there is
an error reading or writing the files. */
ZLIBANDTLS_EXPORT int ZlibCompressFileRaw(FILE * a_source, FILE * a_dest,int a_nCompressionLeel)
{
	z_stream strm;
	int nReturn =Z_OK;
	
	unsigned char* in = (unsigned char*)malloc(DEF_CHUNK_SIZE);
	if (!in) { return -1; }
	::std::unique_ptr<unsigned char, decltype(&::free) > in_up(in, &::free);

	unsigned char* out = (unsigned char*)malloc(DEF_CHUNK_SIZE);
	if (!out) { return -1; }
	::std::unique_ptr<unsigned char, decltype(&::free) > out_up(out, &::free);

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	nReturn = deflateInit(&strm, a_nCompressionLeel);
	if (nReturn != Z_OK){return nReturn;}

	nReturn=ZlibCompressFileRawEx(&strm,a_source,a_dest,in, DEF_CHUNK_SIZE,out, DEF_CHUNK_SIZE,1);

	(void)deflateEnd(&strm);
	return Z_OK;
}


ZLIBANDTLS_EXPORT int ZlibCompressFolderEx(const SCompressList* a_list, uint16_t a_headerSize, uint16_t a_numberOfItems, FILE *a_dest, int a_level)
{
	uint8_t *pCurrent2;
	SCompressDecompressHeader *pHeader2 = NULL;
	SFileItemList *pItem;
	z_stream strm;
	int nReturn = -1;
	uint16_t unLenFromItem;
	
	unsigned char* in = (unsigned char*)malloc(DEF_CHUNK_SIZE);
	if (!in) { return -1; }
	::std::unique_ptr<unsigned char, decltype(&::free) > in_up(in, &::free);

	unsigned char* out = (unsigned char*)malloc(DEF_CHUNK_SIZE);
	if (!out) { return -1; }
	::std::unique_ptr<unsigned char, decltype(&::free) > out_up(out, &::free);

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	nReturn = deflateInit(&strm, a_level);
	if (nReturn != Z_OK)
		return nReturn;

	pHeader2 = ZlibCreateCompressDecompressHeader(a_headerSize, CompressedContentDirectory, a_numberOfItems);
	if(!pHeader2){return ENOMEM;}

	pCurrent2 = ((uint8_t*)pHeader2)+ sizeof(SCompressDecompressHeader);

	pItem = a_list->first;
	while(pItem){
		unLenFromItem = LEN_FROM_ITEM(pItem->item);
		memcpy(pCurrent2,pItem->item,unLenFromItem);
		pCurrent2 += unLenFromItem;
		pItem = pItem->next;
	}

	strm.avail_in = a_headerSize;
	strm.next_in = (Bytef*)pHeader2;
	nReturn=ZlibCompressBufferToFile(&strm,0,out, DEF_CHUNK_SIZE,a_dest);
	if(nReturn){goto returnPoint;}

	pItem = a_list->first;
	while (pItem) {
		if(pItem->file){
			strm.avail_in = DEF_CHUNK_SIZE;
			nReturn = ZlibCompressFileRawEx(&strm,pItem->file,a_dest,in, DEF_CHUNK_SIZE,out, DEF_CHUNK_SIZE,0);
			if (nReturn) { goto returnPoint; }
		}
		pItem = pItem->next;
	}

	strm.avail_in = LAST_STRING_IN_THE_FILE_LEN;
	strm.next_in = (Bytef*)LAST_STRING_IN_THE_FILE;
	nReturn = ZlibCompressBufferToFile(&strm,1, out, DEF_CHUNK_SIZE, a_dest);
	if (nReturn) { goto returnPoint; }

	nReturn = 0;
returnPoint:

	if (pHeader2) { DestroyCompressDecompressHeader(pHeader2); }
	(void)deflateEnd(&strm); // if not inited then this line will not run

	return nReturn;
}


static int FilterFuncDefault(const char*, void*, const DirIterFileData*) { return 0; }


int ZlibCompressFolder(const char* a_directoryPath, FILE *a_dest, int a_level, TypeFilter a_filter, void* a_userData)
{
	SFileItemList *pItem,*pItemNext;
	SUserDataForDirCompress aData;
	int nReturn;

	memset(&aData, 0, sizeof(SUserDataForDirCompress));

	aData.offsetToTakeRoot = (uint16_t)strlen(a_directoryPath)+1;
	aData.funcFilter = a_filter ? a_filter : &FilterFuncDefault;
	aData.userData = a_userData;
	IterateOverDirectoryFilesNoRecurse(a_directoryPath, &DirectoryIterator, &aData);

	aData.headerSize += sizeof(SCompressDecompressHeader);
	nReturn = ZlibCompressFolderEx(&aData.list, aData.headerSize, aData.numberOfItems, a_dest, a_level);

	pItem = aData.list.first;
	while(pItem){
		pItemNext = pItem->next;
		if(pItem->file){fclose(pItem->file);}
		if(pItem->item){free(pItem->item);}
		free(pItem);
		pItem = pItemNext;
	}

	return nReturn;
}


/* /////////////////////////////////////////////////////////////////////////////////////////////////// */
ZLIBANDTLS_EXPORT SFileItemList* ZlibCreateListItemCompress(
	const char* a_cpcFileName,
	uint16_t a_strLen, int a_isDir, const char* CPPUTILS_ARG_NONULL a_fullPath)
{
	SFileItemList * pItem=CPPUTILS_NULL;
	struct stat fStat;
	uint16_t strLenPlus1= a_strLen+1,strLenPlus1Normalized;

	pItem = (SFileItemList*)malloc(sizeof(SFileItemList));
	if(!pItem){ return pItem;}
	pItem->file = NULL;
	pItem->next = NULL;
	
	strLenPlus1Normalized = ((strLenPlus1 / 8) + 1) * 8;
	pItem->item = (SFileItem*)malloc(sizeof(SFileItem) + strLenPlus1Normalized);
	if (!pItem->item) { free(pItem); return NULL; }

	pItem->item->fileNameLen = strLenPlus1Normalized;
	pItem->item->folderNum = 0;
	memcpy(ITEM_NAME(pItem->item), a_cpcFileName, a_strLen);
	*(ITEM_NAME(pItem->item)+ a_strLen)=0;

	if (a_isDir) {
		pItem->item->fileSize = 0;
	}
	else {
		pItem->file = fopen_zlibandtls(a_fullPath, "rb");
		if (!pItem->file) { goto errorPoint; }
		if (fstat(fileno_zlibandtls(pItem->file), &fStat)) { goto errorPoint; }
		pItem->item->fileSize = (uint32_t)fStat.st_size;
	}

	return pItem;

errorPoint:
	free(pItem->item);
	free(pItem); 
	return NULL;
}


static int DirectoryIterator(const char* a_dir, void* a_pUd, const DirIterFileData* a_pFileData)
{
	const char* cpcFileName;
	SUserDataForDirCompress* pUserData = (SUserDataForDirCompress*)a_pUd;
	SFileItemList* pItem = NULL;
	uint16_t strLen;
	char vcStrFilePath[ZLIBANDTLS_MAX_PATH];

	if ((*pUserData->funcFilter)(a_dir,pUserData->userData, a_pFileData)) {
		return 0;
	}

	snprintf_zlibandtls(vcStrFilePath, ZLIBANDTLS_MAX_PATH, "%s\\%s", a_dir, a_pFileData->pFileName);
	cpcFileName = vcStrFilePath + pUserData->offsetToTakeRoot;
	strLen = (uint16_t)strlen(cpcFileName);

	pItem=ZlibCreateListItemCompress(cpcFileName,strLen, a_pFileData->isDir,vcStrFilePath);
	if(!pItem){return ENOMEM;}
	pItem->item->folderNum = pUserData->lastFolderNum;

	if(pUserData->list.last){ pUserData->list.last->next=pItem;}
	else /*if(!pData->first)*/{ pUserData->list.first = pItem;}
	pUserData->list.last = pItem;

	++pUserData->numberOfItems;
	pUserData->headerSize += LEN_FROM_ITEM(pItem->item);
	if(a_pFileData->isDir){++pUserData->lastFolderNum;}
	
	return 0;
}


static int CallbackForCompressToFile(const void*a_buffer, int a_bufLen, void*a_userData)
{
	FILE* fpOutFile = (FILE*)a_userData;

    if (((int)fwrite(a_buffer,1, a_bufLen,fpOutFile)) != a_bufLen || ferror(fpOutFile)) {
		return Z_ERRNO;
	}

	return 0;
}


CPPUTILS_END_C
