#
# repo			:	zlib_with_tools	
# file			: 	sys_common.pri
# created on	: 	12 Feb 2017
# created by 	: 	Davit Kalantaryan (davit.kalantaryan@desy.de)
#


message("!!! $${PWD}/sys_common.pri")
zlibWithToolsSysCommonIncluded = 1

isEmpty( zlibWithToolsResolveCommonIncluded ) {
        include("$${PWD}/resolve_common.pri")
        zlibWithToolsResolveCommonIncluded = 1
}

isEmpty( directoryIteratorSysCommonIncluded ) {
        include ( "$${directoryIteratorRepoRoot}/prj/common/common_qt/sys_common.pri" )
	directoryIteratorSysCommonIncluded = 1
}

isEmpty( qtutilsSysCommonIncluded ) {
        include ( "$${qtutilsRepoRoot}/prj/common/common_qt/sys_common.pri" )
        qtutilsSysCommonIncluded = 1
}

isEmpty( systemkSysCommonIncluded ) {
        include ( "$${systemkRepositoryRoot}/prj/common/common_qt/sys_common.pri" )
        systemkSysCommonIncluded = 1
}
