#
# Copyright (C) 2018-2025 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.18)


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


add_definitions(-DHEART_LIBRARY)

if (HEART_PRINT_SQL_QUERIES)
    add_definitions(-DHEART_PRINT_SQL_QUERIES)
endif()

if (HEART_PRINT_PACKAGES)
    add_definitions(-DHEART_PRINT_PACKAGES)
endif()

if (HEART_VALIDATE_PACKS)
    add_definitions(-DHEART_VALIDATE_PACKS)
endif()

set(SLL_DEFINE "WITHOUT_SSL")
if (HEART_SSL)
    set(SLL_DEFINE "USE_HEART_SSL")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/heart_global.h.in ${CMAKE_CURRENT_SOURCE_DIR}/heart_global.h @ONLY)

file(GLOB SOURCE_CPP
    "*.cpp" "*.h" "*.qrc"
    "public/*.cpp" "public/*.h" "public/*.qrc"
    "public/packages/*.cpp" "public/packages/*.h" "public/packages/*.qrc"
    "private/*.cpp" "private/*.h" "private/*.qrc"
)

set(PUBLIC_INCUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PUBLIC_INCUDE_DIR ${PUBLIC_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/public")
set(PUBLIC_INCUDE_DIR ${PUBLIC_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/public/packages")
set(PRIVATE_INCUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/private")


add_library(${PROJECT_NAME} ${SOURCE_CPP})
target_link_libraries(${PROJECT_NAME} PUBLIC Qt::Core Qt::Network Qt::Sql Qt::Concurrent QuasarApp crc )
if (HEART_SSL)
    target_link_libraries(${PROJECT_NAME} PUBLIC easyssl)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${PUBLIC_INCUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${PRIVATE_INCUDE_DIR})
