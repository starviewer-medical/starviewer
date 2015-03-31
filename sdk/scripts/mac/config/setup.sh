#!/bin/bash

# List of build types to use. Possible values: debug, release.
BuildTypes="release"
# List of libs to build. Possible values: dcmtk, vtk, gdcm, itk, ecm, threadweaver.
Libs="dcmtk vtk gdcm itk ecm threadweaver"
SourceDirBase="/Developer/starviewer-0.13-sdk"

CMakeGenerator="-G \"Unix Makefiles\""
CMake="/Applications/CMake\\ 2.8-12.app/Contents/bin/cmake"

QtInstallPrefix="/Developer/Qt/5.4/clang_64"
CMAKE_PREFIX_PATH=$QtInstallPrefix

Git="git"

PatchesRoot=$ScriptsRoot/../../patches
