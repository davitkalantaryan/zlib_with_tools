//
// file:			string_zlibandtls.h
// path:			include/zlib_with_tools/string_zlibandtls.h
// created on:		2023 Feb 03
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_STRING_ZLIBANDTLS_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_STRING_ZLIBANDTLS_H

#include <zlib_with_tools/export_symbols.h>
#include <string.h>

#ifdef _MSC_VER
#define strcpy_zlibandtls(_dest,_src)	strcpy_s(_dest,1024,_src)
#else
#define strcpy_zlibandtls				strcpy
#endif

#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_STRING_ZLIBANDTLS_H
