#!/bin/bash
DIR=$(readlink -f $(dirname $BASH_SOURCE))
LD_LIBRARY_PATH="$DIR" "$DIR/starviewer" "$@"
