#!/bin/bash

# Call vtk configuration script to make sure VTKCMAKEDIR is set
. "$SCRIPTS_ROOT/config/vtk.sh"

SOURCE_DIR="$SOURCE_DIR_BASE/GDCM-3.0.4"

################ Nothing should need to be changed below this line ################

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE:STRING=$BUILD_TYPE \
               -DCMAKE_INSTALL_PREFIX:PATH=$SDK_INSTALL_PREFIX \
               -DGDCM_BUILD_SHARED_LIBS:BOOL=TRUE \
               -DGDCM_BUILD_DOCBOOK_MANPAGES:BOOL=FALSE \
               -DGDCM_USE_VTK:BOOL=TRUE \
               -DVTK_DIR:PATH=$VTKCMAKEDIR"

GDCMCMAKEDIR="$SDK_INSTALL_PREFIX/lib/gdcm-3.0"
GDCMLIBDIR="$SDK_INSTALL_PREFIX/lib"
GDCMINCLUDEDIR="$SDK_INSTALL_PREFIX/include/gdcm-3.0"
