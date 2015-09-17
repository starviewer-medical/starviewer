#!/bin/bash

SOURCE_DIR=$SOURCE_DIR_BASE/VTK-6.1.0/VTK-6.1.0

if [ $BUILD_TYPE = debug ]
then
    CMAKE_BUILD_TYPE=Debug
    BUILD_DIR=$SOURCE_DIR-build-debug
    INSTALL_PREFIX=/usr/local
fi
if [ $BUILD_TYPE = release ]
then
    CMAKE_BUILD_TYPE=RelWithDebInfo
    BUILD_DIR=$SOURCE_DIR-build-release
    INSTALL_PREFIX=/usr/local
fi

################ Nothing should need to be changed below this line ################

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE:STRING=$CMAKE_BUILD_TYPE \
               -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PREFIX \
               -DVTK_Group_Qt:BOOL=TRUE \
               -DVTK_QT_VERSION:STRING=5"

VTK_DIR=$INSTALL_PREFIX/lib/cmake/vtk-6.1

