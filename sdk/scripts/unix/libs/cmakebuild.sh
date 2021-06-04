#!/bin/bash

PATCHES_ROOT="$SCRIPTS_ROOT/../../patches"
BUILD_DIR="$SOURCE_DIR-$BUILD_TYPE"

mkdir -p "$BUILD_DIR"
pushd "$BUILD_DIR"

if [[ -d "$PATCHES_ROOT/$ALIB" ]]
then
    PATCHED_SOURCE_DIR="$SOURCE_DIR-patched"
    rsync -a --delete "$SOURCE_DIR/" "$PATCHED_SOURCE_DIR"
    pushd "$PATCHED_SOURCE_DIR"

    for PATCH in "$PATCHES_ROOT/$ALIB"/*
    do
        git apply "$PATCH" --ignore-whitespace -v
        if [ $? -ne 0 ]
        then
            echo "ERROR: The \""$PATCH\"" patch could not be applied."
            exit $?
        fi
    done

    popd
    SOURCE_DIR="$PATCHED_SOURCE_DIR"
fi

if [[ $(uname) == 'MSYS_NT'* ]]
then
    CMAKE_GENERATOR="NMake Makefiles JOM"
else
    CMAKE_GENERATOR="Unix Makefiles"
fi

"$CMAKE" -G "$CMAKE_GENERATOR" -Wno-dev $CMAKE_OPTIONS $CMAKE_DISTCC $CMAKE_COMPILER $CMAKE_CPP11 "$SOURCE_DIR"
if [[ $? -ne 0 ]]
then
    echo "ERROR: CMake of $ALIB failed"
    exit
fi

$MAKE -j$MAKE_CONCURRENCY VERBOSE=$MAKE_VERBOSE
if [[ $? -ne 0 ]]
then
    echo "ERROR: Compilation of $ALIB failed"
    exit
fi

$MAKE VERBOSE=$MAKE_VERBOSE install
if [[ $? -ne 0 ]]
then
    echo "ERROR: Installation phase of $ALIB failed"
    exit
fi

popd
