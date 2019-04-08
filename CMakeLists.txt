cmake_minimum_required(VERSION 3.13)
project(workspace)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system thread filesystem serialization)

include_directories(/usr/include/mysql)
include_directories(/usr/local/lib)

link_libraries(mysqlclient)
link_directories(/usr/lib/x86_64-linux-gnu)

include_directories(include)
include_directories(include/nlohmann)
include_directories(lib/leveldb)
include_directories(/usr/local/include/botan-2)

add_subdirectory(lib/leveldb)

file(GLOB SOURCE_FILES
        "storage/*.cpp"
        "include/*.cpp"
        )

file(GLOB HEADER_FILES
        "chain/*.hpp"
        "storage/*.hpp"
        "include/*.hpp"
        "include/*/*.hpp"
        "utils/*.hpp"
        )

add_executable(workspace_temp test/main.cpp ${SOURCE_FILES} ${HEADER_FILES})

target_sources(workspace_temp
        PRIVATE
        ${SOURCE_FILES}
        PUBLIC
        ${HEADER_FILES}
        )

IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
ENDIF (Boost_FOUND)

set(LIB_PREFIX "/usr/local/lib")
set(LZ4_LIBS "${LIB_PREFIX}/liblz4.a")
set(BOTAN_LIBS "${LIB_PREFIX}/libbotan-2.a")
target_include_directories(workspace_temp PRIVATE include/ /usr/local/include)
target_link_libraries(workspace_temp
        PRIVATE
        ${Boost_LIBRARIES}
        leveldb
        ${LZ4_LIBS}
        ${BOTAN_LIBS}
        )