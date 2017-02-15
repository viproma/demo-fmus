#!/bin/bash
if [ "$1" = "-d" ]; then
    config="debug"
else
    config="release"
fi

for w in 32 64; do
    mFlag="-m$w"
    buildDir="build$w-$config"

    mkdir "$buildDir"
    pushd "$buildDir"
    cmake -DCMAKE_BUILD_TYPE=$config -DCMAKE_C_FLAGS=$mFlag -DCMAKE_CXX_FLAGS=$mFlag -DCMAKE_SHARED_LINKER_FLAGS=$mFlag .. || exit 2
    make || exit 3
    popd
done

fmuOutDir="build-fmu/$config"
mkdir "$fmuOutDir"
pushd $fmuOutDir
for d in `find . -maxdepth 1 -mindepth 1 -type d -printf '%f\n'`; do
    echo " "
    echo "Building $d.fmu"
    pushd "$d"
    zip -r "../$d.fmu" * || exit 4
    popd
done
popd
