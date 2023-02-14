#!/bin/bash
# Call configuration scripts of other libraries to make sure *CMAKEDIR variables are set
. "$SCRIPTS_ROOT/config/dcmtk.sh"
. "$SCRIPTS_ROOT/config/gdcm.sh"
. "$SCRIPTS_ROOT/config/vtk.sh"

SOURCE_DIR="$SDK_SOURCE_DIR_BASE/InsightToolkit-5.3.0"

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE:STRING=$SDK_BUILD_TYPE \
               -DCMAKE_INSTALL_PREFIX:PATH=$SDK_INSTALL_PREFIX \
               -DBUILD_EXAMPLES:BOOL=FALSE \
               -DBUILD_TESTING:BOOL=FALSE \
               -DITK_BUILD_DEFAULT_MODULES:BOOL=FALSE \
               -DITK_SKIP_PATH_LENGTH_CHECKS:BOOL=TRUE \
               -DModule_ITKCommon:BOOL=TRUE \
               -DModule_ITKFFT:BOOL=TRUE \
               -DModule_ITKImageGrid:BOOL=TRUE \
               -DModule_ITKIODCMTK:BOOL=TRUE \
               -DModule_ITKLabelVoting:BOOL=TRUE \
               -DModule_ITKLevelSets:BOOL=TRUE \
               -DModule_ITKRegionGrowing:BOOL=TRUE \
               -DModule_ITKRegistrationCommon:BOOL=TRUE \
               -DModule_ITKVTK:BOOL=TRUE \
               -DModule_ITKVtkGlue:BOOL=TRUE \
               -DITK_USE_SYSTEM_DCMTK:BOOL=TRUE \
               -DDCMTK_DIR:PATH=$DCMTKCMAKEDIR \
               -DITK_USE_SYSTEM_GDCM:BOOL=TRUE \
               -DGDCM_DIR:PATH=$GDCMCMAKEDIR \
               -DVTK_DIR:PATH=$VTKCMAKEDIR"

ITKCMAKEDIR="$SDK_INSTALL_PREFIX/lib/cmake/ITK-5.3"
ITKLIBDIR="$SDK_INSTALL_PREFIX/lib"
ITKBINDIR="$SDK_INSTALL_PREFIX/bin"
ITKINCLUDEDIR="$SDK_INSTALL_PREFIX/include/ITK-5.3"
