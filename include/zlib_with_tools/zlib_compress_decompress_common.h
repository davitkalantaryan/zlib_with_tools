
// zlib_compress_decompress_common.h
// to include  ->  #include "zlib_compress_decompress_common.h"
// 2018 Feb 12

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_COMPRESS_DECOMPRESS_COMMON_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_COMPRESS_DECOMPRESS_COMMON_H

#include <zlib_with_tools/export_symbols.h>
#include <stdint.h>
#include <zlib.h>
#include <stdio.h>

#define ZLIB_COMPR_DECOMPR_VERSION	1

#define SIZE_OF_MAIN_HEADER			32  // B

#define LAST_STRING_IN_THE_FILE		"End"
#define LAST_STRING_IN_THE_FILE_LEN	4

#define DEF_CHUNK_SIZE				16384

#define ZLIBANDTLS_MAX_PATH		1024


CPPUTILS_BEGIN_C


enum TypeOfCompressedContent{CompressedContentDirectory,CompressedContentFile};

// padding is 8
typedef struct SFileItem{
	uint32_t fileSize;
	uint16_t folderNum, fileNameLen/* increased to 8* */;
}SFileItem;

typedef struct SCompressDecompressHeader{
	uint32_t version;
	uint32_t wholeHeaderSizeInBytes;
	uint32_t typeOfCompressedContent;
	uint32_t numberOfItems;
	uint32_t vReserved[4];
}SCompressDecompressHeader;


#define LEN_FROM_ITEM(_item)	(  sizeof(SFileItem)+(_item)->fileNameLen    )
#define ITEM_NAME(_item)		(  ((char*)(_item))+sizeof(SFileItem)  )

ZLIBANDTLS_EXPORT SCompressDecompressHeader* ZlibCreateCompressDecompressHeader(uint32_t headerSize, uint32_t typeOfCompressedContent, uint32_t numberOfItems);
ZLIBANDTLS_EXPORT SCompressDecompressHeader* ZlibCreateAndCopyComprDecomprHeader(const SCompressDecompressHeader* orgin, int a_nAll);
ZLIBANDTLS_EXPORT void DestroyCompressDecompressHeader(SCompressDecompressHeader* header);


CPPUTILS_END_C


#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_COMPRESS_DECOMPRESS_COMMON_H
