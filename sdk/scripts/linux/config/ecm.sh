#!/bin/bash
SOURCE_DIR=$SOURCE_DIR_BASE/threadweaver-5.3.0/extra-cmake-modules-1.3.0
BUILD_DIR=$SOURCE_DIR

################ Nothing should need to be changed below this line ################

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE:STRING=Release \
               -DCMAKE_INSTALL_PREFIX:PATH=$SOURCE_DIR-install \
               -DBUILD_TESTING:BOOL=FALSE"

ECM_DIR=$SOURCE_DIR-install/share/ECM/cmake
