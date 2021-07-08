#!/bin/bash
DIR=$(dirname $(readlink -f $BASH_SOURCE))
LD_LIBRARY_PATH="$DIR/lib" "$DIR/starviewer" "$@"
