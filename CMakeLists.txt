cmake_minimum_required (VERSION 2.8)
project (Service)

set (CMAKE_CXX_STANDARD 11)

add_executable (Service main.cpp service.cpp ZlibUtil.cpp)

# Indicate which libraries to include during the link process.
link_directories(/usr/lib/x86_64-linux-gnu/)

target_link_libraries (Service -lboost_system -lopencv_core -lopencv_imgcodecs -lz)

install (TARGETS Service DESTINATION bin)
