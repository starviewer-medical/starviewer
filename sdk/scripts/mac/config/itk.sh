#!/bin/bash

# Call gdcm configuration script to make sure VtkDir and GdcmDir are set
. $ScriptsRoot/config/gdcm.sh

SourceDir="$SourceDirBase/InsightToolkit-4.7.1"

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
              -DBUILD_EXAMPLES:BOOL=FALSE \
              -DBUILD_TESTING:BOOL=FALSE \
              -DITK_USE_SYSTEM_GDCM:BOOL=TRUE \
              -DGDCM_DIR:PATH=$GdcmDir \
              -DVTK_DIR:PATH=$VtkDir"
