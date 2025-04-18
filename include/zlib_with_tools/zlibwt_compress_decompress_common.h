//
// file:			zlibwt_compress_decompress_common.h
// path:			include/zlib_with_tools/zlibwt_compress_decompress_common.h
// created on:		2023 Feb 08
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIBWT_COMPRESS_DECOMPRESS_COMMON_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIBWT_COMPRESS_DECOMPRESS_COMMON_H

#include <zlib_with_tools/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <stdint.h>
#include <cinternal/undisable_compiler_warnings.h>


CPPUTILS_BEGIN_C


#ifdef CPPUTILS_GCC_FAMILY
#pragma GCC diagnostic ignored "-Wattributes"
#endif


enum ZLIBANDTLS_LL_EXPORT TypeOfCompressedContent{
	CompressedContentNone = 0,
	CompressedContentDirectory = 1,
    CompressedContentFile = 2,
    CompressedContentBlob = 4
};


// size=40 B, padding=8 B
struct ZLIBANDTLS_LL_EXPORT SFileItem {
    uint64_t fileSize;
    uint64_t fileSizeNorm;
    uint32_t fileNameLen;
    uint32_t fileNameLenNorm;
    uint32_t mode;
    //#define ZLIBWT_DIR_CONTENT_FILE         0 
    //#define ZLIBWT_DIR_CONTENT_DIR_START    1
    //#define ZLIBWT_DIR_CONTENT_DIR_END      2
    //#define ZLIBWT_DIR_CONTENT_SINGLE_BLOB  3
    uint8_t  contentType;
    uint8_t  reserved01[3];
    uint8_t  reserved02[8];
};


CPPUTILS_END_C


#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIBWT_COMPRESS_DECOMPRESS_COMMON_H
