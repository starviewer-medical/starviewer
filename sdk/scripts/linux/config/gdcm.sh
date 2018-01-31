#!/bin/bash

# Call vtk configuration script to make sure VTKCMAKEDIR is set
. $SCRIPTS_ROOT/config/vtk.sh

SOURCE_DIR=$SOURCE_DIR_BASE/gdcm-2.6.4

if [ $BUILD_TYPE = debug ]
then
    CMAKE_BUILD_TYPE=Debug
    BUILD_DIR=$SOURCE_DIR-build-debug
fi
if [ $BUILD_TYPE = releasewithdebinfo ]
then
    CMAKE_BUILD_TYPE=RelWithDebInfo
    BUILD_DIR=$SOURCE_DIR-build-releasewithdebinfo
fi
if [ $BUILD_TYPE = release ]
then
    CMAKE_BUILD_TYPE=Release
    BUILD_DIR=$SOURCE_DIR-build-release
fi

################ Nothing should need to be changed below this line ################

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE:STRING=$CMAKE_BUILD_TYPE \
               -DCMAKE_INSTALL_PREFIX:PATH=$SDK_INSTALL_PREFIX \
               -DGDCM_BUILD_SHARED_LIBS:BOOL=TRUE \
               -DGDCM_USE_VTK:BOOL=TRUE \
               -DVTK_DIR:PATH=$VTKCMAKEDIR"

GDCMCMAKEDIR=$SDK_INSTALL_PREFIX/lib/gdcm-2.6
GDCMLIBDIR=$SDK_INSTALL_PREFIX/lib
GDCMINCLUDEDIR=$SDK_INSTALL_PREFIX/include/gdcm-2.6
