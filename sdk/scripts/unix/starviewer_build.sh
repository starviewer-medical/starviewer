#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. $SCRIPTS_ROOT/config/setup.sh

mkdir -p $STARVIEWER_BUILD_DIR_BASE
pushd $STARVIEWER_BUILD_DIR_BASE

echo "Invoking QMake with the following extra arguments: ${STARVIEWER_QMAKE_ARGUMENTS[@]}"
$QTDIR/bin/qmake -r ${STARVIEWER_QMAKE_ARGUMENTS[@]} $STARVIEWER_SOURCE_DIR_BASE/starviewer.pro

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
