#
# repo:			qtutils
# name:			flagsandsys_common.pri
# path:			prj/common/common_qt/flagsandsys_common.pri
# created on:   2023 Jun 21
# created by:   Davit Kalantaryan (davit.kalantaryan@desy.de)
# usage:		Use this qt include file to calculate some platform specific stuff
#


message("!!! $${PWD}/flagsandsys_common.pri")

isEmpty(qtutilsFlagsAndSysCommonIncluded){
    qtutilsFlagsAndSysCommonIncluded = 1

    qtutilsRepoRoot = $${PWD}/../../..
	include("$${qtutilsRepoRoot}/contrib/emsutils/prj/common/common_qt/flagsandsys_common.pri")

    isEmpty(artifactRoot) {
        artifactRoot = $$(artifactRoot)
		isEmpty(artifactRoot) {
			artifactRoot = $${qtutilsRepoRoot}
		}
    }

    INCLUDEPATH += $${qtutilsRepoRoot}/include
}
