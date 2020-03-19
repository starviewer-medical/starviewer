#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. $SCRIPTS_ROOT/config/setup.sh

# This script detects the unix operating system type and then installs the required dependencies

if [[ $(uname) == 'Linux' ]]
then
    echo "Linux based system detected."
    if [ -e /etc/debian_version ]
    then
        echo "Debian-based system detected."
        if [ "10" = `cut -d . -f 1 /etc/debian_version` ]
        then
            echo "Debian 10 (Buster) detected."
            echo "Please, input the password to install the dependencies required to run and compile Starviewer."
            sudo apt-get install build-essential cmake git wget rsync
            sudo apt-get install libssl1.1 libssl-dev zlib1g zlib1g-dev libgl1 libgl1-mesa-dev libxt6 libxt-dev libglu1-mesa libglu1-mesa-dev
        fi
    fi
elif [[ $(uname) == 'Darwin' ]]
then
    echo "macOS system detected."
elif [[ $(uname) == 'MSYS_NT'* ]]
then
    echo "Windows NT (MSYS2) system detected."
fi
