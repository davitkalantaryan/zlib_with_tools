#
# repo			:	zlib_with_tools
# file			:	flags_common.pri
# created on	: 	12 Feb 2017
# created by 	: 	Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message("!!! $${PWD}/flags_common.pri")
zlibWithToolsFlagsCommonIncluded = 1

isEmpty( zlibWithToolsResolveCommonIncluded ) {
        include("$${PWD}/resolve_common.pri")
        zlibWithToolsResolveCommonIncluded = 1
}

isEmpty( directoryIteratorFlagsCommonIncluded ) {
        include ( "$${directoryIteratorRepoRoot}/prj/common/common_qt/flags_common.pri" )
	directoryIteratorFlagsCommonIncluded = 1
}

isEmpty( qtutilsFlagsCommonIncluded ) {
        include ( "$${qtutilsRepoRoot}/prj/common/common_qt/flags_common.pri" )
	qtutilsFlagsCommonIncluded = 1
}

isEmpty( systemkFlagsCommonIncluded ) {
        include ( "$${systemkRepositoryRoot}/prj/common/common_qt/flags_common.pri" )
        systemkFlagsCommonIncluded = 1
}


INCLUDEPATH += $${zlibWithToolsRepositoryRoot}/include
INCLUDEPATH += $${zlibWithToolsRepositoryRoot}/contrib/zlib
