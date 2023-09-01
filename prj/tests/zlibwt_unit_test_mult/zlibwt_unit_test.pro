#
# file:			zlibwt_unit_test.pro
# path:			prj/tests/zlibwt_unit_test_mult/zlibwt_unit_test.pro
# created on:	2021 Mar 27
# created by:	Davit Kalantaryan
#

message("!!! $${_PRO_FILE_}")
include ( "$${PWD}/../../common/common_qt/flagsandsys_common_private.pri" )
DESTDIR     = "$${artifactRoot}/sys/$${CODENAME}/$$CONFIGURATION/test"

QT -= gui
QT -= core
QT -= widgets
CONFIG -= qt
CONFIG += console

LIBS += -lzlib

win32{
	LIBS += -lWs2_32
} else {
	LIBS += -pthread
	LIBS += -ldl
}


DEFINES += CINTERNAL_UNIT_TEST_USE_GTEST_LIKE_MACROSES


SOURCES	+= $$files($${zlibWithToolsRepositoryRoot}/src/core/*.c,true)
SOURCES	+= $$files($${zlibWithToolsRepositoryRoot}/src/core/*.cpp,true)
SOURCES += $$files($${zlibWithToolsRepositoryRoot}/src/tests/unit_test/*.cpp)
SOURCES += $$files($${directoryIteratorRepoRoot}/src/core/*.c,true)

SOURCES +=  \
    "$${cinternalRepoRoot}/src/core/cinternal_core_logger.c"				\
    "$${cinternalRepoRoot}/src/core/cinternal_core_unit_test_checks.c"			\
    "$${cinternalRepoRoot}/src/core/cinternal_core_unit_test_main.c"



HEADERS =  $$files($${zlibWithToolsRepositoryRoot}/include/*.h,true)
HEADERS += $$files($${zlibWithToolsRepositoryRoot}/include/*.hpp,true)

OTHER_FILES += $$files($${PWD}/*.Makefile)
