#!/bin/bash

SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)
. "SCRIPTS_ROOT/config/setup.sh"

# Build SDK
. "SCRIPTS_ROOT/sdk_all.sh"

# Build starviewer
. "SCRIPTS_ROOT/starviewer_build.sh"

# Build package
if [[ $(uname) == 'Linux' ]]
then
    . "$SCRIPTS_ROOT/starviewer_dpkg.sh"
elif [[ $(uname) == 'Darwin' ]]
then
    . "$SCRIPTS_ROOT/starviewer_dmg.sh"
fi

if [[ $(uname) == 'Linux' ]]
then
    echo "Your package is ready to install running dpkg -i starviewer.deb."
fi
echo "The enviroment variables to compile with the SDK are placed in $SDK_ENVIRONMENT_FILE."
echo "Have a nice day."
