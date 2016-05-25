#!/bin/bash

# List of build types to use. Possible values: debug, release.
BuildTypes="release"
# List of libs to build. Possible values: dcmtk, vtk, gdcm, itk, ecm, threadweaver.
Libs="dcmtk vtk gdcm itk ecm threadweaver"

# Global prefix of the Starviewer SDK files.
Prefix=~/starviewer-sdk-0.14

# Where buildall.sh downloads the libraries.
DownloadPrefix=$Prefix/downloads

# Where to install the SDK libraries once compiled.
SdkInstallPrefix=$Prefix/usr/local

# If you use a local Qt installaton, the path where it is installed.
QTDIR=~/Qt/5.6/clang_64

# Location of SDK sources to build.
SourceDirBase=$SdkInstallPrefix/src

# Location of the pathes to apply to SDK libraries.
PatchesRoot=$ScriptsRoot/../../patches

# C++11 support.
CMakeCpp11='-DCMAKE_CXX_STANDARD:STRING=11 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=ON -DCMAKE_C_STANDARD:STRING=11 -DCMAKE_C_STANDARD_REQUIRED:BOOL=ON'

# Number of simultaneous make jobs (-j8).
MakeConcurrency=8

CMake="/Applications/CMake.app/Contents/bin/cmake"

Git="git"
