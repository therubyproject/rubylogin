cmake_minimum_required(VERSION 3.8)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(rubylogin)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(cotire)

add_compile_options(-Wall -Werror -pipe -fvisibility=hidden)

if (CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-fno-strict-aliasing)
endif()

include(FindCXX17)
include(FindLTO)

# Find packages.
find_package(Crypto++ REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(MySQL REQUIRED)
find_package(Threads REQUIRED)

if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-pagezero_size 10000 -image_base 100000000")
endif()

find_package(Boost 1.63.0 COMPONENTS system log filesystem iostreams REQUIRED)

add_subdirectory(src)
add_executable(rubylogin ${rubylogin_SRC})

add_definitions(-DBOOST_LOG_DYN_LINK)

include_directories(${MYSQL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${Crypto++_INCLUDE_DIR})
target_link_libraries(rubylogin ${MYSQL_CLIENT_LIBS} ${Boost_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY} ${Crypto++_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} yaml-cpp stdc++fs)

set_target_properties(rubylogin PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/includes.h")
set_target_properties(rubylogin PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(rubylogin)
