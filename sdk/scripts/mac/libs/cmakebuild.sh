#!/bin/bash

mkdir $BuildDir
pushd $BuildDir

if [ -d "$PatchesRoot/$Lib" ]
then
    PatchedSourceDir="$SourceDir-patched"
    rsync -a --delete $SourceDir/ $PatchedSourceDir
    pushd $PatchedSourceDir

    for PATCH in $(ls "$PatchesRoot/$Lib")
    do
        $Git apply $PatchesRoot/$Lib/$PATCH --ignore-whitespace -v
    done

    popd
    SourceDir=$PatchedSourceDir
fi

$CMake -Wno-dev $CMakeOptions $SourceDir
make -j$MakeConcurrency
make install

popd
