//
// file:			zlibwt_decompress_data.h
// path:			include/zlib_with_tools/zlibwt_decompress_data.h
// created on:		2023 Feb 10
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_DECOMPRESS_DATA_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_DECOMPRESS_DATA_H

#include <zlib_with_tools/export_symbols.h>
#include <zlib_with_tools/zlibwt_compress_decompress_common.h>
#include <directory_iterator/directory_iterator.h>
#include <stddef.h>


CPPUTILS_BEGIN_C

typedef void (*ZlibWtTypeLLDecompressCallback)(const void* buffer, size_t bufLen, void* userData);
typedef void (*ZlibWtTypeDecompressCallback)(const void* buffer, size_t bufLen, void* userData, const DirIterFileData* a_pFileData, const struct SFileItem* a_pExtraData);

CPPUTILS_END_C



#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_COMPRESSION_DATA_H
