#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. $SCRIPTS_ROOT/config/setup.sh

pushd $STARVIEWER_BUILD_DIR_BASE

$MAKE VERBOSE=$MAKE_VERBOSE install

popd
