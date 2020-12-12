cmake_minimum_required(VERSION 3.10.2)

# ------------------------------------------------------------------------------
# Project name and version

project(MVox VERSION 0.1.0)

# Prohibit in-source build
if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
  message(FATAL_ERROR
    "MVox does not support in-source CMake builds at this time.")
endif (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})

# MVOX_DEBUG
if (CMAKE_BUILD_TYPE MATCHES "Debug|debug|DEBUG")
  set(MVox_DEBUG ON)
else()
  set(MVox_DEBUG OFF)
endif()

# ------------------------------------------------------------------------------
# Compilers

# Require C++ standard and disable compiler-specific extensions
set(CMAKE_CXX_STANDARD 14)      # C++14 is minimum required for ITK
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------------------
# Find ITK

message(STATUS "Finding ITK...")
find_package(ITK REQUIRED)
message(STATUS "Found ITK: ${ITK_DIR} (version ${ITK_VERSION})")
include(${ITK_USE_FILE})

# ------------------------------------------------------------------------------
# Find MFEM

# The following variables can be used to help CMake find MFEM:
#  * MFEM_DIR - absolute path to the MFEM build or install prefix.
#  * mfem_DIR - absolute path to where MFEMConfig.cmake is.
message(STATUS "Finding MFEM...")
set(MFEM_DIR "" CACHE PATH "Path to the MFEM build or install prefix.")
if (MFEM_DIR)
   find_package(mfem REQUIRED NAMES MFEM HINTS "${MFEM_DIR}"
                "${MFEM_DIR}/lib/cmake/mfem" NO_DEFAULT_PATH)
else()
   find_package(mfem REQUIRED NAMES MFEM)
endif()
message(STATUS "Found MFEM config in: ${mfem_DIR} (version ${MFEM_VERSION})")
# Use the same C++ compiler as MFEM. This is needed when MFEM was built using
# an MPI wrapper and we do not have explicitly the MPI compile and link flags.
if (NOT CMAKE_CXX_COMPILER AND MFEM_CXX_COMPILER)
  set(CMAKE_CXX_COMPILER "${MFEM_CXX_COMPILER}")
endif()

# Include paths and libraries needed by MFEM
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MFEM_CXX_FLAGS}")
include_directories(${MFEM_INCLUDE_DIRS})

# ------------------------------------------------------------------------------
# Configuration file

configure_file(config/config.h.in config/config.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# ------------------------------------------------------------------------------
# Project Library

add_library(libmvox STATIC
    src/fileutil.cpp
    src/mfemutil.cpp
)

# To avoid conflict between the executable and the library
# we call them mvox and libmvox but CMake prepends lib so
# we remove the prefix here.
set_target_properties(libmvox PROPERTIES PREFIX "")

target_include_directories(libmvox
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# ------------------------------------------------------------------------------
# Project Executables

# bin/mvox
add_executable(mvox app/mvox.cpp)
target_link_libraries(mvox
    libmvox
    ${ITK_LIBRARIES}
    ${MFEM_LIBRARIES}
)

install(TARGETS mvox RUNTIME)

# ------------------------------------------------------------------------------
# Information

message(STATUS "****************************************************************************")
message(STATUS "* MVox was configured as follows...")

# MVox
message(STATUS "* Build type: CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "* Version: ${MVOX_VERSION_STRING}")
message(STATUS "* Git string: ${MVOX_GIT_STRING}")

# Dependencies
message(STATUS "* Using MFEM: ${MFEM_DIR} (version ${MFEM_VERSION})")
message(STATUS "* Using ITK: ${ITK_DIR} (version ${ITK_VERSION})")

message(STATUS "****************************************************************************")
