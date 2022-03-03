#
# Copyright (C) 2018-2022 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.10)

project(Heart)
if(TARGET ${PROJECT_NAME})
  message("The ${PROJECT_NAME} arledy included in main Project")
  return()
endif()

include(QuasarAppLib/CMake/QuasarApp.cmake)
include(QuasarAppLib/CMake/Version.cmake)

if (DEFINED TARGET_PLATFORM_TOOLCHAIN)
    if (${TARGET_PLATFORM_TOOLCHAIN} STREQUAL "wasm32")
        initAll()
        return()
    endif()
endif()

if(NOT DEFINED HEART_BUILD_LVL)
    set(HEART_BUILD_LVL 1)
endif()

if (NOT DEFINED HEART_TESTS)
    set(HEART_TESTS ON)

    if (ANDROID OR IOS)
        set(HEART_TESTS OFF)
    endif()
endif()

if (NOT DEFINED HEART_DB_CACHE)
    set(HEART_DB_CACHE OFF)

    if (ANDROID OR WIN32)
        set(HEART_DB_CACHE OFF)
    endif()
endif()

# Use only for android debug builds with debugging from usb.
option(HEART_STATIC_SSL "This option enable or disabled static link ssl libraryes" OFF)
option(HEART_SSL "This option enable or disabled ssl functions of nodes" ON)

if (HEART_DB_CACHE)
    add_definitions(-DHEART_DB_CACHE)
endif()

# Add sub directories
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network Sql Concurrent REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network Sql Concurrent REQUIRED)

add_subdirectory(QuasarAppLib)
add_subdirectory(Heart)

if (HEART_TESTS)
    add_subdirectory(HeartTests)
endif()

initAll()

addDoc(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.conf)
