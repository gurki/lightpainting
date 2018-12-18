cmake_minimum_required(VERSION 3.6)
project(lte)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package( OpenCV REQUIRED )

include_directories( ./source/ )
aux_source_directory( ./source/ SRC_LIST )
add_executable( ${PROJECT_NAME} ${SRC_LIST} )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )