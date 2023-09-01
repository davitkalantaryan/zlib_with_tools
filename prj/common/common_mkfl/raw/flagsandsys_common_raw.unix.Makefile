#
# repo:		    zlib_with_tools
# file:		    flagsandsys_common_pure.unix.Makefile
# created on:	    2020 Dec 14
# created by:	    Davit Kalantaryan (davit.kalantaryan@desy.de)
# purpose:	    This file can be only as include
#

ifndef zlibWithToolsRepositoryRoot
        mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
        mkfile_dir		=  $(shell dirname $(mkfile_path))
        zlibWithToolsRepositoryRoot	:= $(shell curDir=`pwd` && cd $(mkfile_dir)/../../../.. && pwd && cd ${curDir})
endif

ifndef artifactRoot
        artifactRoot	= $(zlibWithToolsRepositoryRoot)
endif

include $(zlibWithToolsRepositoryRoot)/contrib/directory_iterator/prj/common/common_mkfl/flagsandsys_common.unix.Makefile
include $(zlibWithToolsRepositoryRoot)/contrib/qtutils/prj/common/common_mkfl/flagsandsys_common.unix.Makefile
include $(zlibWithToolsRepositoryRoot)/contrib/system/prj/common/common_mkfl/flagsandsys_common.unix.Makefile

COMMON_FLAGS	+= -I$(zlibWithToolsRepositoryRoot)/include
COMMON_FLAGS	+= -I$(zlibWithToolsRepositoryRoot)/contrib/zlib
