//
// file:			export_symbols.h
// path:			include/zlib_with_tools/export_symbols.h
// created on:		2023 Feb 03
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_EXPORT_SYMBOLS_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_EXPORT_SYMBOLS_H

#include <cinternal/internal_header.h>

#define ZLIBWT_VERSION      0

#ifndef ZLIBANDTLS_EXPORT
#if defined(ZLIBANDTLS_COMPILING_SHARED_LIB)
#define ZLIBANDTLS_EXPORT CPPUTILS_DLL_PUBLIC
#elif defined(ZLIBANDTLS_USING_STATIC_LIB_OR_OBJECTS)
#define ZLIBANDTLS_EXPORT
#elif defined(ZLIBANDTLS_LOAD_FROM_DLL)
#define ZLIBANDTLS_EXPORT CPPUTILS_IMPORT_FROM_DLL
#else
#define ZLIBANDTLS_EXPORT CPPUTILS_DLL_PRIVATE
#endif
#endif


#ifndef ZLIBANDTLS_LL_EXPORT
#if defined(ZLIBANDTLS_LL_COMPILING_SHARED_LIB)
#define ZLIBANDTLS_LL_EXPORT CPPUTILS_DLL_PUBLIC
#elif defined(ZLIBANDTLS_LL_USING_STATIC_LIB_OR_OBJECTS)
#define ZLIBANDTLS_LL_EXPORT
#elif defined(ZLIBANDTLS_LL_LOAD_FROM_DLL)
#define ZLIBANDTLS_LL_EXPORT CPPUTILS_IMPORT_FROM_DLL
#else
#define ZLIBANDTLS_LL_EXPORT CPPUTILS_DLL_PRIVATE
#endif
#endif

#ifndef ZLIB_CONST
#define ZLIB_CONST
#endif

#if defined(_MSC_VER) && !defined(_CPPUNWIND)
#define ZLIBWT_TRY			__try
#define ZLIBWT_CATCH()		__except (EXCEPTION_EXECUTE_HANDLER)
#define ZLIBWT_THROW		throw
#else
#define ZLIBWT_TRY			try
#define ZLIBWT_CATCH()		catch(...)
#define ZLIBWT_THROW		throw
#endif


#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_EXPORT_SYMBOLS_H
