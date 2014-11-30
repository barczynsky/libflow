cmake_minimum_required(VERSION 2.8)

# -----------------------------------------------------------------------------
# CMake
# -----------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(AUX_CXX_FLAGS "-std=c++14 -Wall -Wextra -pthread")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${AUX_CXX_FLAGS}")
	message("\nC++ flags: ${CMAKE_CXX_FLAGS}")

	# set(CMAKE_SKIP_RPATH true)
	# set(CMAKE_USE_RELATIVE_PATH true)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,.")
endif()

# set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_BUILD_TYPE "MinSizeRel")
set(CMAKE_BUILD_TYPE "Release")
# set(CMAKE_BUILD_TYPE "RelWithDebInfo")


# -----------------------------------------------------------------------------
# find_package()
# -----------------------------------------------------------------------------
# find_package(OpenGL REQUIRED)
# include_directories(${OPENGL_INCLUDE_DIR})

# find_package(GLEW REQUIRED)
# include_directories(${GLEW_INCLUDE_DIR})


# -----------------------------------------------------------------------------
# include_directories()
# -----------------------------------------------------------------------------
include_directories("include")


# -----------------------------------------------------------------------------
# ${SRC}
# -----------------------------------------------------------------------------
# aux_source_directory("src" SRC)


# -----------------------------------------------------------------------------
# ${LIBS}
# -----------------------------------------------------------------------------
# set(LIBS ${LIBS} "${OPENGL_LIBRARIES}")
# set(LIBS ${LIBS} "${GLEW_LIBRARY}")


# -----------------------------------------------------------------------------
# add_executable() || add_library()
# -----------------------------------------------------------------------------
add_library(flow SHARED "src/flow.cpp")

add_executable(test1 "test/test1.cpp")
add_dependencies(test1 flow)
target_link_libraries(test1 flow)

add_executable(test2 "test/test2.cpp")
add_dependencies(test2 flow)
target_link_libraries(test2 flow)

