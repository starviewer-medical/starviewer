#!/bin/bash
SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)
. "$SCRIPTS_ROOT/config/setup.sh"

# ALIB instead of LIB to avoid an enviroment variable naming conflict with MSVC
for ALIB in $LIBS
do
    if [[ -f "$SCRIPTS_ROOT/libs/$ALIB.sh" ]]
    then
        . "$SCRIPTS_ROOT/config/$ALIB.sh"
        . "$SCRIPTS_ROOT/libs/$ALIB.sh"
    fi
done
