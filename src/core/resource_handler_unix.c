//
// file:			resource_handler.h
// path:			include/resource_handler/resource_handler.h
// created on:		2023 Feb 19
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <zlib_with_tools/export_symbols.h>

#ifndef _WIN32

#include <resource_handler/resource_handler.h>

CPPUTILS_BEGIN_C


ZLIBANDTLS_EXPORT int ResourceHandlerSecIconToApp(const char* a_path_to_app, const char* a_path_to_icon)
{
	(void)a_path_to_app;
	(void)a_path_to_icon;
	return 1;
}


CPPUTILS_END_C


#endif  //  #ifdef _WIN32
