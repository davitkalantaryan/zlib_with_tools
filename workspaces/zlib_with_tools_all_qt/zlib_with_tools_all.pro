#
# repo:			zlib_with_tool
# file:				zlib_with_tool_all.pro
# path:				workspaces/zlib_with_tool_all_qt/zlib_with_tool_all.pro
# created on:		2023 Jan 10
# Created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message("!!! $${_PRO_FILE_}")

TEMPLATE = subdirs
CONFIG += ordered # todo: get rid of this

include ( "$${PWD}/../../prj/common/common_qt/flagsandsys_common.pri" )

greaterThan(QT_MAJOR_VERSION, 4):QT += widgets


zlib                    = $${zlibWithToolsRepositoryRoot}/prj/core/zlib_qt/zlib.pro
zlib_st                 = $${zlibWithToolsRepositoryRoot}/prj/core/zlib_st_qt/zlib_st.pro
self_extract            = $${zlibWithToolsRepositoryRoot}/prj/tools/self_extract_qt/self_extract.pro
compressdir             = $${zlibWithToolsRepositoryRoot}/prj/tools/compressdir_qt/compressdir.pro
hello_world_test        = $${zlibWithToolsRepositoryRoot}/prj/tests/hello_world_test_qt/hello_world_test.pro
zlibwt_hl_test          = $${zlibWithToolsRepositoryRoot}/prj/tests/zlibwt_hl_test_qt/zlibwt_hl_test.pro
zlibwt_ll_test          = $${zlibWithToolsRepositoryRoot}/prj/tests/zlibwt_ll_test_qt/zlibwt_ll_test.pro


SUBDIRS		+=	"$${zlibWithToolsRepositoryRoot}/prj/tests/any_quick_test_qt/any_quick_test.pro"
SUBDIRS		+=	"$${zlibWithToolsRepositoryRoot}/prj/tests/zlibwt_unit_test_mult/zlibwt_unit_test.pro"


SUBDIRS +=  \
    $$zlib                \
    $$zlib_st             \
    $$self_extract        \
    $$compressdir         \
    $$hello_world_test    \
    $$zlibwt_hl_test      \
    $$zlibwt_ll_test


$${self_extract}.depends = $${zlib_st}
$${compressdir}.depends = $${zlib}
$${zlibwt_hl_test}.depends = $${zlib}
$${zlibwt_ll_test}.depends = $${zlib}


OTHER_FILES += $$files($${zlibWithToolsRepositoryRoot}/docs/*.md,true)
OTHER_FILES += $$files($${zlibWithToolsRepositoryRoot}/docs/*.txt,true)
OTHER_FILES += $$files($${zlibWithToolsRepositoryRoot}/scripts/*.sh,true)
OTHER_FILES += $$files($${zlibWithToolsRepositoryRoot}/scripts/*.bat,true)
OTHER_FILES += $$files($${zlibWithToolsRepositoryRoot}/scripts/.raw/*.sh,true)
OTHER_FILES += $$files($${zlibWithToolsRepositoryRoot}/scripts/.raw/*.bat,true)
OTHER_FILES += $$files($${zlibWithToolsRepositoryRoot}/scripts/.cicd/*.sh,true)
OTHER_FILES += $$files($${zlibWithToolsRepositoryRoot}/scripts/.cicd/*.bat,true)
OTHER_FILES += $$files($${zlibWithToolsRepositoryRoot}/.github/*.yml,true)

OTHER_FILES	+=	\
        $${zlibWithToolsRepositoryRoot}/.gitattributes			\
	$${zlibWithToolsRepositoryRoot}/.gitignore				\
	$${zlibWithToolsRepositoryRoot}/.gitmodules				\
	$${zlibWithToolsRepositoryRoot}/LICENSE				\
	$${zlibWithToolsRepositoryRoot}/README.md
