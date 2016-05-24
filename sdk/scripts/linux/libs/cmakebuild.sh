#!/bin/bash

mkdir -p $BUILD_DIR
pushd $BUILD_DIR

if [ -d $PATCHES_ROOT/$LIB ]
then
    PATCHED_SOURCE_DIR=$SOURCE_DIR-patched
    rsync -a --delete $SOURCE_DIR/ $PATCHED_SOURCE_DIR
    pushd $PATCHED_SOURCE_DIR

    for PATCH in $PATCHES_ROOT/$LIB/*
    do
        git apply $PATCH --ignore-whitespace -v
    done

    popd
    SOURCE_DIR=$PATCHED_SOURCE_DIR
fi

cmake -Wno-dev $CMAKE_OPTIONS $CMAKE_DISTCC $CMAKE_COMPILER $SOURCE_DIR
if [ $? -ne 0 ]
then
    echo "ERROR: CMake of $LIB failed"
    exit
fi

make -j$MAKE_CONCURRENCY $MAKE_VERBOSE
if [ $? -ne 0 ]
then
    echo "ERROR: Compilation of $LIB failed"
    exit
fi

make $MAKE_VERBOSE install
if [ $? -ne 0 ]
then
    echo "ERROR: Installation phase of $LIB failed"
    exit
fi

popd
