cmake_minimum_required(VERSION 3.10)
project(workspace)

set(CMAKE_CXX_STANDARD 14)

include_directories(/usr/include/mysql)
link_libraries(mysqlclient)
link_directories(/usr/lib/x86_64-linux-gnu)

include_directories(/usr/local/include/nlohmann)
include_directories(../lib)

add_executable(workspace_temp main.cpp ../lib/db_lib.hpp)






