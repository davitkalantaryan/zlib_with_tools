//
// file:			zlibwt_compress_decompress_common.h
// path:			src/include/private/zlib_with_tools/zlibwt_compress_decompress_common.h
// created on:		2023 Feb 07
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_SRC_INCLUDE_PRIVATE_ZLIB_WITH_TOOLS_ZLIBWT_COMPRESS_DECOMPRESS_COMMON_H
#define ZLIB_WITH_TOOLS_SRC_INCLUDE_PRIVATE_ZLIB_WITH_TOOLS_ZLIBWT_COMPRESS_DECOMPRESS_COMMON_H

#include <zlib_with_tools/export_symbols.h>
#include <private/zlib_with_tools/zlibwt_compress_decompress_common_version_0000.h>


CPPUTILS_BEGIN_C

#define ZLIBWT_MAIN_HEADER_SIZE		128  // this value will never change
#define ZLIBWT_DEF_CHUNK_SIZE		4096

#define ZLIBWT_MAKE_WARNING(...)
#define ZLIBWT_NORM_LEN(_len)		((((_len)>>3)+1)<<3)
//#define ZLIBWT_NORM_LEN(_len)		((((_len)/8)+1)*8)

struct CPPUTILS_DLL_PRIVATE SCompressDecompressHeaderUsed {
	uint32_t version;
	uint32_t typeOfCompressedContent;
};

// size=128 B, padding=8 B
struct CPPUTILS_DLL_PRIVATE SCompressDecompressHeader {
	struct SCompressDecompressHeaderUsed	used;
	uint32_t reserved01[(ZLIBWT_MAIN_HEADER_SIZE - sizeof(struct SCompressDecompressHeaderUsed))/sizeof(uint32_t)];
};


CPPUTILS_END_C


#endif  // #ifndef ZLIB_WITH_TOOLS_SRC_INCLUDE_PRIVATE_ZLIB_WITH_TOOLS_ZLIBWT_COMPRESS_DECOMPRESS_COMMON_H
