#!/bin/bash

if [[ $(uname) == 'MSYS_NT'* ]]
then
    echo It is strongly recommended that you lift path length limit to avoid problems with ITK. If you have not done it already check this link: https://www.howtogeek.com/266621/how-to-make-windows-10-accept-file-paths-over-260-characters/
    read -p "Press enter to continue or Ctrl+C to stop"
fi

# Base directory for everything related to the SDK.
SDK_BASE_PREFIX=${SDK_BASE_PREFIX:-"$SCRIPTS_ROOT/../../.."}

# Where the libraries are dowloaded.
DOWNLOAD_PREFIX=${DOWNLOAD_PREFIX:-"$SDK_BASE_PREFIX/sdk-download"}

# Location of SDK sources to build.
SOURCE_DIR_BASE=${SOURCE_DIR_BASE:-"$SDK_BASE_PREFIX/sdk-build"}

# Where to install the SDK libraries once compiled.
SDK_INSTALL_PREFIX=${SDK_INSTALL_PREFIX:-"$SDK_BASE_PREFIX/sdk-install"}

# Where the user will be asked to install Qt.
INSTALL_QTDIR=${INSTALL_QTDIR:-"$SDK_INSTALL_PREFIX/qt"}

# Directory where the specific version of Qt is installed.
if [[ $(uname) == 'Linux' ]]
then
    QTDIR=${QTDIR:-"$INSTALL_QTDIR/5.12.6/gcc_64"}
elif [[ $(uname) == 'Darwin' ]]
then
    QTDIR=${QTDIR:-"$INSTALL_QTDIR/5.12.6/clang_64"}
elif [[ $(uname) == 'MSYS_NT'* ]]
then
    QTDIR=${QTDIR:-"$INSTALL_QTDIR/5.12.6/msvc2017_64"}
else
    echo "Error: Qt platform not considered."
    exit 1
fi

# List of build types to use. Possible values: Debug, Release, RelWithDebInfo.
BUILD_TYPES=${BUILD_TYPES:-"Release"}

# List of libs to build. Possible values: qt, dcmtk, vtk, gdcm, itk, ecm, threadweaver.
LIBS=${LIBS:-"qt dcmtk vtk gdcm itk ecm threadweaver"}

# CMake and make
if [[ $(uname) == 'MSYS_NT'* ]]
then
    CMAKE=${CMAKE:-$PROGRAMFILES/CMake/bin/cmake.exe}
    # jom path in PATH needed by CMake
    JOM_PATH=${JOM_PATH:-"$INSTALL_QTDIR/Tools/QtCreator/bin"}
    PATH=$PATH:$JOM_PATH
    MAKE=jom
else
    CMAKE=${CMAKE:-cmake}
    MAKE=make
fi

# Set to appropriate value (example below) if you want to use distcc (distributed C compiler) on CMake.
CMAKE_DISTCC=${CMAKE_DISTCC:-}
#CMAKE_DISTCC='-DCMAKE_CXX_COMPILER_LAUNCHER:STRING=distcc -DCMAKE_C_COMPILER_LAUNCHER:STRING=distcc'

# Set to appropriate value (example below) to compile with non-default compiler (default is gcc on Linux and clang in Mac).
CMAKE_COMPILER=${CMAKE_COMPILER:-}
#CMAKE_COMPILER='-DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang'

# C++11 support
CMAKE_CPP11='-DCMAKE_CXX_STANDARD:STRING=11 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=ON -DCMAKE_C_STANDARD:STRING=11 -DCMAKE_C_STANDARD_REQUIRED:BOOL=ON'

# Number of simultaneous make jobs.
MAKE_CONCURRENCY=${MAKE_CONCURRENCY:-1}

# Verbose compilation: set to 'yes' to output the compiler calls.
MAKE_VERBOSE=${MAKE_VERBOSE:-yes}

# Where to write the SDK environment configuration script.
SDK_ENVIRONMENT_FILE=$SCRIPTS_ROOT/../../../prefix.sh

# Currently only used to know the location of the ThreadWeaver lib dir.
if [[ $(uname) == 'Linux' ]]
then
    if [[ -e /etc/debian_version ]]
    then
        LIB64DIR=lib/`dpkg-architecture -q DEB_BUILD_GNU_TYPE`
    else
        LIB64DIR=lib64
    fi
elif [[ $(uname) == 'Darwin' ]]
then
    LIB64DIR=lib
elif [[ $(uname) == 'MSYS_NT'* ]]
then
    LIB64DIR=lib
fi

# Because SDK libraries binares are not on a standard location.
# This environment variable has to be set when starting starviewer binary or
# when you compile it (if not the linker will fail).
# It will be written to the SDK environment configuration script.
LD_LIBRARY_PATH="$SDK_INSTALL_PREFIX/lib:$SDK_INSTALL_PREFIX/$LIB64DIR:$QTDIR/lib"

# Starviewer source code location
STARVIEWER_SOURCE_DIR_BASE=$SCRIPTS_ROOT/../../../starviewer

# Starviwer shadow build directory
STARVIEWER_BUILD_DIR_BASE=$SCRIPTS_ROOT/../../../starviewer-build

# Temporary directory for package creation
DPKG_TMP=/tmp/starviewer-dpkg

# Where to place the packages
DPKG_DESTINATION=$SCRIPTS_ROOT/../../../starviewer-packaging
