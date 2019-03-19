cmake_minimum_required(VERSION 3.10)
project(workspace)

set(CMAKE_CXX_STANDARD 14)

include_directories(/usr/include/mysql)
link_libraries(mysqlclient)
link_directories(/usr/lib/x86_64-linux-gnu)

include_directories(../include)
include_directories(../include/nlohmann)
include_directories(../include/sha256)

add_executable(workspace_temp main.cpp ../storage/db_lib.cpp ../storage/merkle_lib.cpp ../storage/storage_lib.cpp ../include/sha256/sha256.cpp)









