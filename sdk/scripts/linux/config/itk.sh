#!/bin/bash

# Call gdcm configuration script to make sure GDCM_DIR is set
# This script in turn calls vtk configuration script, so VTK_DIR will also be set
. $SCRIPTS_ROOT/config/gdcm.sh

SOURCE_DIR=$SOURCE_DIR_BASE/InsightToolkit-4.7.1/InsightToolkit-4.7.1

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
               -DBUILD_EXAMPLES:BOOL=FALSE \
               -DBUILD_TESTING:BOOL=FALSE \
               -DBUILD_SHARED_LIBS:BOOL=TRUE \
               -DITK_USE_SYSTEM_GDCM:BOOL=TRUE \
               -DGDCM_DIR:PATH=$GDCM_DIR \
               -DVTK_DIR:PATH=$VTK_DIR"

