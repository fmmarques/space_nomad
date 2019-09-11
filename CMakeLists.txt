# CMakeFile for space_nomad
# Author: Frederico Marques
cmake_minimum_required(VERSION 2.6)
project(space_nomad)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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


find_package(SDL2_image)
if (NOT SDL2_IMAGE_FOUND)
  message(FATAL_ERROR "SDL2_image was not found on the system")
endif()
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
#target_link_libraries(yage ${SDL2_IMAGE_LIBRARIES})

find_package(SDL2_ttf REQUIRED)
if (NOT SDL2_TTF_FOUND)
  message(FATAL_ERROR "SDL2_TTF was not found on the system")
endif()
include_directories(${SDL2_TTF_INCLUDE_DIRS})
#target_link_libraries(yage ${SDL2_TTF_LIBRARIES})



# define target space_nomad sources. 
include_directories(AFTER SYSTEM ${YAGE_INCLUDE_DIR} 
	                         ${SDL2_IMAGE_INCLUDE_DIRS} 
				 include)
set(SPACE_NOMAD_SOURCES 
    src/bejeweled.cpp
  # states	
    src/states/main_menu.cpp
    src/states/base_game.cpp
  # widgets
    src/widgets/jewel.cpp
    src/widgets/grid.cpp
    src/widgets/grid_generators.cpp
    src/widgets/score.cpp
)
add_executable(space_nomad ${BEJEWELED_SOURCES}) 
set_property(TARGET space_nomad PROPERTY CXX_STANDARD 17)
set_target_properties(space_nomad
	PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/lib"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/lib"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/bin")
target_sources(space_nomad
  PRIVATE
    src/bejeweled.cpp
  # states	
    src/states/main_menu.cpp
    src/states/base_game.cpp
  # widgets
    src/widgets/jewel.cpp
    src/widgets/grid.cpp
    src/widgets/grid_generators.cpp
    src/widgets/score.cpp
  PUBLIC
    include/bejeweled.hpp
    include/bejeweled/engine.hpp
  # headers states
    include/bejeweled/states/main_menu.hpp
    include/bejeweled/states/base_game.hpp
  # headers widget 
    include/bejeweled/widgets/grid.hpp
    include/bejeweled/widgets/grid_event_listener.hpp
    include/bejeweled/widgets/grid_generators.hpp
    include/bejeweled/widgets/jewel.hpp
    include/bejeweled/widgets/score.hpp
)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -g")
# Dependencies

if (${YAGE_LIBRARY} STREQUAL "YAGE_LIBRARY-NOT_FOUND")
  set_property(TARGET yage PROPERTY POSITION_INDEPENDENT_CODE ON)
  set(BUILD_SHARED_LIBRARIES ON)
  add_dependencies(space_nomad yage)
endif()
target_link_libraries(space_nomad yage)

add_custom_target(assets_directory ALL COMMAND ${CMAKE_COMMAND} -E copy_directory assets $<TARGET_FILE_DIR:space_nomad>/assets)
#set_target_properties(assets_directory PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/bin")

#file(GLOB textures ${CMAKE_SOURCE_DIR}/assets/*.jpeg
#                   ${CMAKE_SOURCE_DIR}/assets/*.jpg
#                   ${CMAKE_SOURCE_DIR}/assets/*.png)

#add_custom_target(textures ALL) 
#foreach(texture ${textures})
#  add_custom_command(TARGET textures PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${texture} assets_directory)
#endforeach()
