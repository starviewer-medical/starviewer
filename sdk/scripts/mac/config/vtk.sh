#!/bin/bash

SourceDir="$SourceDirBase/VTK-7.0.0"

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
              -DModule_vtkGUISupportQt:BOOL=TRUE \
              -DModule_vtkGUISupportQtOpenGL:BOOL=TRUE \
              -DModule_vtkGUISupportQtSQL:BOOL=TRUE \
              -DModule_vtkGUISupportQtWebkit:BOOL=FALSE \
              -DModule_vtkRenderingQt:BOOL=TRUE \
              -DModule_vtkViewsQt:BOOL=TRUE \
              -DVTK_REQUIRED_OBJCXX_FLAGS:STRING=\"\" \
              -DVTK_QT_VERSION:STRING=5"

VtkDir="$SdkInstallPrefix/lib/cmake/vtk-7.0"
