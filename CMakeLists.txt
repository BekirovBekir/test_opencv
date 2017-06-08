project(test_strim)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)

include_directories(${BOOST_INCLUDE_DIRS})
include_directories( ${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ./project/main.cpp ./project/Binary_tracker.cpp ./project/TcpServer.cpp)

set(BOOST_LIB ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${GTK2_LIBRARIES} ${BOOST_LIB})


