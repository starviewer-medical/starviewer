#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. $SCRIPTS_ROOT/config/setup.sh

mkdir -p $STARVIEWER_BUILD_DIR_BASE
pushd $STARVIEWER_BUILD_DIR_BASE

export SDK_INSTALL_PREFIX
export LD_LIBRARY_PATH
echo "Invoking QMake with the following extra arguments: ${STARVIEWER_QMAKE_ARGUMENTS[@]}"
$QTDIR/bin/qmake -r ${STARVIEWER_QMAKE_ARGUMENTS[@]} $STARVIEWER_SOURCE_DIR_BASE/starviewer.pro


popd
