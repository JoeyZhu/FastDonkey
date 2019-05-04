cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(FastDonkey)
set( CMAKE_BUILD_TYPE Release )
SET("raspicam_DIR" "/usr/local/lib/cmake")
set (CMAKE_CXX_STANDARD 11)
set( CMAKE_BUILD_TYPE "Release" )
set( CMAKE_CXX_FLAGS "-std=c++11 -O3" )

find_package( Threads )
find_package(raspicam REQUIRED)
find_package(OpenCV REQUIRED PATHS "/usr/local/share")
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package( Threads )
find_package(OpenMP)
if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include_directories(/home/pi/samplecodes/ncnn/build/install/include/)
link_directories(/home/pi/samplecodes/ncnn/build/install/lib/)

add_executable (fastdonkey main.cpp)
add_executable (ncnn_test ncnn_test.cpp)


target_link_libraries (fastdonkey ${OpenCV_LIBS} ${Boost_LIBRARIES} ${raspicam_CV_LIBS} ${CMAKE_THREAD_LIBS_INIT} )

target_link_libraries(ncnn_test ncnn ${OpenCV_LIBS} )
