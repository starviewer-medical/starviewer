#!/bin/bash
SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)
. "$SCRIPTS_ROOT/config/setup.sh"

. "$SCRIPTS_ROOT/sdk_download.sh"
. "$SCRIPTS_ROOT/sdk_extract.sh"
. "$SCRIPTS_ROOT/sdk_build.sh"

echo "The Starviewer SDK has been built and installed in $SDK_BASE_PREFIX."
echo "The enviroment variables to compile with the SDK have been placed in $SDK_ENVIRONMENT_FILE."
