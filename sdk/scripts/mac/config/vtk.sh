#!/bin/bash

SourceDir="$SourceDirBase/VTK-6.1.0/VTK-6.1.0"

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
			  -DVTK_Group_Qt:BOOL=TRUE \
			  -DVTK_REQUIRED_OBJCXX_FLAGS:STRING=\"\" \
			  -DCMAKE_INSTALL_PREFIX:PATH=$InstallPrefix \
			  -DVTK_QT_VERSION:STRING=5"

VtkDir="$InstallPrefix/lib/cmake/vtk-6.1"
