cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 14)

project(Acquisition)
project(Callibration)
project(Localization)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
message(Target will be generated under ${EXECUTABLE_OUTPUT_PATH})

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(apriltag REQUIRED)
find_package(Eigen3 REQUIRED)

set(apriltag_INCLUDE_DIRS "/usr/local/apriltags/include/apriltag")
set(apriltag_LIBS "/usr/local/apriltags/lib/libapriltag.so.3")

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${apriltag_INCLUDE_DIRS})

add_executable(Acquisition Acquisition.cpp)
target_link_libraries(Acquisition ${OpenCV_LIBS})
target_link_libraries(Acquisition Boost::system boost_date_time boost_filesystem)
target_link_libraries(Acquisition "/lib/libMVSDK.so")

add_executable(Callibration Fisheye.cpp)
target_link_libraries(Callibration ${OpenCV_LIBS})

add_executable(Localization Localization.cpp Undistort.hpp)
target_link_libraries(Localization ${OpenCV_LIBS} ${apriltag_LIBS})
target_link_libraries(Localization "/lib/libMVSDK.so")
