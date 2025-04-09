#
# repo:			zlib_with_tool
# name:			flagsandsys_common.pri
# path:			prj/common/common_qt/flagsandsys_common.pri
# created on:   2023 Jun 21
# created by:   Davit Kalantaryan (davit.kalantaryan@desy.de)
# usage:		Use this qt include file to calculate some platform specific stuff
#


message("!!! $${PWD}/flagsandsys_common.pri")

isEmpty(zlibWithToolsFlagsAndSysCommonIncluded){
    zlibWithToolsFlagsAndSysCommonIncluded = 1

    zlibWithToolsRepositoryRoot = $${PWD}/../../..

    isEmpty(artifactRoot) {
        artifactRoot = $$(artifactRoot)
        isEmpty(artifactRoot) {
            artifactRoot = $${zlibWithToolsRepositoryRoot}
        }
    }

    include("$${zlibWithToolsRepositoryRoot}/contrib/directory_iterator/prj/common/common_qt/flagsandsys_common.pri")
    include("$${zlibWithToolsRepositoryRoot}/contrib/qtutils/prj/common/common_qt/flagsandsys_common.pri")
    include("$${zlibWithToolsRepositoryRoot}/contrib/system/prj/common/common_qt/flagsandsys_common.pri")

    INCLUDEPATH += $${zlibWithToolsRepositoryRoot}/include
    INCLUDEPATH += $${zlibWithToolsRepositoryRoot}/contrib/zlib

    exists($${zlibWithToolsRepositoryRoot}/sys/$${CODENAME}/$$CONFIGURATION/lib) {
        LIBS += -L$${zlibWithToolsRepositoryRoot}/sys/$${CODENAME}/$$CONFIGURATION/lib
    }
    exists($${zlibWithToolsRepositoryRoot}/sys/$${CODENAME}/$$CONFIGURATION/tlib) {
        LIBS += -L$${zlibWithToolsRepositoryRoot}/sys/$${CODENAME}/$$CONFIGURATION/tlib
    }

    OTHER_FILES += $$files($${PWD}/../common_mkfl/*.Makefile,true)
}
