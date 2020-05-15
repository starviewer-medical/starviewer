#!/bin/bash

SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)
. "$SCRIPTS_ROOT/config/setup.sh"

env

echo "================ WHEREIS ================"

whereis ldd
whereis strip
whereis objdump
whereis objcopy
whereis link
whereis cl
whereis g++
whereis c++
whereis cpp
whereis gcc
whereis clang
whereis clang++
whereis cmake
whereis qmake
whereis make
whereis jom

# The IFS is an internal bash variable that establishes the bash interpreter separator.
UNTOUCHED_IFS=$IFS
IFS=':'
echo "================= PATH ================="
for ITEM in $PATH
do
    echo "$ITEM"
done
echo "=========== LD_LIBRARY_PATH ============"
for ITEM in $LD_LIBRARY_PATH
do
    echo "$ITEM"
done
IFS=$UNTOUCHED_IFS
