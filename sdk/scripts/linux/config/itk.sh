#!/bin/bash

# Call gdcm configuration script to make sure GDCMCMAKEDIR is set
# This script in turn calls vtk configuration script, so VTKCMAKEDIR will also be set
. $SCRIPTS_ROOT/config/gdcm.sh

SOURCE_DIR=$SOURCE_DIR_BASE/InsightToolkit-4.7.1

if [ $BUILD_TYPE = debug ]
then
    CMAKE_BUILD_TYPE=Debug
    BUILD_DIR=$SOURCE_DIR-build-debug
fi
if [ $BUILD_TYPE = release ]
then
    CMAKE_BUILD_TYPE=RelWithDebInfo
    BUILD_DIR=$SOURCE_DIR-build-release
fi

################ Nothing should need to be changed below this line ################

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE:STRING=$CMAKE_BUILD_TYPE \
               -DCMAKE_INSTALL_PREFIX:PATH=$SDK_INSTALL_PREFIX \
               -DCMAKE_PREFIX_PATH:PATH=$QTDIR \
               -DBUILD_EXAMPLES:BOOL=FALSE \
               -DBUILD_TESTING:BOOL=FALSE \
               -DBUILD_SHARED_LIBS:BOOL=TRUE \
               -DITK_USE_SYSTEM_GDCM:BOOL=TRUE \
               -DGDCM_DIR:PATH=$GDCMCMAKEDIR \
               -DVTK_DIR:PATH=$VTKCMAKEDIR"

ITKCMAKEDIR=$SDK_INSTALL_PREFIX/lib/cmake/ITK-4.7
ITKLIBDIR=$SDK_INSTALL_PREFIX/lib
ITKINCLUDEDIR=$SDK_INSTALL_PREFIX/include/ITK-4.7
