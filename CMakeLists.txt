cmake_minimum_required(VERSION 3.6)
project(lightpainting)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

find_package( OpenCV REQUIRED )

include_directories( ./src/ )
aux_source_directory( ./src/ SRC_LIST )
add_executable( ${PROJECT_NAME} ${SRC_LIST} )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )