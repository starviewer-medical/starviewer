#!/bin/bash

mkdir $BuildDir
pushd $BuildDir

if [ -d "$PatchesRoot/$Lib" ]
then
    PatchedSourceDir="$SourceDir-patched"
    rsync -auv $SourceDir/ $PatchedSourceDir
    pushd $PatchedSourceDir

    for PATCH in $(ls "$PatchesRoot/$Lib")
    do
        $Git apply $PatchesRoot/$Lib/$PATCH --ignore-whitespace -v
    done

    popd
    SourceDir=$PatchedSourceDir
fi

echo $CMake $CMakeOptions $SourceDir
eval $CMake $CMakeGenerator $CMakeOptions -DCMAKE_PREFIX_PATH:STRING=$CMAKE_PREFIX_PATH $SourceDir
make -j4
make install

popd
