//
// file:			io_zlibandtls.h
// path:			include/zlib_with_tools/utils/io_zlibandtls.h
// created on:		2023 Feb 14
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_UTILS_IO_ZLIBANDTLS_H
#define ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_UTILS_IO_ZLIBANDTLS_H

#include <zlib_with_tools/export_symbols.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef _MSC_VER
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif


#ifdef _MSC_VER
#define sopen_zlibandtls(_pfh,_filename,_oflag,_pmode)	_sopen_s(_pfh,_filename,_oflag,_SH_DENYNO,_pmode)
#define mkdir_zlibandtls(_path,_mode)					_mkdir(_path)
#define read_zlibandtls(_fd,_buffer,_bufSize)			_read(_fd,_buffer,CPPUTILS_STATIC_CAST(unsigned int,_bufSize))
#define write_zlibandtls(_fd,_buffer,_bufSize)			_write(_fd,_buffer,CPPUTILS_STATIC_CAST(unsigned int,_bufSize))
#define close_zlibandtls								_close
#else
#define sopen_zlibandtls(_pfh,_filename,_oflag,_pmode)	*(_pfh) = open(_filename,_oflag,_pmode)
#define mkdir_zlibandtls								mkdir
#define read_zlibandtls									read
#define write_zlibandtls								write
#define close_zlibandtls								close
#endif


#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_UTILS_IO_ZLIBANDTLS_H
