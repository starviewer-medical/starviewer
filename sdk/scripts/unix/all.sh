#!/bin/bash
SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)

if [[ $(uname) == 'MSYS_NT'* ]]
then
    echo "This script wishes to run configure_system.sh script, but it is not possible because the actions performed there require you to manually open and close the MSYS2 console, and even reboot your computer if you enable long paths."
    echo "Run manually configure_system.sh at least once. If you do so, close this window."
    read -p "Press enter to continue. Press Ctrl+C to exit."
else
    . "$SCRIPTS_ROOT/configure_system.sh"
fi
. "$SCRIPTS_ROOT/generate_prefixfiles.sh"
. "$SCRIPTS_ROOT/sdk_all.sh"
. "$SCRIPTS_ROOT/starviewer_all.sh"
