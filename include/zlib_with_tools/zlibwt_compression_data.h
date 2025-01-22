//
// file:			zlibwt_compression_data.h
// path:			include/zlib_with_tools/zlibwt_compression_data.h
// created on:		2023 Feb 08
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_COMPRESSION_DATA_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_COMPRESSION_DATA_H

#include <zlib_with_tools/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <stddef.h>
#include <cinternal/undisable_compiler_warnings.h>


CPPUTILS_BEGIN_C

typedef void (*ZlibWtTypeCompressCallback)(const void* buffer, size_t bufLen, void* userData);

CPPUTILS_END_C



#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_ZLIB_COMPRESSION_DATA_H
