#
# Copyright (C) 2018-2019 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.10)
project(NetworkProtocol)
if(TARGET ${PROJECT_NAME})
  message("The ${PROJECT_NAME} arledy included in main Project")
  return()
endif()

find_package(Qt5 COMPONENTS Core Test REQUIRED)
if(NOT DEFINED BUILD_LVL)
    set(BUILD_LVL 1)
endif()

include(QuasarAppLib/CMake/ccache.cmake)

# Add sub directories
add_subdirectory(QuasarAppLib)
add_subdirectory(NetworkProtocol)
#add_subdirectory(ProtocolTests)

include(QuasarAppLib/CMake/QuasarAppCITargets.cmake)
initAll()

addDoc(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.conf)
