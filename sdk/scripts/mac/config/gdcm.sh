#!/bin/bash

# Call vtk configuration script to make sure VtkDir is set
. $ScriptsRoot/config/vtk.sh

SourceDir="$SourceDirBase/gdcm-2.4.4/gdcm-2.4.4"

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
                 -DGDCM_BUILD_SHARED_LIBS:BOOL=TRUE \
                 -DGDCM_USE_VTK:BOOL=TRUE \
                 -DVTK_DIR=$VtkDir"

GdcmDir="$InstallPrefix/lib/gdcm-2.4"
