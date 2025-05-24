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

# Копирование всех .csv файлов из папки data в build/output
file(GLOB CSV_FILES "${CMAKE_SOURCE_DIR}/Data/*.csv")
file(COPY ${CSV_FILES} DESTINATION ${CMAKE_BINARY_DIR}/output/release)


# Find Intel MKL
find_path(MKL_INCLUDE_DIR mkl.h
    HINTS "C:/Program Files (x86)/Intel/oneAPI/mkl/latest/include")
find_library(MKL_CORE_LIB mkl_core
    HINTS "C:/Program Files (x86)/Intel/oneAPI/mkl/latest/lib/intel64")
find_library(MKL_INTEL_LP64_LIB mkl_intel_lp64
    HINTS "C:/Program Files (x86)/Intel/oneAPI/mkl/latest/lib/intel64")
find_library(MKL_INTEL_THREAD_LIB mkl_intel_thread
    HINTS "C:/Program Files (x86)/Intel/oneAPI/mkl/latest/lib/intel64")
# Find Intel OpenMP library
find_library(OPENMP_LIB libiomp5md
    HINTS "C:/Program Files (x86)/Intel/oneAPI/compiler/latest/windows/lib")

if(MKL_INCLUDE_DIR AND MKL_CORE_LIB AND MKL_INTEL_LP64_LIB AND MKL_INTEL_THREAD_LIB AND OPENMP_LIB)
    include_directories(${MKL_INCLUDE_DIR})
    # Form MKL libraries list with OpenMP
    set(MKL_LIBRARIES ${MKL_INTEL_LP64_LIB} ${MKL_INTEL_THREAD_LIB} ${MKL_CORE_LIB} ${OPENMP_LIB})
    add_definitions(-DEIGEN_USE_MKL_ALL)
    # Enable OpenMP
    find_package(OpenMP REQUIRED)
    if(OpenMP_CXX_FOUND)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        # Ensure OpenMP is linked
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    endif()
else()
    message(FATAL_ERROR "Intel MKL or OpenMP not found. Please install Intel oneAPI Base Toolkit and Compiler.")
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

# Find prebuilt OpenXLSX library
find_library(OPENXLSX_LIB
    NAMES OpenXLSX
    HINTS "${CMAKE_SOURCE_DIR}/lib/OpenXLSX/lib"
    REQUIRED
)

# Check for OpenXLSX headers
find_path(OPENXLSX_INCLUDE_DIR OpenXLSX.hpp
    HINTS "${CMAKE_SOURCE_DIR}/lib/OpenXLSX"
    REQUIRED
)

# Create an imported target for the prebuilt OpenXLSX library
add_library(OpenXLSX::OpenXLSX STATIC IMPORTED)
set_target_properties(OpenXLSX::OpenXLSX PROPERTIES
    IMPORTED_LOCATION "${OPENXLSX_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${OPENXLSX_INCLUDE_DIR};${CMAKE_SOURCE_DIR}/lib/OpenXLSX/headers"
)

# Collect all source files
file(GLOB SOURCE_FILES source/*.cpp)

# Collect all test files
file(GLOB TEST_FILES tests/actual/*.cpp)

# Create an executable for each test file
foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE} ${SOURCE_FILES})
    target_link_libraries(${TEST_NAME} PRIVATE OpenXLSX::OpenXLSX ${MKL_LIBRARIES} ${VTK_LIBRARIES} OpenMP::OpenMP_CXX)
    vtk_module_autoinit(
        TARGETS ${TEST_NAME}
        MODULES ${VTK_LIBRARIES}
    )

    # Copy libiomp5md.dll
    add_custom_command(TARGET ${TEST_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/Program Files (x86)/Intel/oneAPI/compiler/latest/bin/libiomp5md.dll"
        $<TARGET_FILE_DIR:${TEST_NAME}>
    )
endforeach()