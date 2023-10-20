#!/bin/bash

# script to prepare developer host, to work with the code on this repo

#scriptFileFullPath=`readlink -f ${0}`
#scriptDirectory=`dirname ${scriptFileFullPath}`
#cd ${scriptDirectory}/../..
#repositoryRoot=`pwd`

# in mac os above short calculation will not work
# also realpath utilit is missing in mac

scriptDirectory=`dirname "${0}"`
scriptFileName=`basename "${0}"`
cd "${scriptDirectory}"
fileOrigin=`readlink "${scriptFileName}"` || :
while [ ! -z "${fileOrigin}" ]
do
	scriptDirectory=`dirname "${fileOrigin}"`
	scriptFileName=`basename "${fileOrigin}"`
	cd "${scriptDirectory}"
	fileOrigin=`readlink "${scriptFileName}"`  || :
done
scriptDirectory=`pwd`

source ${scriptDirectory}/unix_source_per_session.sh ${scriptDirectory}/unix_source_per_session.sh ${scriptDirectory}/unix_source_per_session.sh

cd ${zlibWithToolsRepositoryRoot}/workspaces/zlib_with_tools_all_mkfl
unset CPPUTILS_DEBUG
make -f zlib_with_tools_all.unix.Makefile CPPUTILS_RELEASE=1
unset CPPUTILS_RELEASE
make -f zlib_with_tools_all.unix.Makefile CPPUTILS_DEBUG=1
