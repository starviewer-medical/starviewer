#!/bin/bash

# Call vtk configuration script to make sure VTK_DIR is set
. $SCRIPTS_ROOT/config/vtk.sh

SOURCE_DIR=$SOURCE_DIR_BASE/gdcm-2.4.4/gdcm-2.4.4

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
               -DGDCM_BUILD_SHARED_LIBS:BOOL=TRUE \
               -DGDCM_USE_VTK:BOOL=TRUE \
               -DVTK_DIR:PATH=$VTK_DIR"

GDCM_DIR=$INSTALL_PREFIX/lib/gdcm-2.4

