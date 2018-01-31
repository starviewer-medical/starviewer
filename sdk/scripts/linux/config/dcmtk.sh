#!/bin/bash

SOURCE_DIR=$SOURCE_DIR_BASE/dcmtk-3.6.1_20120515

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
               -DDCMTK_WITH_PRIVATE_TAGS:BOOL=TRUE \
               -DDCMTK_WITH_ZLIB:BOOL=TRUE \
               -DDCMTK_WITH_SNDFILE:BOOL=FALSE \
               -DDCMTK_WITH_OPENSSL:BOOL=FALSE"

DCMTKLIBDIR=$SDK_INSTALL_PREFIX/lib
DCMTKINCLUDEDIR=$SDK_INSTALL_PREFIX/include/dcmtk
