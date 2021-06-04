#!/bin/bash

# Call ecm configuration script to make sure ECMCMAKEDIR is set
. "$SCRIPTS_ROOT/config/ecm.sh"

SOURCE_DIR="$SOURCE_DIR_BASE/threadweaver-5.64.0"

################ Nothing should need to be changed below this line ################

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE:STRING=$BUILD_TYPE \
               -DCMAKE_INSTALL_PREFIX:PATH=$SDK_INSTALL_PREFIX \
               -DCMAKE_PREFIX_PATH:PATH=$QTDIR \
               -DBUILD_TESTING:BOOL=FALSE \
               -DECM_DIR:PATH=$ECMCMAKEDIR"

THREADWEAVERCMAKEDIR="$SDK_INSTALL_PREFIX/$LIB64DIR/cmake/KF5ThreadWeaver/"
THREADWEAVERLIBDIR="$SDK_INSTALL_PREFIX/$LIB64DIR"
THREADWEAVERINCLUDEDIR="$SDK_INSTALL_PREFIX/include/KF5"
