#
# Copyright (C) 2018-2023 QuasarApp.
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

include(submodules/QuasarAppLib/CMake/QuasarApp.cmake)
include(submodules/QuasarAppLib/CMake/Version.cmake)

if (DEFINED TARGET_PLATFORM_TOOLCHAIN)
    if (${TARGET_PLATFORM_TOOLCHAIN} STREQUAL "wasm32")
        initAll()
        return()
    endif()
endif()

updateGitVars()
set(HEART_VERSION 1.2.${GIT_COMMIT_COUNT}.${GIT_COMMIT_HASH})

if (ANDROID OR IOS)
    option(HEART_TESTS "Enable or disable tests of the heart library" OFF)
else()
    option(HEART_TESTS "Enable or disable tests of the heart library" ON)
endif()
# Use only for android debug builds with debugging from usb.
option(HEART_STATIC_SSL "This option enable or disabled static link ssl libraryes" OFF)
option(CMAKE_SHARE "This option enable or disabled ssl functions of nodes" OFF)
option(PRINT_SQL_QUERIES "This option enable or disabled log of all sql queries" OFF)

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
add_subdirectory(src)

if (HEART_TESTS)
    add_subdirectory(HeartTests)
endif()

initAll()

configure_file_in(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.conf)
addDoc(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.conf)

