cmake_minimum_required(VERSION 2.8)

set(projectName "SimpleTextLayout")

include_directories(".")
aux_source_directory("." LIB_SRC)

add_library(${projectName} ${LIB_SRC})
