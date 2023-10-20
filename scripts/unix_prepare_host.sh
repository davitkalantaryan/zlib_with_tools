#!/bin/bash

# https://intoli.com/blog/exit-on-errors-in-bash-scripts/
# exit when any command fails
set -e

# keep track of the last executed command
trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG

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
cd ..
zlibWithToolsRepositoryRoot=`pwd`
echo zlibWithToolsRepositoryRoot=$zlibWithToolsRepositoryRoot

git submodule sync --recursive
git submodule update --init --recursive

./contrib/directory_iterator/scripts/unix_prepare_host.sh
./contrib/qtutils/scripts/unix_prepare_host.sh
./contrib/system/scripts/unix_prepare_host.sh
