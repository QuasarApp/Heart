#
# Copyright (C) 2018-2020 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.10)

include(../QuasarAppLib/CMake/ProjectOut.cmake)
include(../QuasarAppLib/CMake/Version.cmake)

add_subdirectory(Qt-Secret)


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



add_definitions(-DNETWORKPROTOCOL_LIBRARY)


find_package(Qt5 COMPONENTS Core Network Sql Concurrent REQUIRED)

file(GLOB SOURCE_CPP
    "*.cpp" "*.qrc"
    "AbstractSpace/*.cpp" "AbstractSpace/*.qrc"
    "BaseSpace/*.cpp" "BaseSpace/*.qrc"
    "UserSpace/*.cpp" "UserSpace/*.qrc"

    "AbstractSpace/Private/*.cpp" "AbstractSpace/Private/*.qrc"
    "BaseSpace/Private/*.cpp" "BaseSpace/Private/*.qrc"
    "UserSpace/Private/*.cpp" "UserSpace/Private/*.qrc"
)

add_library(${PROJECT_NAME} SHARED ${SOURCE_CPP})
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core Qt5::Network Qt5::Sql Qt5::Concurrent QuasarApp crypto ssl Qt-Secret)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/AbstractSpace")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/BaseSpace")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/UserSpace")

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/AbstractSpace/Private")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/BaseSpace/Private")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/UserSpace/Private")


setVersion(1 1 0)
