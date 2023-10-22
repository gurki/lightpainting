cmake_minimum_required( VERSION 3.25 )
set( CMAKE_CXX_STANDARD 23 )

project( lightpainting
    VERSION 1.0.0
    LANGUAGES CXX
)
add_executable( ${PROJECT_NAME}
    src/keys.cpp
    src/main.cpp
    src/timer.cpp
)

find_package( OpenCV REQUIRED )

target_compile_definitions( ${PROJECT_NAME} PRIVATE
    ROOT_PATH="""${CMAKE_SOURCE_DIR}/"""
)

target_include_directories( ${PROJECT_NAME} PRIVATE
    src/
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries( ${PROJECT_NAME} PRIVATE
    ${OpenCV_LIBS}
)