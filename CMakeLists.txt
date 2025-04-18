cmake_minimum_required(VERSION 3.15)
set(CMAKE_C_COMPILER "icx")
set(CMAKE_CXX_COMPILER "icx")
project(FEProblems)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories for binaries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

# Define data directory path relative to project root
set(DATA_DIR "${CMAKE_SOURCE_DIR}/Data" CACHE PATH "Path to data directory")
if(NOT EXISTS "${DATA_DIR}")
    message(WARNING "Data directory not found: ${DATA_DIR}")
endif()
add_definitions(-DDATA_DIR="${DATA_DIR}")

# Include directories for headers
include_directories(include)
include_directories(lib/Eigen)  # Eigen is header-only

# Find Intel MKL
find_path(MKL_INCLUDE_DIR mkl.h
    HINTS "C:/Program Files (x86)/Intel/oneAPI/mkl/latest/include")
find_library(MKL_CORE_LIB mkl_core
    HINTS "C:/Program Files (x86)/Intel/oneAPI/mkl/latest/lib/intel64")
find_library(MKL_INTEL_LP64_LIB mkl_intel_lp64
    HINTS "C:/Program Files (x86)/Intel/oneAPI/mkl/latest/lib/intel64")
find_library(MKL_SEQUENTIAL_LIB mkl_sequential
    HINTS "C:/Program Files (x86)/Intel/oneAPI/mkl/latest/lib/intel64")

if(MKL_INCLUDE_DIR AND MKL_CORE_LIB)
    include_directories(${MKL_INCLUDE_DIR})
    set(MKL_LIBRARIES ${MKL_INTEL_LP64_LIB} ${MKL_SEQUENTIAL_LIB} ${MKL_CORE_LIB})
    add_definitions(-DEIGEN_USE_MKL_ALL)
else()
    message(FATAL_ERROR "Intel MKL not found. Please install Intel oneAPI Base Toolkit.")
endif()

# Find VTK
find_package(VTK 9.4 COMPONENTS
    CommonCore
    CommonDataModel
    IOXML
    RenderingCore
    RenderingOpenGL2
    InteractionStyle
    REQUIRED
)

# Add OpenXLSX as a subdirectory
add_subdirectory(lib/OpenXLSX)

# Collect all source files
file(GLOB SOURCE_FILES source/*.cpp)

# Collect all test files
file(GLOB TEST_FILES tests/actual/*.cpp)

# Create an executable for each test file
foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE} ${SOURCE_FILES})
    target_link_libraries(${TEST_NAME} PRIVATE OpenXLSX::OpenXLSX ${MKL_LIBRARIES} ${VTK_LIBRARIES})
    vtk_module_autoinit(
        TARGETS ${TEST_NAME}
        MODULES ${VTK_LIBRARIES}
    )
endforeach()

