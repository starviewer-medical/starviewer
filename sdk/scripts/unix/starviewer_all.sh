#!/bin/bash
SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)
. "$SCRIPTS_ROOT/config/setup.sh"

. $SCRIPTS_ROOT/starviewer_build.sh
. $SCRIPTS_ROOT/starviewer_install.sh
