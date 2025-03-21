//
// file:			string_zlibandtls.h
// path:			include/zlib_with_tools/string_zlibandtls.h
// created on:		2023 Feb 03
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_UTILS_STRING_ZLIBANDTLS_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_UTILS_STRING_ZLIBANDTLS_H

#include <zlib_with_tools/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <string.h>
#include <cinternal/undisable_compiler_warnings.h>

#ifdef _MSC_VER
#define strcpy_zlibandtls(_dest,_src)			strcpy_s(_dest,1024,_src)
#define strncpy_zlibandtls(_dest,_src,_count)	strncpy_s(_dest,_count,_src,_count)
#define strdup_zlibandtls						_strdup
#else
#define strcpy_zlibandtls						strcpy
#define strncpy_zlibandtls						strncpy
#define strdup_zlibandtls						strdup
#endif

#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_UTILS_STRING_ZLIBANDTLS_H
