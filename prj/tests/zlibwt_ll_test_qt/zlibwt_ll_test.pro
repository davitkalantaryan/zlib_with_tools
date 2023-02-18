#   
# file:		zlibwt_ll_test.pro
# created on:	2010 May 27 
# created by:	D. Kalantaryan (davit.kalantaryan@gmail.com)  
#  


message("file:  zlibwt_ll_test.pro  ")
repositoryRoot = $${PWD}/../../..
include($${repositoryRoot}/contrib/qtutils/contrib/cpputils/prj/common/common_qt/sys_common.pri)
DESTDIR     = $${artifactRoot}/$${SYSTEM_PATH}/$$CONFIGURATION/test
message(DESTDIR is $$DESTDIR)

QT -= core
QT -= gui
CONFIG -= qt

DEFINES += ZLIB_CONST

INCLUDEPATH += "$${PWD}/../../../include"
INCLUDEPATH += "$${PWD}/../../../contrib/zlib"
INCLUDEPATH += $${PWD}/../../../contrib/qtutils/contrib/cpputils/include
INCLUDEPATH += $${PWD}/../../../contrib/qtutils/contrib/cpputils/contrib/cinternal/include
INCLUDEPATH += $${PWD}/../../../contrib/directory_iterator/include
INCLUDEPATH += $${PWD}/../../../src/include


LIBS += -L$${DESTDIR}/../lib
LIBS += -lzlib


SOURCES		+=	\
	\
        $${PWD}/../../../src/tests/main_zlibwt_ll_test.cpp                                          \
        $${PWD}/../../../src/core/zlibwt_ll_compression_routines.cpp                                \
        $${PWD}/../../../src/core/zlibwt_compression_routines.cpp                                   \
        $${PWD}/../../../src/core/zlibwt_decompress_routines.cpp                                    \
        $${PWD}/../../../src/core/zlibwt_decompress_routines_version_0000.cpp                       \
        $${PWD}/../../../src/core/zlibwt_ll_decompress_routines.cpp                                 \
        $${PWD}/../../../contrib/directory_iterator/src/core/diriter/directory_iterator_unix.c

HEADERS += $$files($${repositoryRoot}/include/*.hpp,true)
HEADERS += $$files($${repositoryRoot}/include/*.h,true)
HEADERS += $$files($${repositoryRoot}/src/include/*.hpp,true)
HEADERS += $$files($${repositoryRoot}/src/include/*.h,true)

OTHER_FILES += $$files($${repositoryRoot}/src/core/*.cpp,true)
OTHER_FILES += $$files($${repositoryRoot}/src/core/*.c,true)
