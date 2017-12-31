cmake_minimum_required(VERSION 2.8)
project(my_database_logger)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

find_package(g3logger REQUIRED)

include_directories(~/software/program/database/sqlite/SQLiteCpp/SQLiteCpp/include)
link_directories(~/software/program/database/sqlite/SQLiteCpp/SQLiteCpp/build)
link_directories(~/software/program/database/sqlite/SQLiteCpp/SQLiteCpp/build/sqlite3)

add_executable(test_database_logger src/main.cpp
  src/logfile_sink.cc
  src/sqlite3_sink.cc
  src/my_sqlite3_table.cc
  src/my_database_log_format.cc
  )
target_link_libraries(test_database_logger g3logger
  SQLiteCpp
  sqlite3)


