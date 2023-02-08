

// zlib_compression_routines.h
// to include ->  #include "zlib_compression_routines.h"
// 2018 Feb 12

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_COMPRESSION_ROUTINES_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_COMPRESSION_ROUTINES_H

#include <zlib_with_tools/export_symbols.h>
#include <zlib_with_tools/zlibwt_compression_data.h>
#include <directory_iterator/directory_iterator.h>


CPPUTILS_BEGIN_C

// if a_filter returns non zero, then file is skipped, if returns DIRITER_EXIT_ALL(=387), then compression stopped
ZLIBANDTLS_EXPORT int ZlibWtCompressDirectoryEx(const char* a_directoryPath,ZlibWtTypeCompressCallback a_clbk, TypeDirIterFunc a_filter, void* a_userData,int a_compressionLevel);

#if 0
typedef struct SFileItemList{
	SFileItem*		item;
	SFileItemList*	next;
	FILE*			file;
}SFileItemList;
typedef struct SCompressList{ SFileItemList	*first, *last; }SCompressList;

#define Init_SCompressList(_list)	do{(_list)->first=(_list)->last=NULL;}while(0)

// return 0, continues, non 0 stops
#ifndef typeCompressCallback_defined
typedef int (*typeCompressCallback)(const void*buffer,int bufLen,void*userData);
#define typeCompressCallback_defined
#endif

typedef TypeDirIterFunc TypeFilter;  // return non 0 means skip
//typedef int(*TypeFilter)(void* a_userData, const char* dir, const char* file_name, int isDir);  // 1 means skip

ZLIBANDTLS_EXPORT SFileItemList*	ZlibCreateListItemCompress(
	const char* a_cpcFileName,
	uint16_t fileNameLen,int a_isDir, const char* CPPUTILS_ARG_NONULL a_fullPath);

ZLIBANDTLS_EXPORT int ZlibCompressBufferToCallback(
	z_stream* a_strm, int a_flush,
	void* a_out, int a_outBufferSize,
	typeCompressCallback a_clbk, void* a_userData);

ZLIBANDTLS_EXPORT int ZlibCompressBufferToFile(
	z_stream* a_strm, int a_flush,
	void* a_out, int a_outBufferSize,
	FILE *a_dest);

ZLIBANDTLS_EXPORT int ZlibCompressFileRawEx(
	z_stream* a_strm,
	FILE * a_source, FILE * a_dest,
	void* a_in, int a_inBufferSize,
	void* a_out, int a_outBufferSize,
	int a_nFlushInTheEnd);

ZLIBANDTLS_EXPORT int ZlibCompressFileRaw(FILE * a_source, FILE * a_dest, int a_nCompressionLeel);
ZLIBANDTLS_EXPORT int ZlibCompressFolderEx(const SCompressList* a_list, uint16_t a_headerSize, uint16_t a_numberOfItems, FILE *a_dest, int a_level);
ZLIBANDTLS_EXPORT int ZlibCompressFolder(const char* a_directoryPath, FILE *a_dest, int a_level, TypeFilter a_filter, void* a_userData);
ZLIBANDTLS_EXPORT int ZlibCompressDiskRaw(const char* a_driveName, FILE * a_dest, int a_nCompressionLeel);

#endif


CPPUTILS_END_C



#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_COMPRESSION_ROUTINES_H
