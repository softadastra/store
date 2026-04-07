cmake_minimum_required(VERSION 3.20)

project(softadastra_store
    VERSION 0.1.0
    DESCRIPTION "Softadastra Store - Key-value engine with WAL"
    LANGUAGES CXX
)

include(GNUInstallDirs)

# ========================
# Options
# ========================
option(SOFTADASTRA_STORE_BUILD_TESTS "Build store tests" OFF)
option(SOFTADASTRA_STORE_BUILD_EXAMPLES "Build store examples" OFF)

# ========================
# Global settings
# ========================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ========================
# Dependencies
# ========================
if(NOT SOFTADASTRA_UMBRELLA_BUILD)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.vix/vix_deps.cmake")
        include("${CMAKE_CURRENT_SOURCE_DIR}/.vix/vix_deps.cmake")
    endif()
endif()

if(NOT TARGET softadastra::wal AND NOT TARGET softadastra_wal)
    find_package(softadastra_wal CONFIG QUIET)
    find_package(wal CONFIG QUIET)
endif()

set(SOFTADASTRA_STORE_WAL_TARGET "")
if(TARGET softadastra::wal)
    set(SOFTADASTRA_STORE_WAL_TARGET softadastra::wal)
elseif(TARGET softadastra_wal)
    set(SOFTADASTRA_STORE_WAL_TARGET softadastra_wal)
endif()

if(SOFTADASTRA_STORE_WAL_TARGET STREQUAL "")
    message(FATAL_ERROR
        "softadastra wal target not found.\n"
        "Install dependencies with Vix:\n"
        "  vix add @softadastra/wal\n"
        "  vix install"
    )
endif()

# ========================
# Sources
# ========================
file(GLOB_RECURSE SOFTADASTRA_STORE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ========================
# Library
# ========================
add_library(softadastra_store ${SOFTADASTRA_STORE_SOURCES})
add_library(softadastra::store ALIAS softadastra_store)

target_compile_features(softadastra_store PUBLIC cxx_std_20)

target_link_libraries(softadastra_store
    PUBLIC
        ${SOFTADASTRA_STORE_WAL_TARGET}
)

# ========================
# Include directories
# ========================
target_include_directories(softadastra_store
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# ========================
# Compiler options
# ========================
if(MSVC)
    target_compile_options(softadastra_store PRIVATE /W4 /permissive-)
else()
    target_compile_options(softadastra_store PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ========================
# Target properties
# ========================
set_target_properties(softadastra_store PROPERTIES
    OUTPUT_NAME softadastra_store
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    EXPORT_NAME store
)

# ========================
# Examples
# ========================
if(SOFTADASTRA_STORE_BUILD_EXAMPLES AND NOT SOFTADASTRA_UMBRELLA_BUILD)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
        add_subdirectory(examples)
    else()
        message(STATUS "[store] examples/ missing CMakeLists.txt -> skipping")
    endif()
endif()

# ========================
# Tests
# ========================
if(SOFTADASTRA_STORE_BUILD_TESTS)
    include(CTest)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
        add_subdirectory(tests)
    else()
        message(STATUS "[store] tests/ missing CMakeLists.txt -> skipping")
    endif()
endif()

# ========================
# Summary
# ========================
message(STATUS "------------------------------------------------------")
message(STATUS "softadastra::store configured (${PROJECT_VERSION})")
message(STATUS "WAL target:      ${SOFTADASTRA_STORE_WAL_TARGET}")
message(STATUS "Include dir:     ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "Build tests:     ${SOFTADASTRA_STORE_BUILD_TESTS}")
message(STATUS "Build examples:  ${SOFTADASTRA_STORE_BUILD_EXAMPLES}")
message(STATUS "Build type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")
