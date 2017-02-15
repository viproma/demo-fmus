@echo off
setlocal EnableDelayedExpansion

if -%1-==-- (
    echo Usage: %~n0 ^<MSVC version^> [output directory]
    echo   MSVC version is 10 for VS2010, 11 for VS2012, etc. The output
    echo   directory will be created if it does not exist. The default name
    echo   is "build-vs-output".
    exit /B 1
)
set vsVersion=%1
if -%2-==-- (
    set outputDir=build-vs-output
) else (
    set outputDir=%2
)

set sourceDir=%~dp0
set fmuDir=fmu-vs%vsVersion%

md %outputDir%
pushd %outputDir%

for /l %%w in (32,32,64) do (
    set cmakeGenerator=Visual Studio !vsVersion!
    if %%w==64 set cmakeGenerator=!cmakeGenerator! Win64

    set buildDir=build-vs!vsVersion!-win%%w
    md !buildDir!
    pushd !buildDir!
    cmake -G "!cmakeGenerator!" "-DFMU_OUTPUT_DIR=..\!fmuDir!" "!sourceDir!"
    if !errorLevel! neq 0 exit /B 2
    popd
)

for %%c in (Debug Release) do (
    for /l %%w in (32,32,64) do (
        set buildDir=build-vs!vsVersion!-win%%w
        pushd !buildDir!
        cmake --build . --config %%c
        if !errorLevel! neq 0 exit /B 3
        popd
    )

    pushd !fmuDir!\%%c
    for /d %%d in (*) do (
        echo(
        echo Building %%d.fmu...
        pushd %%d
        !sourceDir!\tools\win\zip.exe -r ..\%%d.fmu *
        if !errorLevel! neq 0 exit /B 4
        popd
    )
    popd
)

popd
