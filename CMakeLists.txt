cmake_minimum_required(VERSION 2.8)

INCLUDE(os_env.cmake)
INCLUDE(boost.cmake)

file(GLOB_RECURSE SESSION_FILES
	inc/*.h 
	inc/*.hpp
	src/Session.cpp
	src/SharedMemory.cpp
	src/dllmain.cpp
)
include_directories(inc)
ADD_DEFINITIONS(-DSHM_MANAGER_NAME="manager")
add_library(seassion SHARED 
	${SESSION_FILES}
)

target_link_libraries(seassion ${Boost_LIBRARIES})
IF(UNIX)

target_link_libraries(seassion pthread)

target_link_libraries(seassion rt)
ENDIF(UNIX)

add_executable( test_shm test/main.cpp
)
target_link_libraries(test_shm seassion )
#target_link_libraries(test_shm ${Boost_LIBRARIES})