cmake_minimum_required (VERSION 3.10)
project(pipes
        VERSION 0.0.1)

OPTION(BUILD_TESTS ON)

#########################
########## LIB ##########
#########################

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

add_library(${PROJECT_NAME} INTERFACE)
add_library(tillh::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
target_include_directories(${PROJECT_NAME} INTERFACE include)

add_subdirectory(test)