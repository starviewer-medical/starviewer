#!/bin/bash

# Call ecm configuration script to make sure EcmDir is set
. $ScriptsRoot/config/ecm.sh

SourceDir="$SourceDirBase/threadweaver-5.3.0"

if [ $BuildType = "debug" ]
then
    CMakeBuildType="Debug"
    BuildDir="$SourceDir-build-debug"
fi
if [ $BuildType = "release" ]
then
    CMakeBuildType="RelWithDebInfo"
    BuildDir="$SourceDir-build-release"
fi

# ============== Nothing should need to be changed below this line ==============

CMakeOptions="-DCMAKE_BUILD_TYPE:STRING=$CMakeBuildType \
              -DCMAKE_INSTALL_PREFIX:PATH=$SdkInstallPrefix \
              -DCMAKE_PREFIX_PATH:PATH=$QTDIR \
              -DBUILD_TESTING:BOOL=FALSE \
              -DECM_DIR:PATH=$EcmDir"
