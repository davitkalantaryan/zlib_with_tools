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

DEFINES += ZLIB_CONST

INCLUDEPATH += $${PWD}/../../../contrib/qtutils/contrib/cpputils/include
INCLUDEPATH += $${PWD}/../../../contrib/qtutils/contrib/cpputils/contrib/cinternal/include
INCLUDEPATH += $${PWD}/../../../contrib/directory_iterator/include


SOURCES		+=	\
	\
        $${PWD}/../../../src/tests/main_zlib_test.cpp                                               \
        $${PWD}/../../../src/core/zlibwt_ll_compression_routines.cpp                                \
        $${PWD}/../../../contrib/qtutils/contrib/cpputils/src/core/cpputils_hashtbl.cpp             \
        $${PWD}/../../../contrib/directory_iterator/src/core/diriter/directory_iterator_unix.c

HEADERS += $$files($${repositoryRoot}/include/*.hpp,true)
HEADERS += $$files($${repositoryRoot}/include/*.h,true)
HEADERS += $$files($${repositoryRoot}/src/include/*.hpp,true)
HEADERS += $$files($${repositoryRoot}/src/include/*.h,true)

OTHER_FILES += $$files($${repositoryRoot}/src/core/*.cpp,true)
OTHER_FILES += $$files($${repositoryRoot}/src/core/*.c,true)
