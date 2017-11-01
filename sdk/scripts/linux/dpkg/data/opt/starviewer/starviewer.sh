#!/bin/bash
DIR=$(readlink -f $(dirname $BASH_SOURCE))
LD_LIBRARY_PATH=/opt/starviewer/lib /opt/starviewer/bin/starviewer "$@"
