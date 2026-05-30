cmake_minimum_required(VERSION 3.20)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(softadastra_store
        VERSION 0.1.0
        DESCRIPTION "Softadastra Store - Key-value engine with WAL"
        LANGUAGES CXX
    )
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ========================
# Standalone fallback options
# ========================
if(NOT DEFINED SOFTADASTRA_WARNINGS_AS_ERRORS)
    option(SOFTADASTRA_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
endif()

if(NOT DEFINED SOFTADASTRA_UMBRELLA_BUILD)
    set(SOFTADASTRA_UMBRELLA_BUILD OFF)
endif()

if(SOFTADASTRA_UMBRELLA_BUILD)
    set(SOFTADASTRA_STORE_BUILD_TESTS ${SOFTADASTRA_BUILD_TESTS})
else()
    option(SOFTADASTRA_STORE_BUILD_TESTS "Build store tests" OFF)
endif()

option(SOFTADASTRA_STORE_BUILD_EXAMPLES "Build store examples" OFF)

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
    if(SOFTADASTRA_WARNINGS_AS_ERRORS)
        target_compile_options(softadastra_store PRIVATE /WX)
    endif()
else()
    target_compile_options(softadastra_store PRIVATE -Wall -Wextra -Wpedantic)
    if(SOFTADASTRA_WARNINGS_AS_ERRORS)
        target_compile_options(softadastra_store PRIVATE -Werror)
    endif()
endif()

# ========================
# Target properties
# ========================
set_target_properties(softadastra_store PROPERTIES
    OUTPUT_NAME softadastra_store
    VERSION 0.1.0
    SOVERSION 0
    EXPORT_NAME store
)

# ========================
# Install
# ========================
install(
    TARGETS softadastra_store
    EXPORT softadastraStoreTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT softadastraStoreTargets
    FILE softadastraStoreTargets.cmake
    NAMESPACE softadastra::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/softadastra_store"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/softadastra_storeConfigVersion.cmake"
    VERSION 0.1.0
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/softadastra_storeConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/softadastra_storeConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/softadastra_store"
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/softadastra_storeConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/softadastra_storeConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/softadastra_store"
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
message(STATUS "softadastra::store configured (0.1.0)")
message(STATUS "WAL target:      ${SOFTADASTRA_STORE_WAL_TARGET}")
message(STATUS "Include dir:     ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "Build tests:     ${SOFTADASTRA_STORE_BUILD_TESTS}")
message(STATUS "Build examples:  ${SOFTADASTRA_STORE_BUILD_EXAMPLES}")
message(STATUS "Warnings=error:  ${SOFTADASTRA_WARNINGS_AS_ERRORS}")
message(STATUS "Build type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "Install prefix:  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "------------------------------------------------------")
