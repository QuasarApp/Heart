#
# Copyright (C) 2018-2020 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.10)

set(CURRENT_PROJECT ${PROJECT_NAME}Test)

include(../QuasarAppLib/CMake/ProjectOut.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DBUILD_LVL=${BUILD_LVL})


if (${BUILD_LVL} GREATER_EQUAL 0)
    message("BUILD LVL >= 0")
    file(GLOB SOURCE_CPP_LVL_0
        "*.cpp" "*.qrc"
        "AbstractSpace/*.cpp" "AbstractSpace/*.qrc"
    )
    set(PUBLIC_INCUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    set(PUBLIC_INCUDE_DIR ${PUBLIC_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/AbstractSpace")
endif()

if (${BUILD_LVL} GREATER_EQUAL 1)
    message("BUILD LVL >= 1")
    file(GLOB SOURCE_CPP_LVL_1
        "DataBaseSpace/*.cpp" "DataBaseSpace/*.qrc"
    )
    set(PUBLIC_INCUDE_DIR ${PUBLIC_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/DataBaseSpace")

endif()

if (${BUILD_LVL} GREATER_EQUAL 2)
    message("BUILD LVL >= 2")
    file(GLOB SOURCE_CPP_LVL_2
        "UserSpace/*.cpp" "UserSpace/*.qrc"
    )
    set(PUBLIC_INCUDE_DIR ${PUBLIC_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/UserSpace")

endif()

set(SOURCE_CPP ${SOURCE_CPP_LVL_0} ${SOURCE_CPP_LVL_1} ${SOURCE_CPP_LVL_2})

message(SOURCE_CPP = ${SOURCE_CPP})

add_executable(${CURRENT_PROJECT} ${SOURCE_CPP})
target_link_libraries(${CURRENT_PROJECT} PRIVATE Qt5::Test NetworkProtocol)

target_include_directories(${CURRENT_PROJECT} PUBLIC ${PUBLIC_INCUDE_DIR})
target_include_directories(${CURRENT_PROJECT} PRIVATE ${PRIVATE_INCUDE_DIR})


include(../QuasarAppLib/CMake/QuasarAppCITargets.cmake)
initTests()
addTests("NetworkProtokol" ${CURRENT_PROJECT})
