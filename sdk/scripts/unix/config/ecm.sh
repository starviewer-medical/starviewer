#!/bin/bash

SOURCE_DIR="$SOURCE_DIR_BASE/threadweaver-5.64.0/extra-cmake-modules-5.64.0"
BUILD_DIR=$SOURCE_DIR

################ Nothing should need to be changed below this line ################

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE:STRING=Release \
               -DCMAKE_INSTALL_PREFIX:PATH=$SOURCE_DIR-install \
               -DBUILD_TESTING:BOOL=FALSE \
               -DBUILD_HTML_DOCS:BOOL=FALSE \
               -DBUILD_MAN_DOCS:BOOL=FALSE"

ECMCMAKEDIR="$SOURCE_DIR-install/share/ECM/cmake"
