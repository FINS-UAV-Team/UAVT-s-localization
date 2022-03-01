cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 14)

project(Acquisition)
project(Callibration)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
message(Target will be generated under ${EXECUTABLE_OUTPUT_PATH})

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(Acquisition main.cpp)
target_link_libraries(Acquisition ${OpenCV_LIBS})
target_link_libraries(Acquisition Boost::system boost_date_time boost_filesystem)

add_executable(Callibration fisheye.cpp)
target_link_libraries(Callibration ${OpenCV_LIBS})
