cmake_minimum_required(VERSION 3.15)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(pseudoparallel)

set(CLIENT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/client)
set(SERVER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/server)
set(SHARED_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/shared)

set(PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/project)

set(CLIENT_NAME ${PROJECT_NAME}-client)
add_executable(${CLIENT_NAME}
    ${CLIENT_SRC_DIR}/Main.cpp)

set(SERVER_NAME ${PROJECT_NAME}-server)
add_executable(${SERVER_NAME}
    ${SERVER_SRC_DIR}/Main.cpp)

target_include_directories(${CLIENT_NAME} PRIVATE ${SHARED_SRC_DIR})
target_include_directories(${SERVER_NAME} PRIVATE ${SHARED_SRC_DIR})