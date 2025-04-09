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

# Enable MKL for Eigen
add_definitions(-DEIGEN_USE_MKL_ALL)

# Find MKL (Intel oneAPI sets MKLROOT env var)
set(MKL_ROOT $ENV{MKLROOT})
find_package(MKL REQUIRED)

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

    # Link MKL to each target
    target_include_directories(${TEST_NAME} PRIVATE ${MKL_INCLUDE_DIRS})
    target_link_directories(${TEST_NAME} PRIVATE ${MKL_LIBRARY_DIRS})
    target_link_libraries(${TEST_NAME} ${MKL_LIBRARIES})
endforeach()
