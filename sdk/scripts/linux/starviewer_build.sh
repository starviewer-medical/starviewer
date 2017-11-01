#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. $SCRIPTS_ROOT/config/setup.sh

# Building starviewer

export SDK_INSTALL_PREFIX
export LD_LIBRARY_PATH

mkdir -p $STARVIEWER_BUILD_DIR_BASE
cd $STARVIEWER_BUILD_DIR_BASE
$QTDIR/bin/qmake $STARVIEWER_SOURCE_DIR_BASE/starviewer.pro -r -spec linux-g++
make -j$MAKE_CONCURRENCY $MAKE_VERBOSE

