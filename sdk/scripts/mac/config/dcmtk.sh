#!/bin/bash

SourceDir="$SourceDirBase/dcmtk-3.6.1_20120515"

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
              -DDCMTK_WITH_PRIVATE_TAGS:BOOL=TRUE \
              -DDCMTK_WITH_ZLIB:BOOL=TRUE \
              -DDCMTK_WITH_SNDFILE:BOOL=FALSE \
              -DDCMTK_WITH_TIFF:BOOL=FALSE \
              -DDCMTK_WITH_ICONV:BOOL=FALSE"
