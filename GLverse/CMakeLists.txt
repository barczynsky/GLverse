cmake_minimum_required(VERSION 3.1)

include_directories(".")
aux_source_directory("." ${PROJECT_NAME}_SRC)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})
