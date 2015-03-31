#!/bin/bash

ScriptsRoot=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

. $ScriptsRoot/config/setup.sh

for BuildType in $BuildTypes
do
    for Lib in $Libs
    do
        . $ScriptsRoot/config/$Lib.sh
        . $ScriptsRoot/libs/$Lib.sh
    done
done
