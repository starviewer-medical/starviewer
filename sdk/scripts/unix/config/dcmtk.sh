#!/bin/bash

SOURCE_DIR="$SOURCE_DIR_BASE/dcmtk-DCMTK-3.6.5"

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

if [[ $(uname) == 'Linux' ]]
then
    ICONV=FALSE
    STDLIBC_ICONV=TRUE
    CMAKE_EXTRA_OPTIONS=
elif [[ $(uname) == 'Darwin' ]]
then
    ICONV=TRUE
    STDLIBC_ICONV=FALSE
    CMAKE_EXTRA_OPTIONS=-DCMAKE_PREFIX_PATH:PATH=/usr/local/opt/openssl
fi

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE:STRING=$CMAKE_BUILD_TYPE \
               -DCMAKE_INSTALL_PREFIX:PATH=$SDK_INSTALL_PREFIX \
               -DDCMTK_WITH_TIFF:BOOL=FALSE \
               -DDCMTK_WITH_PNG:BOOL=FALSE \
               -DDCMTK_WITH_XML:BOOL=FALSE \
               -DDCMTK_WITH_ZLIB:BOOL=TRUE \
               -DDCMTK_WITH_OPENSSL:BOOL=TRUE \
               -DDCMTK_WITH_SNDFILE:BOOL=FALSE \
               -DDCMTK_WITH_ICONV:BOOL=$ICONV \
               -DDCMTK_WITH_ICU:BOOL=FALSE \
               -DDCMTK_WITH_STDLIBC_ICONV:BOOL=$STDLIBC_ICONV \
               -DDCMTK_WITH_WRAP:BOOL=FALSE \
               -DDCMTK_ENABLE_PRIVATE_TAGS:BOOL=TRUE \
               -DDCMTK_WITH_THREADS:BOOL=TRUE \
               -DDCMTK_WITH_DOXYGEN:BOOL=FALSE \
               -DDCMTK_ENABLE_CXX11:BOOL=TRUE \
               -DDCMTK_ENABLE_BUILTIN_DICTIONARY:BOOL=TRUE \
               -DDCMTK_ENABLE_EXTERNAL_DICTIONARY:BOOL=FALSE \
               $CMAKE_EXTRA_OPTIONS"

DCMTKLIBDIR="$SDK_INSTALL_PREFIX/lib"
DCMTKINCLUDEDIR="$SDK_INSTALL_PREFIX/include/dcmtk"
