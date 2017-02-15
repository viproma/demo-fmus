#!/bin/bash

# Usage: build-make.sh  [output directory]
#
# The output directory will be created if it does not exist.
# If not specified, it defaults to "build-make-output".
set -e

if [ -z "$1" ]; then
    readonly outputDir="build-make-output"
else
    readonly outputDir="$1"
fi

readonly relSourceDir=`dirname $0`
readonly sourceDir=`cd $relSourceDir; pwd`

mkdir -p "$outputDir"
pushd "$outputDir"

for c in debug release; do
    fmuDir="fmu"
    for w in 32 64; do
        mFlag="-m$w"
        buildDir="build-$w-$c"

        mkdir -p "$buildDir"
        pushd "$buildDir"
        cmake -DCMAKE_BUILD_TYPE=$c \
              -DCMAKE_C_FLAGS=$mFlag \
              -DCMAKE_CXX_FLAGS=$mFlag \
              -DCMAKE_SHARED_LINKER_FLAGS=$mFlag \
              -DFMU_OUTPUT_DIR="../$fmuDir" \
              $sourceDir
        make
        popd # $buildDir
    done

    pushd "$fmuDir/$c"
    for d in `find . -maxdepth 1 -mindepth 1 -type d -printf '%f\n'`; do
        echo " "
        echo "Building $d.fmu"
        pushd "$d"
        zip -r "../$d.fmu" * || exit 4
        popd
    done
    popd # $fmuDir
done

popd # $outputDir
