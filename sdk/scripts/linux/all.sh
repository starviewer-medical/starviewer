#!/bin/bash

SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)
UNIX_SCRIPTS_ROOT="$SCRIPTS_ROOT/../unix"
. "$UNIX_SCRIPTS_ROOT/config/setup.sh"

. "$UNIX_SCRIPTS_ROOT/sdk_all.sh"

# Building starviewer
. $SCRIPTS_ROOT/starviewer_build.sh
. $SCRIPTS_ROOT/starviewer_dpkg.sh


echo "Your package is ready to install running dpkg -i starviewer.deb."
echo "The enviroment variables to compile with the SDK are placed in $SDK_ENVIRONMENT_FILE."
echo "Have a nice day."
