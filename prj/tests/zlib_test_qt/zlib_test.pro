#   
# file:			zlib_test.pro  
# created on:	2010 May 27 
# created by:	D. Kalantaryan (davit.kalantaryan@gmail.com)  
#  


message("file:  zlib_test.pro  ")
include($${PWD}/../../common/common_qt/zlib.pri)
repositoryRoot = $${PWD}/../../..
include($${repositoryRoot}/contrib/qtutils/contrib/cpputils/prj/common/common_qt/sys_common.pri)
DESTDIR     = $${artifactRoot}/$${SYSTEM_PATH}/$$CONFIGURATION/test
message(DESTDIR is $$DESTDIR)

INCLUDEPATH += $${PWD}/../../../contrib/qtutils/contrib/cpputils/include
INCLUDEPATH += $${PWD}/../../../contrib/qtutils/contrib/cpputils/contrib/cinternal/include
INCLUDEPATH += $${PWD}/../../../contrib/directory_iterator/include

SOURCES		+=	\
	\
        $${PWD}/../../../src/tests/main_zlib_test.cpp                                               \
        $${PWD}/../../../src/core/zlib_decompress_routines.cpp                                      \
        $${PWD}/../../../src/core/zlib_compression_routines.cpp                                     \
        $${PWD}/../../../src/core/zlib_compress_decompress_common.cpp                               \
        $${PWD}/../../../contrib/qtutils/contrib/cpputils/src/core/cpputils_hashtbl.cpp             \
        $${PWD}/../../../contrib/directory_iterator/src/core/diriter/directory_iterator_unix.c
