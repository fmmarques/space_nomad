# CMakeFile for bejeweled
# Author: Frederico Marques
cmake_minimum_required(VERSION 2.6)
project(bejeweled)

set(CMAKE_MODULE_PATH cmake)



# Includes 
include(dependencies.helper)

# Update dependencies through git
update_dependencies()
set(YAGE_ROOT_DIR ${PROJECT_SOURCE_DIR}/dependencies/yage)
find_package(yage)
if (NOT YAGE_FOUND)
  if (IS_DIRECTORY ${PROJECT_SOURCE_DIR}/dependencies/yage ) 
    set(YAGE_INCLUDE_DIR ${YAGE_ROOT_DIR}/include CACHE PATH "yage's include dir" FORCE)
    add_subdirectory(${YAGE_ROOT_DIR})
    message(STATUS "include_directories(dependencies/yage/include)")
  endif()
endif()

# define target bejeweled sources. 
include_directories(AFTER SYSTEM 
	${YAGE_INCLUDE_DIR}
	include)
set(BEJEWELED_SOURCES src/bejeweled.cpp)
add_executable(bejeweled ${BEJEWELED_SOURCES}) 
set_property(TARGET bejeweled PROPERTY CXX_STANDARD 17)
set_target_properties(bejeweled
	PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/lib"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/lib"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/bin")

# Dependencies

if (${YAGE_LIBRARY} STREQUAL "YAGE_LIBRARY-NOT_FOUND")
  set_property(TARGET yage PROPERTY POSITION_INDEPENDENT_CODE ON)
  set(BUILD_SHARED_LIBRARIES ON)
  add_dependencies(bejeweled yage)
endif()
