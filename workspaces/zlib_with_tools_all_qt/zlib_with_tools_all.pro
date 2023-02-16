#
# file:				directory_iterator_all.pro
# path:				workspaces/directory_iterator_all_qt/directory_iterator_all.pro
# created on:		2023 Jan 10
# Created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#

TEMPLATE = subdirs
#CONFIG += ordered

greaterThan(QT_MAJOR_VERSION, 4):QT += widgets

repositoryRoot = $${PWD}/../..

SUBDIRS	+=	"$${repositoryRoot}/prj/tests/zlibwt_hl_test_qt/zlibwt_hl_test.pro"
SUBDIRS	+=	"$${repositoryRoot}/prj/tests/zlibwt_ll_test_qt/zlibwt_ll_test.pro"


OTHER_FILES += $$files($${repositoryRoot}/docs/*.md,true)
OTHER_FILES += $$files($${repositoryRoot}/docs/*.txt,true)
OTHER_FILES += $$files($${repositoryRoot}/scripts/*.sh,true)
OTHER_FILES += $$files($${repositoryRoot}/scripts/*.bat,true)
OTHER_FILES += $$files($${repositoryRoot}/scripts/.raw/*.sh,true)
OTHER_FILES += $$files($${repositoryRoot}/scripts/.raw/*.bat,true)
OTHER_FILES += $$files($${repositoryRoot}/scripts/.cicd/*.sh,true)
OTHER_FILES += $$files($${repositoryRoot}/scripts/.cicd/*.bat,true)
OTHER_FILES += $$files($${repositoryRoot}/.github/*.yml,true)

OTHER_FILES	+=	\
        $${repositoryRoot}/.gitattributes			\
        $${repositoryRoot}/.gitignore				\
        $${repositoryRoot}/LICENSE				\
        $${repositoryRoot}/README.md
