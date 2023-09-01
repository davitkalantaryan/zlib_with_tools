#   
# file:		zlibwt_ll_test.pro
# created on:	2010 May 27 
# created by:	D. Kalantaryan (davit.kalantaryan@gmail.com)  
#  


message("!!! $${_PRO_FILE_}")
include ( "$${PWD}/../../common/common_qt/flagsandsys_common_private.pri" )
DESTDIR     = "$${artifactRoot}/sys/$${CODENAME}/$$CONFIGURATION/test"

QT -= core
QT -= gui
CONFIG -= qt
CONFIG += console

DEFINES += ZLIB_CONST


LIBS += -L$${DESTDIR}/../lib
LIBS += -lzlib


SOURCES		+=	\
	\
	$${PWD}/../../../src/tests/main_zlibwt_ll_test.cpp					\
	$${PWD}/../../../src/core/zlibwt_ll_compression_routines.c				\
	$${PWD}/../../../src/core/zlibwt_compression_routines.c					\
	$${PWD}/../../../src/core/zlibwt_decompress_routines.c					\
	$${PWD}/../../../src/core/zlibwt_decompress_routines_version_0000.c			\
	$${PWD}/../../../src/core/zlibwt_ll_decompress_routines.c				\
	$${PWD}/../../../contrib/directory_iterator/src/core/diriter/directory_iterator_unix.c	\
	$${PWD}/../../../contrib/directory_iterator/src/core/diriter/directory_iterator_win.c

HEADERS += $$files($${zlibWithToolsRepositoryRoot}/include/*.hpp,true)
HEADERS += $$files($${zlibWithToolsRepositoryRoot}/include/*.h,true)
HEADERS += $$files($${zlibWithToolsRepositoryRoot}/src/include/*.hpp,true)
HEADERS += $$files($${zlibWithToolsRepositoryRoot}/src/include/*.h,true)

OTHER_FILES += $$files($${zlibWithToolsRepositoryRoot}/src/core/*.cpp,true)
OTHER_FILES += $$files($${zlibWithToolsRepositoryRoot}/src/core/*.c,true)
