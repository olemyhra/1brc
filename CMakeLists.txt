#CMake standard configuration file
#Created by Ole Myhra, May 2024 - Sofia, Bulgaria

#This file is a standard small CMake file for C projects.
#Settings that is required for each projects is specified



cmake_minimum_required(VERSION 3.28)

set(PROJECT_NAME ole1)
project(${PROJECT_NAME} VERSION 1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED true)


add_executable(${PROJECT_NAME} ${PROJECT_NAME}.c)

set(MSVC_FLAGS
    /W3
    /std:c11
)

set(CLANG_FLAGS
    -Wall
)

set(GCC_FLAGS
	-Wall
	-Werror
	-std=c99
	-pedantic
	-O2
	-g
	-pg
)

set(GCC_LINK_FLAGS
	-pg
)

if(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    message(STATUS "${PROJECT_NAME}:Compiler is Microsoft Visual C")
    set(FLAGS ${MSVC_FLAGS})

elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(STATUS "${PROJECT_NAME}: Compiler is Clang")
    set(FLAGS ${CLANG_FLAGS})

elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
    message(STATUS "${PROJECT_NAME}: Compiler is GNU GCC")
    set(FLAGS ${GCC_FLAGS})

endif()

target_compile_options(${PROJECT_NAME} PRIVATE ${FLAGS})
target_link_options(${PROJECT_NAME} PRIVATE ${GCC_LINK_FLAGS})
