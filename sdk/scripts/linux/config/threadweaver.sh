#!/bin/bash

# Call ecm configuration script to make sure ECM_DIR is set
. $SCRIPTS_ROOT/config/ecm.sh

SOURCE_DIR=$SOURCE_DIR_BASE/threadweaver-5.3.0/threadweaver-5.3.0

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
               -DBUILD_TESTING:BOOL=FALSE \
               -DECM_DIR:PATH=$ECM_DIR"

