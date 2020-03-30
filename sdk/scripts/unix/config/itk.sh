#!/bin/bash

# Call gdcm configuration script to make sure GDCMCMAKEDIR is set
. "$SCRIPTS_ROOT/config/gdcm.sh"

SOURCE_DIR="$SOURCE_DIR_BASE/InsightToolkit-5.0.1"

if [ $BUILD_TYPE = debug ]
then
    CMAKE_BUILD_TYPE=Debug
    BUILD_DIR="$SOURCE_DIR-build-debug"
fi
if [ $BUILD_TYPE = release ]
then
    CMAKE_BUILD_TYPE=RelWithDebInfo
    BUILD_DIR="$SOURCE_DIR-build-release"
fi

################ Nothing should need to be changed below this line ################

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE:STRING=$CMAKE_BUILD_TYPE \
               -DCMAKE_INSTALL_PREFIX:PATH=$SDK_INSTALL_PREFIX \
               -DBUILD_EXAMPLES:BOOL=FALSE \
               -DBUILD_TESTING:BOOL=FALSE \
               -DITK_BUILD_DEFAULT_MODULES:BOOL=FALSE \
               -DModule_ITKCommon:BOOL=TRUE \
               -DModule_ITKFFT:BOOL=TRUE \
               -DModule_ITKImageGrid:BOOL=TRUE \
               -DModule_ITKLabelVoting:BOOL=TRUE \
               -DModule_ITKLevelSets:BOOL=TRUE \
               -DModule_ITKRegionGrowing:BOOL=TRUE \
               -DModule_ITKRegistrationCommon:BOOL=TRUE \
               -DModule_ITKVTK:BOOL=TRUE \
               -DITK_USE_SYSTEM_GDCM:BOOL=TRUE \
               -DGDCM_DIR:PATH=$GDCMCMAKEDIR"

ITKCMAKEDIR="$SDK_INSTALL_PREFIX/lib/cmake/ITK-5.0"
ITKLIBDIR="$SDK_INSTALL_PREFIX/lib"
ITKINCLUDEDIR="$SDK_INSTALL_PREFIX/include/ITK-5.0"
