SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
SET(CMAKE_ASM_COMPILER arm-linux-gnueabihf-gcc)
SET(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

cmake_minimum_required (VERSION 2.4) 
enable_language(C)
project (power_test)

IF(NOT CMAKE_BUILD_TYPE )
   SET( CMAKE_BUILD_TYPE "Release" )
ENDIF()

# Threads
find_package(Threads)

# WinSock
if(WIN32)
#set(WINSOCK ws2_32)
endif()

include_directories("c:/SysGCC/Raspberry/arm-linux-gnueabihf/include/")

set (SRCS
    main.c ComPort.c GPIO.c ModemSARA.c FrameBuffer.c
)

add_executable (${PROJECT_NAME} ${SRCS})  

target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})


