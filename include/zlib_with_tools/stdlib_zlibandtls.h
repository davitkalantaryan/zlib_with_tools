//
// file:			stdlib_zltls.h
// path:			include/zlib_with_tools/stdlib_zltls.h
// created on:		2023 Feb 03
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_STDLIB_ZLIBANDTLS_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_STDLIB_ZLIBANDTLS_H

#include <zlib_with_tools/export_symbols.h>
#include <stdlib.h>


#ifdef _MSC_VER
#include <share.h>
#define fopen_zlibandtls(_filename,_mode)	_fsopen(_filename,_mode,_SH_DENYRW)
#else
#define fopen_zlibandtls					fopen
#endif

#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_STDLIB_ZLIBANDTLS_H
