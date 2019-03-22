cmake_minimum_required(VERSION 3.10)
project(workspace)

set(CMAKE_CXX_STANDARD 11)

include_directories(/usr/include/mysql)
include_directories(/usr/local/lib)

link_libraries(mysqlclient)
link_directories(/usr/lib/x86_64-linux-gnu)

include_directories(../include)
include_directories(../include/nlohmann)

include_directories(/usr/local/include/botan-2)

file(GLOB SOURCE_FILES
        "../storage/*.cpp"
        )

file(GLOB HEADER_FILES
        "../storage/*.hpp"
        "../include/*.hpp"
        "../include/*/*.hpp"
        )

add_executable(workspace_temp main.cpp ${SOURCE_FILES} ${HEADER_FILES})

target_sources(workspace_temp
        PRIVATE
        ${SOURCE_FILES}
        PUBLIC
        ${HEADER_FILES}
        )

set(LIB_PREFIX "/usr/local/lib")
set(BOTAN_LIBS "${LIB_PREFIX}/libbotan-2.a")
target_include_directories(workspace_temp PRIVATE include/ /usr/local/include)
target_link_libraries(workspace_temp
        PRIVATE
        ${BOTAN_LIBS}
        )