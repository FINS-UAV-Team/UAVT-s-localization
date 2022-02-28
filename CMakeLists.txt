cmake_minimum_required(VERSION 3.16)
project(CVTest)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(CVTest main.cpp)
target_link_libraries(CVTest ${OpenCV_LIBS})
target_link_libraries(CVTest Boost::system boost_date_time boost_filesystem)
