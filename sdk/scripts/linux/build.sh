#!/bin/bash

SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))

. $SCRIPTS_ROOT/config/setup.sh

for BUILD_TYPE in $BUILD_TYPES
do
    for LIB in $LIBS
    do
        . $SCRIPTS_ROOT/config/$LIB.sh
        . $SCRIPTS_ROOT/libs/$LIB.sh
    done
done

