#
# repo:			zlib_with_tools
# name:			resolve_common.pri
# path:			${repositoryRoot}/prj/common/common_qt/resolve_common.pri
# created on:	2023 Jun 28
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)  
# usage:		Use this qt include file to calculate some platform specific stuff
#


message("!!! $${PWD}/resolve_common.pri")
zlibWithToolsResolveCommonIncluded = 1

isEmpty( zlibWithToolsRepositoryRoot ) {
        zlibWithToolsRepositoryRoot = $$(zlibWithToolsRepositoryRoot)
        isEmpty(zlibWithToolsRepositoryRoot) {
            zlibWithToolsRepositoryRoot = $${PWD}/../../..
        }
}

isEmpty( repositoryRoot ) {
        repositoryRoot = $$(repositoryRoot)
        isEmpty(repositoryRoot) {
            repositoryRoot = $${zlibWithToolsRepositoryRoot}
        }
}

isEmpty(artifactRoot) {
    artifactRoot = $$(artifactRoot)
    isEmpty(artifactRoot) {
        artifactRoot = $${repositoryRoot}
    }
}

isEmpty( directoryIteratorRepoRoot ) {
        directoryIteratorRepoRoot = $$(directoryIteratorRepoRoot)
	isEmpty(directoryIteratorRepoRoot) {
	    directoryIteratorRepoRoot = $${zlibWithToolsRepositoryRoot}/contrib/directory_iterator
        }
}

isEmpty( qtutilsRepoRoot ) {
        qtutilsRepoRoot = $$(qtutilsRepoRoot)
	isEmpty(qtutilsRepoRoot) {
	    qtutilsRepoRoot = $${zlibWithToolsRepositoryRoot}/contrib/qtutils
	}
}

isEmpty( systemkRepositoryRoot ) {
        systemkRepositoryRoot = $$(systemkRepositoryRoot)
        isEmpty(systemkRepositoryRoot) {
            systemkRepositoryRoot = $${zlibWithToolsRepositoryRoot}/contrib/system
        }
}
