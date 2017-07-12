cmake_minimum_required(VERSION 2.8)

# -----------------------------------------------------------------------------
# CMake preferences
# -----------------------------------------------------------------------------
set(projectName "ft-layout")

set(AUX_CXX_FLAGS "-std=c++14 -O2 -g -Wall -Wextra -pedantic -pthread -Wno-deprecated-register -fcolor-diagnostics -Wno-gnu-anonymous-struct")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${AUX_CXX_FLAGS}")


# -----------------------------------------------------------------------------
# find_package()
# -----------------------------------------------------------------------------
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})


# -----------------------------------------------------------------------------
# include_directories()
# -----------------------------------------------------------------------------
include_directories(".")
include_directories("libs/freetype/include")
include_directories("libs/glfw/include")


# -----------------------------------------------------------------------------
# ${SRC} - sources
# -----------------------------------------------------------------------------
aux_source_directory("src" SRC)


# -----------------------------------------------------------------------------
# ${LIBS} - libraries
# -----------------------------------------------------------------------------
set(LIBS ${LIBS} "${OPENGL_LIBRARIES}")
set(LIBS ${LIBS} "${GLEW_LIBRARY}")

# set(LIBS ${LIBS} "${CMAKE_SOURCE_DIR}/libs/glfw/lib/libglfw3.a")
set(LIBS ${LIBS} "/usr/lib/x86_64-linux-gnu/libglfw.so")
set(LIBS ${LIBS} "Xi;Xrandr;Xcursor;Xxf86vm")

# set(LIBS ${LIBS} "${CMAKE_SOURCE_DIR}/libs/freetype/lib/libfreetype.a")
set(LIBS ${LIBS} "freetype")
set(LIBS ${LIBS} "png;z")


# -----------------------------------------------------------------------------
# add_subdirectory() && add_executable()
# -----------------------------------------------------------------------------
add_subdirectory("SimpleTextLayout")
include_directories("SimpleTextLayout")
set(LIBS "${CMAKE_BINARY_DIR}/SimpleTextLayout/libSimpleTextLayout.a" ${LIBS})

add_executable(${projectName} ${SRC})
target_link_libraries(${projectName} ${LIBS})

add_dependencies(${projectName} "SimpleTextLayout")

message("\nLIBS: '${LIBS}'\nSRC:  '${SRC}'\n")
file(COPY "res/fonts" DESTINATION ${CMAKE_BINARY_DIR})
