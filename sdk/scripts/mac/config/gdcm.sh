#!/bin/bash

# Call vtk configuration script to make sure VtkDir is set
. $ScriptsRoot/config/vtk.sh

SourceDir="$SourceDirBase/gdcm-2.6.4"

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
              -DGDCM_BUILD_SHARED_LIBS:BOOL=TRUE \
              -DGDCM_USE_VTK:BOOL=TRUE \
              -DVTK_DIR=$VtkDir"

GdcmDir="$SdkInstallPrefix/lib/gdcm-2.6"
