#!/bin/bash

# This script can be used to compile login_server for FocusT
# export QT_ROOT_DIR=~/Qt/6.8.0

# https://intoli.com/blog/exit-on-errors-in-bash-scripts/
# exit when any command fails
set -e
trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG

if [ -z "$QT_ROOT_DIR" ]; then
    echo "QT_ROOT_DIR environment variable is not set. call: 'export QT_ROOT_DIR=~/Qt/6.8.0'"
    exit 1
fi

targetNameBase=libzlib_st
ConfigurationIn=Debug

currentDirectory=`pwd`
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
cd ..
repositoryRoot=`pwd`
echo repositoryRoot=$repositoryRoot
proFileDir="${repositoryRoot}/prj/core/prj/core/zlib_st_qt"

${scriptDirectory}/.raw/_unix_compile_all.sh "${targetNameBase}" "${proFileDir}" "${ConfigurationIn}"
