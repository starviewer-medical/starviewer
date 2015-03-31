#!/bin/bash

SourceDir="$SourceDirBase/dcmtk-3.6.1_20120515/dcmtk-3.6.1_20120515"

if [ $BuildType = "debug" ]
then
    CMakeBuildType="Debug"
    BuildDir="$SourceDir-build-debug"
    InstallPrefix="/usr/local/"
fi
if [ $BuildType = "release" ]
then
    CMakeBuildType="RelWithDebInfo"
    BuildDir="$SourceDir-build-release"
    InstallPrefix="/usr/local/"
fi

# ============== Nothing should need to be changed below this line ==============

CMakeOptions="-DCMAKE_BUILD_TYPE:STRING=$CMakeBuildType \
                -DCMAKE_INSTALL_PREFIX:PATH=$InstallPrefix \
                -DDCMTK_WITH_TIFF:BOOL=OFF \
                -DDCMTK_WITH_ICONV:BOOL=OFF"
