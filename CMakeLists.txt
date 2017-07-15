cmake_minimum_required(VERSION 3.1)
project(GLverse)

# -----------------------------------------------------------------------------
# CMake preferences
# -----------------------------------------------------------------------------
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)
# set(CMAKE_BUILD_TYPE MinSizeRel)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# -----------------------------------------------------------------------------
# Clang flags
# -----------------------------------------------------------------------------
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	set(AUX_CXX_FLAGS "${AUX_CXX_FLAGS} -Wall -Wextra -pedantic")
	set(AUX_CXX_FLAGS "${AUX_CXX_FLAGS} -fcolor-diagnostics")
	set(AUX_CXX_FLAGS "${AUX_CXX_FLAGS} -Wno-gnu-anonymous-struct")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${AUX_CXX_FLAGS}")
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 14)


# -----------------------------------------------------------------------------
# find_package(), include_directories()
# -----------------------------------------------------------------------------
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)


find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLEW 2.0 REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

find_package(Freetype 2.6 REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})


# -----------------------------------------------------------------------------
# include_directories()
# -----------------------------------------------------------------------------
include_directories(".")
include_directories("src")


# -----------------------------------------------------------------------------
# ${SRC}, aux_source_directory()
# -----------------------------------------------------------------------------
aux_source_directory("src" SRC)


# -----------------------------------------------------------------------------
# ${LIBS}
# -----------------------------------------------------------------------------
set(LIBS ${LIBS} "${OPENGL_LIBRARIES}")
set(LIBS ${LIBS} "${GLEW_LIBRARIES}")
set(LIBS ${LIBS} "${FREETYPE_LIBRARIES}")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(LIBS ${LIBS} "glfw")
	set(LIBS ${LIBS} "Xi;Xrandr;Xcursor;Xxf86vm")
endif()


# -----------------------------------------------------------------------------
# add_subdirectory(), add_executable()
# -----------------------------------------------------------------------------
add_subdirectory(${PROJECT_NAME})
include_directories(${PROJECT_NAME})


add_executable(demo.${PROJECT_NAME} ${SRC})
target_link_libraries(demo.${PROJECT_NAME} ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${LIBS})

add_dependencies(demo.${PROJECT_NAME} ${PROJECT_NAME})


file(COPY "data/fonts" DESTINATION ${CMAKE_BINARY_DIR})
message("")


# -----------------------------------------------------------------------------
# add_custom_target()
# -----------------------------------------------------------------------------
# if(${CMAKE_SYSTEM_NAME} MATCHES "Linux") # actually should match only Debian or Arch
# 	message(">> Use 'deps' target to install missing dependencies.")
# 	message("")
# 	add_custom_target(deps)
# endif()


# -----------------------------------------------------------------------------
# CMake summary, message()
# -----------------------------------------------------------------------------
message("-- SRC:   ${SRC}")
message("-- LIBS:  ${LIBS}")
message("")

