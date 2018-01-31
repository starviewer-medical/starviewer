#!/bin/bash

# Call ecm configuration script to make sure ECM_DIR is set
. $SCRIPTS_ROOT/config/ecm.sh

SOURCE_DIR=$SOURCE_DIR_BASE/threadweaver-5.3.0

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
               -DBUILD_TESTING:BOOL=FALSE \
               -DECM_DIR:PATH=$ECM_DIR"

THREADWEAVERCMAKEDIR=$SDK_INSTALL_PREFIX/lib/x86_64-linux-gnu/cmake/KF5ThreadWeaver/
THREADWEAVERLIBDIR=$SDK_INSTALL_PREFIX/lib/x86_64-linux-gnu
THREADWEAVERINCLUDEDIR=$SDK_INSTALL_PREFIX/include/KF5
