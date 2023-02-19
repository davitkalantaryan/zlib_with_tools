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
#define ZLIBWT_O_WRONLY									_O_WRONLY
#define ZLIBWT_O_CREAT									_O_CREAT
#define ZLIBWT_O_BINARY									_O_BINARY
#define ZLIBWT_FILE_DELIM                               "\\"

#define sopen_zlibandtls(_pfh,_filename,_oflag,_pmode)	_sopen_s(_pfh,_filename,_oflag,_SH_DENYNO,_pmode)
#define mkdir_zlibandtls(_path,_mode)					_mkdir(_path)
#define read_zlibandtls(_fd,_buffer,_bufSize)			_read(_fd,_buffer,CPPUTILS_STATIC_CAST(unsigned int,_bufSize))
#define write_zlibandtls(_fd,_buffer,_bufSize)			_write(_fd,_buffer,CPPUTILS_STATIC_CAST(unsigned int,_bufSize))
#define close_zlibandtls								_close
#define lseek_zlibandtls(_fd,_off,_org)                 _lseek(_fd,CPPUTILS_STATIC_CAST(long,_off),_org)
#define mode_fo_zlibandtls(_mode)						((int)((_S_IREAD | _S_IWRITE)& (_mode)))
#else
#define ZLIBWT_O_WRONLY									O_WRONLY
#define ZLIBWT_O_CREAT									O_CREAT
#define ZLIBWT_O_BINARY									0
#define ZLIBWT_FILE_DELIM                               "/"

#define sopen_zlibandtls(_pfh,_filename,_oflag,_pmode)	*(_pfh) = open(_filename,_oflag,_pmode)
#define mkdir_zlibandtls								mkdir
#define read_zlibandtls									read
#define write_zlibandtls								write
#define close_zlibandtls								close
#define lseek_zlibandtls(_fd,_off,_org)                 lseek(_fd,CPPUTILS_STATIC_CAST(off_t,_off),_org)
#define mode_fo_zlibandtls(_mode)						((S_IRWXU | S_IRWXG | S_IRWXO)&(_mode))
#endif


#endif  // #ifndef ZLIB_WITH_TOOLS_INCLUDE_ZLIB_WITH_TOOLS_UTILS_IO_ZLIBANDTLS_H
