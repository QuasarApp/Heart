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

if (${BUILD_LVL} GREATER_EQUAL 0)
    message("BUILD LVL >= 0")
    file(GLOB SOURCE_CPP_LVL_0
        "*.cpp" "*.qrc"
        "AbstractSpace/*.cpp" "AbstractSpace/*.qrc"
        "AbstractSpace/Private/*.cpp" "AbstractSpace/Private/*.qrc"
    )

    set(PUBLIC_INCUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    set(PUBLIC_INCUDE_DIR ${PUBLIC_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/AbstractSpace")
    set(PRIVATE_INCUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/AbstractSpace/Private")

endif()

if (${BUILD_LVL} GREATER_EQUAL 1)
    message("BUILD LVL >= 1")
    file(GLOB SOURCE_CPP_LVL_1
        "BaseSpace/*.cpp" "BaseSpace/*.qrc"
        "BaseSpace/Private/*.cpp" "BaseSpace/Private/*.qrc"
    )

    set(PUBLIC_INCUDE_DIR ${PUBLIC_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/BaseSpace")
    set(PRIVATE_INCUDE_DIR ${PRIVATE_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/BaseSpace/Private")

endif()

if (${BUILD_LVL} GREATER_EQUAL 2)
    message("BUILD LVL >= 2")
    file(GLOB SOURCE_CPP_LVL_2
        "UserSpace/*.cpp" "UserSpace/*.qrc"
        "UserSpace/Private/*.cpp" "UserSpace/Private/*.qrc"
    )


    set(PUBLIC_INCUDE_DIR ${PUBLIC_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/UserSpace")
    set(PRIVATE_INCUDE_DIR ${PRIVATE_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/UserSpace/Private")

endif()

set(SOURCE_CPP ${SOURCE_CPP_LVL_0} ${SOURCE_CPP_LVL_1} ${SOURCE_CPP_LVL_2})

add_library(${PROJECT_NAME} SHARED ${SOURCE_CPP})
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core Qt5::Network Qt5::Sql Qt5::Concurrent QuasarApp crypto ssl Qt-Secret)

target_include_directories(${PROJECT_NAME} PUBLIC ${PUBLIC_INCUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${PRIVATE_INCUDE_DIR})


setVersion(1 1 0)
