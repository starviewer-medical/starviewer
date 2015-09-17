#!/bin/bash

SOURCE_DIR=$SOURCE_DIR_BASE/threadweaver-5.3.0/extra-cmake-modules-1.3.0

BUILD_DIR=$SOURCE_DIR
INSTALL_PREFIX=$SOURCE_DIR-install

################ Nothing should need to be changed below this line ################

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE:STRING=Release \
               -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PREFIX \
               -DBUILD_TESTING:BOOL=FALSE"

ECM_DIR=$INSTALL_PREFIX/share/ECM/cmake

