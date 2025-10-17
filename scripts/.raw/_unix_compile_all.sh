#!/bin/bash

# https://intoli.com/blog/exit-on-errors-in-bash-scripts/
# exit when any command fails
set -e
trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG

if [ -z "$QT_ROOT_DIR" ]; then
    echo "QT_ROOT_DIR environment variable is not set"
    exit 1
fi

if [ $# -lt 3 ]; then
    echo "Usage: $0 libzlib_st prj/core/zlib_st_qt Debug|Release"
    exit 1
fi

targetNameBase=$1
proFileDir=$2
ConfigurationIn=$3

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
cd ../..
repositoryRoot=`pwd`
echo repositoryRoot=$repositoryRoot

if [ "$(echo "${ConfigurationIn}" | tr '[:upper:]' '[:lower:]')" = "$(echo "Debug" | tr '[:upper:]' '[:lower:]')" ]; then
    Confilguration="Debug"
    extraArgs="CONFIG-=release CONFIG+=debug"
else
    Confilguration="Release"
    extraArgs="CONFIG+=release CONFIG-=debug"
fi

#source ${repositoryRoot}/ENVIRONMENT

if [[ "$(uname)" == "Darwin" ]]; then
    lsbCode=mac
    qtTarget=macos
elif [[ "$(uname -s)" == Linux* ]]; then
    source /etc/os-release
    if [ -n "$VERSION_CODENAME" ]; then
        lsbCode="$VERSION_CODENAME"
    elif echo "$ID_LIKE" | grep -qE 'rhel|fedora|centos'; then
        lsbCode="el$(echo "$VERSION_ID" | cut -d. -f1)"
    else
        lsbCode="$(echo "$ID-$VERSION_ID" | tr '[:upper:]' '[:lower:]' | tr ' ' '-')"
    fi
    qtTarget=gcc_64
fi

echo "lsbCode=${lsbCode}"
echo "qtTarget=${qtTarget}"

cd "${currentDirectory}"
cd "${proFileDir}"
rm -rf .qmake.stash
${QT_ROOT_DIR}/${qtTarget}/bin/qmake ${extraArgs} MONITOR_NO_MEMORY_ANALYZE=1
make -f Makefile.${targetNameBase}.$lsbCode.${Confilguration}
