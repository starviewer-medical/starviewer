#!/bin/bash

# List of build types to use. Possible values: debug, release.
BUILD_TYPES="release"
# List of libs to build. Possible values: dcmtk, vtk, gdcm, itk, ecm, threadweaver.
LIBS="dcmtk vtk gdcm itk ecm threadweaver"

SOURCE_DIR_BASE=~/starviewer-sdk

PATCHES_ROOT=$SCRIPTS_ROOT/../../patches

