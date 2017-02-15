cmake_minimum_required(VERSION 2.8.12)
project(fmus)

option(FMU_USE_STATIC_RUNTIME
    "Whether to link the model code against a static C(++) runtime library"
    ON)
set(FMU_OUTPUT_DIR "${CMAKE_BINARY_DIR}/fmu-contents"
    CACHE PATH "Where to put the generated FMU contents")

# The names of the FMUs built in this project
set(fmuNames "identity" "mass_1d" "sine" "spring_1d")

# ==============================================================================
# Compiler/platform specific settings
# ==============================================================================

if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options("-std=c++1y" "-Wall" "-Wextra")
    if(FMU_USE_STATIC_RUNTIME)
        add_compile_options("-static")
    endif()
elseif(MSVC)
    foreach(lang "C" "CXX")
        foreach(config "DEBUG" "MINSIZEREL" "RELEASE" "RELWITHDEBINFO")
            set(flagVar "CMAKE_${lang}_FLAGS_${config}")
            if(FMU_USE_STATIC_RUNTIME)
                string(REPLACE "/MD" "/MT" ${flagVar} ${${flagVar}})
            endif()
            # Treat warnings as errors and disable warning C4996
            set(${flagVar} "${${flagVar}} /WX /wd4996")
        endforeach()
    endforeach()
endif()

if(UNIX)
    # Remove the "lib" prefix on the generated .so files
    set(CMAKE_SHARED_MODULE_PREFIX )
endif()

# ==============================================================================
# Dependencies
# ==============================================================================

# CPPFMU
file(GLOB cppfmuSources "${CMAKE_SOURCE_DIR}/cppfmu/*.?pp")

# fmu-uuid
include(ExternalProject)
set(fmuUuidDir "${CMAKE_BINARY_DIR}/external")
ExternalProject_Add(fmu-uuid
    GIT_REPOSITORY "git@github.com:viproma/fmu-uuid.git"
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX:PATH=${fmuUuidDir}"
)
set(fmuUuidCommand "${fmuUuidDir}/bin/fmu-uuid")


# ==============================================================================
# FMUs
# ==============================================================================

# Detect platform
if("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
    set(os "darwin")
elseif("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    set(os "linux")
elseif(WIN32)
    set(os "win")
else()
    message(FATAL_ERROR "Unknown or unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()
math(EXPR wordSize 8*${CMAKE_SIZEOF_VOID_P})
set(fmuPlatform "${os}${wordSize}")

# Prepare names for FMU output directories.
if(CMAKE_CONFIGURATION_TYPES)
    set(fmuFullOutputDir "${FMU_OUTPUT_DIR}/$<CONFIGURATION>")
else()
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "release")
    endif()
    set(fmuFullOutputDir "${FMU_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}")
endif()

# Create targets
set(uuidOutputDir "${CMAKE_BINARY_DIR}/fmu-uuids")
set(fmuIncludeDirs
    "fmi"
    "cppfmu"
    ${uuidOutputDir}
)
foreach(fmuName IN LISTS fmuNames)
    set(dlTarget "${fmuName}")
    set(uuidTarget "${fmuName}_uuid")
    set(fmuTarget "${fmuName}_fmu")
    set(sourceDir "${CMAKE_SOURCE_DIR}/src/${fmuName}")
    set(outputDir "${fmuFullOutputDir}/${fmuName}")
    set(binariesDir "${outputDir}/binaries/${fmuPlatform}")
    set(sourceModelDescription "${sourceDir}/modelDescription.xml")

    # Target to generate UUID
    set(outputModelDescription "${uuidOutputDir}/${fmuName}-modelDescription.xml")
    set(outputUuidHeader "${uuidOutputDir}/${fmuName}-fmu-uuid.h")
    add_custom_command(
        OUTPUT "${outputModelDescription}" "${outputUuidHeader}"
        COMMAND
            "${fmuUuidCommand}"
            "${sourceModelDescription}"
            "@FMU_UUID@"
            "${outputModelDescription}"
            "${outputUuidHeader}"
            "FMU_UUID"
        DEPENDS "fmu-uuid-gen" "${sourceModelDescription}"
        VERBATIM
    )
    add_custom_target(${uuidTarget}
        DEPENDS "${outputModelDescription}" "${outputUuidHeader}"
    )

    # Target to build dynamic library
    set(sourceFile "${sourceDir}/sources/${fmuName}.c")
    if(EXISTS "${sourceFile}pp")
        set(sources "${sourceFile}pp" ${cppfmuSources} "${sourceModelDescription}")
    else()
        set(sources "${sourceFile}" "${sourceModelDescription}")
    endif()
    add_library(${dlTarget} MODULE ${sources})
    add_dependencies(${dlTarget} ${uuidTarget})
    target_compile_definitions(${dlTarget} PRIVATE
        "MODEL_IDENTIFIER=${fmuName}")
    target_include_directories(${dlTarget} PRIVATE ${fmuIncludeDirs})

    # Target to generate FMU contents
    set(ruleName "${fmuName}-contents")
    add_custom_command(OUTPUT "${ruleName}"
        COMMAND "${CMAKE_COMMAND}" "-E" "copy_directory" "${sourceDir}" "${outputDir}"
        COMMAND "${CMAKE_COMMAND}" "-E" "make_directory" "${binariesDir}"
        COMMAND "${CMAKE_COMMAND}" "-E" "copy" "$<TARGET_FILE:${dlTarget}>" "${binariesDir}/"
        COMMAND "${CMAKE_COMMAND}" "-E" "copy" "${outputModelDescription}" "${outputDir}/modelDescription.xml"
        COMMAND "${CMAKE_COMMAND}" "-E" "copy" "${outputUuidHeader}" "${outputDir}/sources/"
        DEPENDS ${dlTarget} ${uuidTarget}
        VERBATIM)
    set_property(SOURCE "${ruleName}" PROPERTY SYMBOLIC TRUE)
    add_custom_target(${fmuTarget} ALL DEPENDS "${ruleName}")
endforeach()

source_group("Metadata" REGULAR_EXPRESSION "modelDescription.xml")
