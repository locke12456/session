cmake_minimum_required(VERSION 2.8)

INCLUDE(os_env.cmake)
INCLUDE(boost.cmake)
if(UNIX)
	add_definitions("-DLINUX -O2 -s -Wall")
endif(UNIX)

if(WIN32)
	if(MSVC_VERSION EQUAL 1700)
	  add_definitions(-D_VARIADIC_MAX=10)
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0501")
	# force this option to ON so that Google Test will use /MD instead of /MT
	# /MD is now the default for Visual Studio, so it should be our default, too
	#option(gtest_force_shared_crt ON)
endif(WIN32)

file(GLOB_RECURSE SESSION_FILES
	inc/*.h 
	inc/*.hpp
	src/Session.cpp
	src/SharedMemory.cpp
	src/dllmain.cpp
)
file(GLOB_RECURSE TEST_API_FILES
	test/test_api.cpp
)
include_directories(inc)
ADD_DEFINITIONS(-DSHM_MANAGER_NAME="manager")
add_library(session SHARED 
	${SESSION_FILES}
	${TEST_API_FILES}
)

target_link_libraries(session ${Boost_LIBRARIES})
IF(UNIX)

target_link_libraries(session pthread)

target_link_libraries(session rt)
ENDIF(UNIX)

add_executable( test_shm test/main.cpp
)
target_link_libraries(test_shm session )
#target_link_libraries(test_shm ${Boost_LIBRARIES})