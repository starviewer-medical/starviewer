#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. $SCRIPTS_ROOT/config/setup.sh


. $SCRIPTS_ROOT/sdk_download.sh
. $SCRIPTS_ROOT/sdk_extract.sh
. $SCRIPTS_ROOT/sdk_build.sh

# Building starviewer
. $SCRIPTS_ROOT/starviewer_build.sh
. $SCRIPTS_ROOT/starviewer_dpkg.sh


echo "Your package is ready to install running dpkg -i starviewer.deb."
echo "The enviroment variables to compile with the SDK are placed in $SDK_ENVIRONMENT_FILE."
echo "Have a nice day."
