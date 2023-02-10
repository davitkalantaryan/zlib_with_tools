//
// file:			zlibwt_compress_decompress_common_version_0000.h
// path:			src/include/private/zlib_with_tools/zlibwt_compress_decompress_common_version_0000.h
// created on:		2023 Feb 07
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_SRC_INCLUDE_PRIVATE_ZLIB_WITH_TOOLS_ZLIBWT_COMPRESS_DECOMPRESS_COMMON_VERSION_0000_H
#define ZLIB_WITH_TOOLS_SRC_INCLUDE_PRIVATE_ZLIB_WITH_TOOLS_ZLIBWT_COMPRESS_DECOMPRESS_COMMON_VERSION_0000_H

#include <zlib_with_tools/export_symbols.h>
#include <stdint.h>

#define ZLIBWT_DIR_CONTENT_FILE         0 
#define ZLIBWT_DIR_CONTENT_DIR_START    1
#define ZLIBWT_DIR_CONTENT_DIR_END      2

CPPUTILS_BEGIN_C


// size=32 B, padding=8 B
struct CPPUTILS_DLL_PRIVATE SFileItem0000{
    uint64_t fileSize;
    uint64_t fileSizeNorm;
    uint32_t fileNameLen;
    uint32_t fileNameLenNorm;
    uint32_t mode;
    uint8_t  contentType;
    uint8_t  reserved01[3];
    uint8_t  reserved02[8];
};


// size=64 B, padding=8 B
struct CPPUTILS_DLL_PRIVATE SCompressDecompressHeader0000{
	uint32_t version;
	uint32_t typeOfCompressedContent;
    uint32_t hasMode;
	uint32_t vReserved[13];
};



CPPUTILS_END_C


#endif  // #ifndef ZLIB_WITH_TOOLS_SRC_INCLUDE_PRIVATE_ZLIB_WITH_TOOLS_ZLIBWT_COMPRESS_DECOMPRESS_COMMON_VERSION_0000_H
