cmake_minimum_required(VERSION 3.15)
project(FEProblems)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories for binaries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

# Include directories for headers
include_directories(include)
include_directories(lib/Eigen)  # Eigen is header-only

# Add OpenXLSX as a subdirectory
add_subdirectory(lib/OpenXLSX)

# MKL Configuration ----------------------------------------------------------
option(USE_MKL "Use Intel MKL for Eigen" ON)

if(USE_MKL)
    # Set explicit paths for oneAPI MKL
    set(MKL_ROOT "$ENV{MKLROOT}" CACHE PATH "Path to Intel MKL")
    if(NOT MKL_ROOT)
        set(MKL_ROOT "C:/Program Files (x86)/Intel/oneAPI/mkl/latest")
    endif()

    set(MKL_INCLUDE_DIRS "${MKL_ROOT}/include")
    set(MKL_LIBRARY_DIRS "${MKL_ROOT}/lib/intel64")

    # Find MKL components
    find_path(MKL_INCLUDE_DIR mkl.h PATHS ${MKL_INCLUDE_DIRS} REQUIRED)
    find_library(MKL_CORE_LIB NAMES mkl_core PATHS ${MKL_LIBRARY_DIRS} REQUIRED)
    find_library(MKL_SEQUENTIAL_LIB NAMES mkl_sequential PATHS ${MKL_LIBRARY_DIRS} REQUIRED)
    find_library(MKL_INTEL_ILP64_LIB NAMES mkl_intel_ilp64 PATHS ${MKL_LIBRARY_DIRS} REQUIRED)

    if(MKL_INCLUDE_DIR AND MKL_CORE_LIB AND MKL_SEQUENTIAL_LIB AND MKL_INTEL_ILP64_LIB)
        message(STATUS "MKL found at: ${MKL_ROOT}")
        message(STATUS "MKL includes: ${MKL_INCLUDE_DIR}")
        message(STATUS "MKL libraries: ${MKL_CORE_LIB};${MKL_SEQUENTIAL_LIB};${MKL_INTEL_ILP64_LIB}")
        
        add_definitions(-DEIGEN_USE_MKL_ALL)
        include_directories(${MKL_INCLUDE_DIR})
        
        set(MKL_LIBRARIES 
            ${MKL_INTEL_ILP64_LIB}
            ${MKL_SEQUENTIAL_LIB}
            ${MKL_CORE_LIB}
            pthread
            m
            dl
        )
    else()
        message(WARNING "MKL not found - falling back to standard Eigen")
        set(USE_MKL OFF)
    endif()
endif()

# Compiler optimization flags
if (CMAKE_CXX_COMPILER_ID MATCHES "IntelLLVM|Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O3 /QxHost /Qopenmp")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /openmp")
endif()

# Collect all source files (definitions of classes/methods)
file(GLOB SOURCE_FILES source/*.cpp)

# Collect all test files (*.cpp) from tests/actual/ folder
file(GLOB TEST_FILES tests/actual/*.cpp)

# Create an executable for each test file
foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    add_executable(${TEST_NAME} ${TEST_FILE} ${SOURCE_FILES})

    # Link OpenXLSX
    target_link_libraries(${TEST_NAME} OpenXLSX::OpenXLSX)

    # Link MKL if available
    if(USE_MKL)
        target_include_directories(${TEST_NAME} PRIVATE ${MKL_INCLUDE_DIR})
        target_link_directories(${TEST_NAME} PRIVATE ${MKL_LIBRARY_DIRS})
        target_link_libraries(${TEST_NAME} ${MKL_LIBRARIES})
    endif()

    # Add OpenMP support
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(${TEST_NAME} PRIVATE /openmp)
    else()
        find_package(OpenMP REQUIRED)
        target_link_libraries(${TEST_NAME} PRIVATE OpenMP::OpenMP_CXX)
    endif()
endforeach()

# Windows-specific settings
if(WIN32)
    # Add resource compiler for Windows builds
    set(CMAKE_RC_COMPILER "C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64/rc.exe")
    enable_language(RC)
endif()