#!/bin/bash

SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)
. "$SCRIPTS_ROOT/config/setup.sh"

export SDK_INSTALL_PREFIX
if [[ $(uname) == 'Linux' ]]
then
    export LD_LIBRARY_PATH
elif [[ $(uname) == 'Darwin' ]]
then
    export DYLD_LIBRARY_PATH="$LD_LIBRARY_PATH"
    export DYLD_FRAMEWORK_PATH="$LD_LIBRARY_PATH"
fi

mkdir -p "$STARVIEWER_BUILD_DIR_BASE"
pushd "$STARVIEWER_BUILD_DIR_BASE"

$QMAKE "$STARVIEWER_SOURCE_DIR_BASE/starviewer.pro" -spec $SPEC

make $MAKE_CONCURRENCY $MAKE_VERBOSE
if [[ $? -ne 0 ]]
then
    echo "ERROR: Compilation of Starviewer failed"
    exit
fi

popd
