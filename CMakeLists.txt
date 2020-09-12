cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

SET(PROJECT_NAME life)

# Set name of the project
project(${PROJECT_NAME}
        VERSION 1.0.0
        DESCRIPTION "Implementation of Conway's Game of Life using C++ and SDL"
        LANGUAGES CXX
        )

# Set various compiler flags
set(CMAKE_CXX_STANDARD 17)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    message(STATUS "GCC is detected, adding flags")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lm")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

    # Set debug flags
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3 -DDEBUG")

    # Set release flags
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)

if (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    message(STATUS "MSVC is detected, adding flags")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

    # Set debug flags
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /DEBUG /DDEBUG")

    # Set release flags
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
endif (CMAKE_CXX_COMPILER_ID MATCHES MSVC)

# Our own include directory
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
include_directories(${COMMON_INCLUDES})

# Source files
file(GLOB INTERNAL_SOURCE ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB INTERNAL_HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)

# Create the executable from our sources
add_executable(${PROJECT_NAME} ${INTERNAL_SOURCE} ${INTERNAL_HEADERS})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

target_link_libraries(${PROJECT_NAME} SDL2::Main SDL2::Image SDL2::TTF)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})