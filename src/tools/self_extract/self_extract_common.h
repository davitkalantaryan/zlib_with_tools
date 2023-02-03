//
// file:            self_extract_common.h
// path:			src/tools/self_extract/self_extract_common.h
// created on:		2022 Oct 01
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//


#ifndef ZLIB_WITH_TOOLS_SRC_TOOLS_SELF_EXTRACT_SELF_EXTRACT_H
#define ZLIB_WITH_TOOLS_SRC_TOOLS_SELF_EXTRACT_SELF_EXTRACT_H

#include <cpputils/export_symbols.h>
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#else
#endif


CPPUTILS_BEGIN_C

struct SChildHandles{
#ifdef _WIN32
	HANDLE hChildStd_IN_Rd;
	HANDLE hChildStd_IN_Wr;
	HANDLE hChildStd_OUT_Rd;
	HANDLE hChildStd_OUT_Wr;
#else
	int v1;
#endif
};

CPPUTILS_END_C


#endif  //  #ifndef ZLIB_WITH_TOOLS_SRC_TOOLS_SELF_EXTRACT_SELF_EXTRACT_H
