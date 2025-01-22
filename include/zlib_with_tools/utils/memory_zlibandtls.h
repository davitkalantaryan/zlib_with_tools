//
// file:			memory_zlibandtls.h
// path:			include/zlib_with_tools/utils/memory_zlibandtls.h
// created on:		2023 Feb 14
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_UTILS_MEMORY_ZLIBANDTLS_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_UTILS_MEMORY_ZLIBANDTLS_H

#include <zlib_with_tools/export_symbols.h>

#include <cinternal/disable_compiler_warnings.h>
#ifdef _WIN32
#include <malloc.h>
#define alloca_zlibandtls	_alloca
#else
#include <alloca.h>
#define alloca_zlibandtls	alloca
#endif
#include <cinternal/undisable_compiler_warnings.h>


#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_UTILS_MEMORY_ZLIBANDTLS_H
