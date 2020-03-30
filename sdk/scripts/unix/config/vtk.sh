#!/bin/bash

SOURCE_DIR="$SOURCE_DIR_BASE/VTK-8.2.0"

if [ $BUILD_TYPE = debug ]
then
    CMAKE_BUILD_TYPE=Debug
    BUILD_DIR="$SOURCE_DIR-build-debug"
fi
if [ $BUILD_TYPE = release ]
then
    CMAKE_BUILD_TYPE=RelWithDebInfo
    BUILD_DIR="$SOURCE_DIR-build-release"
fi

################ Nothing should need to be changed below this line ################

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE:STRING=$CMAKE_BUILD_TYPE \
               -DCMAKE_INSTALL_PREFIX:PATH=$SDK_INSTALL_PREFIX \
               -DCMAKE_PREFIX_PATH:PATH=$QTDIR \
               -DBUILD_TESTING:BOOL=FALSE \
               -DModule_vtkGUISupportQt:BOOL=TRUE \
               -DModule_vtkGUISupportQtOpenGL:BOOL=TRUE \
               -DModule_vtkGUISupportQtSQL:BOOL=TRUE \
               -DModule_vtkGUISupportQtWebkit:BOOL=FALSE \
               -DModule_vtkRenderingQt:BOOL=TRUE \
               -DModule_vtkViewsQt:BOOL=TRUE \
               -DVTK_QT_VERSION:STRING=5"

VTKCMAKEDIR="$SDK_INSTALL_PREFIX/$LIB64DIR/cmake/vtk-8.2"
VTKLIBDIR="$SDK_INSTALL_PREFIX/$LIB64DIR"
VTKINCLUDEDIR="$SDK_INSTALL_PREFIX/include/vtk-8.2"
