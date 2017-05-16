#!/bin/bash

SourceDir="$SourceDirBase/threadweaver-5.3.0/extra-cmake-modules-1.3.0"

BuildDir=$SourceDir
InstallPrefix="$SourceDir-install"

# ============== Nothing should need to be changed below this line ==============

CMakeOptions="-DCMAKE_BUILD_TYPE:STRING=Release \
              -DCMAKE_INSTALL_PREFIX:PATH=$InstallPrefix \
              -DCMAKE_PREFIX_PATH:PATH=$QTDIR \
              -DBUILD_TESTING:BOOL=FALSE"

EcmDir="$InstallPrefix/share/ECM/cmake"
