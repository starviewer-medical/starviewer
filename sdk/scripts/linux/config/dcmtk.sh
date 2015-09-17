#!/bin/bash

SOURCE_DIR=$SOURCE_DIR_BASE/dcmtk-3.6.1_20120515/dcmtk-3.6.1_20120515

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
               -DDCMTK_WITH_PRIVATE_TAGS:BOOL=TRUE \
               -DDCMTK_WITH_ZLIB:BOOL=TRUE \
               -DDCMTK_WITH_SNDFILE:BOOL=FALSE"

