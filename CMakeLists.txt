# Minimum required version
cmake_minimum_required (VERSION 3.12)

# Project definition
project(moneymaker LANGUAGES CXX)

# Set variables before including
set(ENABLE_ALL_WARNINGS "off")
set(ENABLE_EFFECTIVE_CXX "off")

# Include various cmake stuff for c++
include(cmake/compiler-options.cmake)
include(cmake/c++-standards.cmake)
include(cmake/code-coverage.cmake)
include(cmake/sanitizers.cmake)
include(cmake/tools.cmake)
include(cmake/link-time-optimization.cmake)

# Require c++20
cxx_20()

# Enable link time optimization
link_time_optimization(REQUIRED)

message(STATUS "Using ${CMAKE_CXX_COMPILER} (compiler id: ${CMAKE_CXX_COMPILER_ID})")

# Variables
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -march=native")
endif()

set(moneymaker_VERSION_STRING 0.1)
if(DEFINED ENV{Boost_DIR})
	set(BOOST_ROOT $ENV{Boost_DIR})
else()
	set(BOOST_ROOT "/home/matthijs/boost/boost_1_82_0")
endif()

# Find required libraries
find_package(Threads REQUIRED)
find_package(Boost 1.82.0 REQUIRED)
find_package(OpenSSL REQUIRED)
#find_package(date REQUIRED)
#find_package(Sqlpp11 REQUIRED)
#find_package(PostgreSQL REQUIRED)
#find_package(type_safe REQUIRED)
find_package(fmt REQUIRED)
#find_package(eve REQUIRED)

# Add compile definitons
#add_compile_definitions(BOOST_ALL_DYN_LINK)
#add_compile_definitions(BOOST_ASIO_ENABLE_HANDLER_TRACKING)

# Include directories
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
#include_directories(${OPENSSL_INCLUDE_DIR})

# Executable
add_executable(main main.cpp)

# Link
#target_link_libraries(main PUBLIC ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} fmt::fmt eve::eve)
target_link_libraries(main PUBLIC ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} Threads::Threads fmt::fmt /usr/local/lib/libboost_async.a)
