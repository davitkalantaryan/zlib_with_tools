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


CPPUTILS_BEGIN_C


enum CPPUTILS_DLL_PRIVATE TypeOfCompressedContent0000{
	CompressedContentNone = 0,
	CompressedContentDirectory = 1,
	CompressedContentFile = 2
};


// padding is 8
struct CPPUTILS_DLL_PRIVATE SFileItem0000{
	uint64_t fileSize : 48;
	uint64_t fileNameLenNorm : 12;
	uint64_t isDir : 1;
	uint64_t reserved01 : 3;
};


struct CPPUTILS_DLL_PRIVATE SCompressDecompressHeader0000{
	uint32_t version;
	uint32_t typeOfCompressedContent;
	uint32_t vReserved[14];
};



CPPUTILS_END_C


#endif  // #ifndef ZLIB_WITH_TOOLS_SRC_INCLUDE_PRIVATE_ZLIB_WITH_TOOLS_ZLIBWT_COMPRESS_DECOMPRESS_COMMON_VERSION_0000_H
