cmake_minimum_required (VERSION 2.6)
set (TARGET idiot_shooting)
project (${TARGET})

set (CMAKE_CXX_FLAGS "-W -Wall ${CMAKE_CXX_FLAGS}")

set (BOOST_ROOT /Users/jasonzha/Dev/3rd_party/boost_1_51_0)
set (Boost_LIB_DIR ${BOOST_ROOT}/lib)
set (Boost_USE_STATIC_LIBS ON)
find_package (Boost COMPONENTS system thread date_time REQUIRED)
include_directories (${Boost_INCLUDE_DIR})

set (QT_QMAKE_EXECUTABLE /Users/jasonzha/QtSDK/Desktop/Qt/4.8.1/gcc/bin/qmake)
set(QT_USE_QTOPENGL TRUE)
find_package (Qt4 REQUIRED)
include (${QT_USE_FILE})
add_definitions (${QT_DEFINITIONS})

set (qt_to_be_moced_headers GLWidget.h)
qt4_wrap_cpp (qt_moced ${qt_to_be_moced_headers})

message( status, ${qt_moced} )

set (srcs
     GLWidget.h
     GLWidget.cpp
     Model.h
     Model.cpp
     main.cpp)

include_directories (${CMAKE_BINARY_DIR})
add_executable (${TARGET}
		${srcs}
		${qt_moced})
link_directories (${Boost_ROOT}/lib)
target_link_libraries (${TARGET}
		       ${Boost_THREAD_LIBRARY}
		       ${Boost_DATE_TIME_LIBRARY}
		       ${Boost_SYSTEM_LIBRARY}
		       ${QT_LIBRARIES})
