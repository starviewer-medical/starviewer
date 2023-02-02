#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. $SCRIPTS_ROOT/config/setup.sh

mkdir -p $STARVIEWER_BUILD_DIR_BASE
pushd $STARVIEWER_BUILD_DIR_BASE

if [[ $(uname) == 'MSYS_NT'* ]]
then
    CMAKE_GENERATOR="NMake Makefiles JOM"
else
    CMAKE_GENERATOR="Unix Makefiles"
fi

echo "Invoking CMake with the following extra arguments: $STARVIEWER_CMAKE_ARGUMENTS"
$CMAKE -G "$CMAKE_GENERATOR" $STARVIEWER_CMAKE_ARGUMENTS $CMAKE_DISTCC $CMAKE_COMPILER "$STARVIEWER_SOURCE_DIR_BASE"

if [[ $? -ne 0 ]]
then
    echo "[!] Error when trying to compile Starviewer. Quitting... [!]"
    exit 1
fi

$MAKE -j$MAKE_CONCURRENCY VERBOSE=$MAKE_VERBOSE
if [[ $? -ne 0 ]]
then
    echo "[!] Error when trying to compile Starviewer. Quitting... [!]"
    exit 1
fi

popd
