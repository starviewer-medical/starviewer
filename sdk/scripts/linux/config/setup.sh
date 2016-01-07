#!/bin/bash

# List of build types to use. Possible values: debug, release.
BUILD_TYPES="release"
# List of libs to build. Possible values: dcmtk, vtk, gdcm, itk, ecm, threadweaver.
LIBS="dcmtk vtk gdcm itk ecm threadweaver"

# Where buildall.sh downloads the libraries
DOWNLOAD_PREFIX=~/uroot/downloads

# Where to install the SDK libraries once compiled
SDK_INSTALL_PREFIX=~/uroot/usr/local

# Only used to tell the user where to install Qt on buildall.sh
INSTALL_QTDIR=$SDK_INSTALL_PREFIX/lib/Qt5.4.1

# If you use a local Qt installaton, the path where it is installed
QTDIR=$SDK_INSTALL_PREFIX/lib/Qt5.4.1/5.4/gcc_64

# Starviewer source code location
STARVIEWER_SOURCE_DIR_BASE=$SCRIPTS_ROOT/../../../starviewer

# Location of SDK sources to build
SOURCE_DIR_BASE=$SDK_INSTALL_PREFIX/src

# Location of the pathes to apply on SDK libraries
PATCHES_ROOT=$SCRIPTS_ROOT/../../patches

# Because SDK libraries binares are not on a standard location.
# This environment variable has to be set when starting starviewer binary or
# when you compile it. (if not the linker will fail).
LD_LIBRARY_PATH=$SDK_INSTALL_PREFIX/lib:$SDK_INSTALL_PREFIX/lib/x86_64-linux-gnu:$SDK_INSTALL_PREFIX/lib/Qt5.4.1/5.4/gcc_64/lib/
