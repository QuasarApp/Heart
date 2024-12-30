#
# Copyright (C) 2018-2025 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.18)

project(Heart)
if(TARGET ${PROJECT_NAME})
  message("The ${PROJECT_NAME} arledy included in main Project")
  return()
endif()

include(submodules/QuasarAppLib/CMake/QuasarApp.cmake)
include(submodules/QuasarAppLib/CMake/Version.cmake)

if (DEFINED TARGET_PLATFORM_TOOLCHAIN)
    if (${TARGET_PLATFORM_TOOLCHAIN} STREQUAL "wasm32")
        initAll()
        return()
    endif()
endif()

updateGitVars()
set(HEART_VERSION 1.3.${GIT_COMMIT_COUNT}.${GIT_COMMIT_HASH})

if (ANDROID OR IOS)
    option(HEART_TESTS "Enable or disable tests of the heart library" OFF)
else()
    option(HEART_TESTS "Enable or disable tests of the heart library" ON)
endif()
# Use only for android debug builds with debugging from usb.
option(HEART_STATIC_SSL "This option enable or disabled static link ssl libraryes" OFF)
option(HEART_PRINT_PACKAGES "This option enable or disabled log of add incoming network packages" OFF)
option(HEART_PRINT_SQL_QUERIES "This option enable or disabled log of all sql queries" OFF)
option(HEART_VALIDATE_PACKS "This option enable or disabled validation of child classes of the DataPack class" ON)

option(BUILD_SHARED_LIBS "Enable or disable shared libraryes" OFF)

if (WIN32)
    option(HEART_SSL "This option enable or disabled ssl functions of nodes" OFF)
else()
    option(HEART_SSL "This option enable or disabled ssl functions of nodes" ON)
endif()

# Add sub directories
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network Sql Concurrent REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network Sql Concurrent REQUIRED)

add_subdirectory(submodules/QuasarAppLib)
add_subdirectory(submodules/crc)

if (HEART_SSL)
    option(EASYSSL_TESTS "disable tests of the easyssl submodule " OFF)
    option(EASYSSL_STATIC_SSL "disable tests of the easyssl submodule " ${HEART_STATIC_SSL})

    add_subdirectory(submodules/easyssl)
endif()

add_subdirectory(src)

if (HEART_TESTS)
    add_subdirectory(HeartTests)
endif()

initAll()

configure_file_in(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.conf)
addDoc(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.conf)

