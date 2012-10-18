cmake_minimum_required(VERSION 2.6)
set(TARGET loadtest_jq)
project(${TARGET})

include(../jq_find_3rd_parties.txt)

set(src loadtest_jq.cpp
	loadtest_task_scheduler.cpp
	#the following files should not be included like this, should be linked by binary
	../../lib/jq/task_scheduler.cpp
	../../lib/jq/task_scheduler.h
	../../lib/jq/active_task_scheduler.cpp
	../../lib/jq/active_task_scheduler.h
	../../lib/jq/io_service_pool.cpp
	../../lib/jq/io_service_pool.h
	../../lib/jq/key_exists_error.cpp
	../../lib/jq/key_exists_error.h
	)

include_directories(${Boost_INCLUDE_DIR})
#include_directories(${JQ_INCLUDE_DIR})
include_directories(/Users/jasonzha/github_image/cplusplus/lib/jq/)

add_executable(${TARGET}
	       ${src})

link_directories(${Boost_LIB_DIR})
#link_directories(${JQ_LIB_DIR})

target_link_libraries(${TARGET}
#		${JQ_LIBRARY}
		${Boost_THREAD_LIBRARY}
		${Boost_DATE_TIME_LIBRARY}
		${Boost_SYSTEM_LIBRARY}
		${Boost_LIBRARIES}
		${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
