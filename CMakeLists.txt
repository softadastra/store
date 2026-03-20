cmake_minimum_required(VERSION 3.20)

project(softadastra_store
    VERSION 0.1.0
    DESCRIPTION "Softadastra Store - Key-value engine with WAL"
    LANGUAGES CXX
)

# ========================
# Options
# ========================
option(SOFTADASTRA_STORE_BUILD_TESTS "Build store tests" ON)
option(SOFTADASTRA_STORE_BUILD_EXAMPLES "Build store examples" ON)

# ========================
# Library
# ========================
add_library(softadastra_store)

target_compile_features(softadastra_store PUBLIC cxx_std_20)

# ========================
# Dependencies
# ========================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.vix/vix_deps.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/.vix/vix_deps.cmake)
endif()

if(NOT TARGET softadastra::wal)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../wal/CMakeLists.txt)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../wal build_wal)
    else()
        message(FATAL_ERROR
            "softadastra::wal not found. "
            "Install dependencies with Vix or provide ../wal in the monorepo."
        )
    endif()
endif()

target_link_libraries(softadastra_store
    PUBLIC
        softadastra::wal
)

# ========================
# Include directories
# ========================
target_include_directories(softadastra_store
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# ========================
# Sources
# ========================
file(GLOB_RECURSE STORE_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

target_sources(softadastra_store
    PRIVATE
        ${STORE_SOURCES}
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
# Position Independent Code
# ========================
set_target_properties(softadastra_store PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# ========================
# Namespacing
# ========================
add_library(softadastra::store ALIAS softadastra_store)

# ========================
# Examples
# ========================
if(SOFTADASTRA_STORE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ========================
# Tests
# ========================
if(SOFTADASTRA_STORE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
