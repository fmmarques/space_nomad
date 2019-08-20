# Findyage.cmake 
# author: frederico marques
# purpose: Finds Yet-Another-Game-Engine

# _find_library
# Finds a given library
# _library_name - the library name
function(find_yage_library _root_dir)
  message(TRACE "Findyage.cmake::find_yage_library: enter.")
 
  if (NOT(IS_DIRECTORY ${_root_dir}))
    message(SEND_ERROR "Findyage.cmake::find_yage_library: the argument is not a valid directory.")
    message(TRACE "Findyage.cmake::find_yage_library: exit.")
    return()
  endif()

  message(TRACE "Findyage.cmake::find_yage_library: searching for the library within \"${_root_dir}; ${YAGE_ROOT_DIR};" ENV{YAGE_ROOT_DIT} "\"")
  find_file( YAGE_LIBRARY
	  yage
	  HINTS
	    ${_root_dir}
	    ${YAGE_ROOT_DIR}
	    ENV{YAGE_ROOT_DIR}
	  DOC
	    "yage's library")
  if (${YAGE_LIBRARY} STREQUAL "YAGE_LIBRARY-NOT_FOUND")
    message(NOTICE "Couldn't find the yage library from \"${YAGE_ROOT_DIR}")
    message(TRACE "Findyage.cmake::find_yage_library: exit.")
    return()
  endif()
  message(STATUS "Findyage.cmake::find_yage_library: setting YAGE_LIBRARY(${YAGE_LIBRARY}) in PARENT_SCOPE.")
  set(YAGE_LIBRARY ${YAGE_LIBRARY} PARENT_SCOPE)
  message(TRACE "Findyage.cmake::find_yage_library: exit.")
endfunction()

function(find_yage_include_dir _root_dir)
  message(TRACE "Findyage.cmake::find_yage_include_dir: enter.")
  
  if (NOT(IS_DIRECTORY ${_root_dir}))
    message(SEND_ERROR "Findyage.cmake::find_yage_include_dir: the argument given is not a valid directory.")
    message(TRACE "Findyage.cmake::find_yage_include_dir: exit.")
    return()
  endif()
  
  message(TRACE "Findyage.cmake::find_yage_include_dir: searching for the include directory within \"${_root_dir}; ${YAGE_ROOT_DIR};" ENV{YAGE_ROOT_DIT} "\"")
  find_file( YAGE_INCLUDE_DIR
	  include/yage.h
	  HINTS
	    ${_root_dir}
	    ${YAGE_ROOT_DIR}
	    ENV{YAGE_ROOT_DIR}
	  DOC
	    "yage's include directory")
  if (${YAGE_INCLUDE_DIR} STREQUAL "YAGE_INCLUDE_DIR-NOT_FOUND")
    message(SEND_ERROR "Couldn't find the yage include directory from \"${YAGE_ROOT_DIR}")
    message(TRACE "Findyage.cmake::find_yage_include_dir: exit.")
  endif()
  message(STATUS "Findyage.cmake::find_yage_include_dir: setting YAGE_INCLUDE_DIR(${YAGE_INCLUDE_DIR}) in PARENT_SCOPE.")
  set(YAGE_INCLUDE_DIR ${YAGE_INCLUDE_DIR} PARENT_SCOPE)
  message(TRACE "Findyage.cmake::find_yage_include_dir: exit.")
endfunction()

if (NOT DEFINED ${YAGE_ROOT_DIR} AND NOT DEFINED ENV{YAGE_ROOT_DIR})
  message(TRACE "yage's root directory is not defined. You might consider defining YAGE_ROOT_DIR to the directory where yage resides.")
  set(YAGE_ROOT_DIR ${PROJECT_SOURCE_DIR}/dependencies/yage CACHE PATH "yage's root directory")
endif()

find_yage_include_dir(${YAGE_ROOT_DIR})
find_yage_library(${YAGE_ROOT_DIR})
find_package_handle_standard_args(yage
	DEFAULT_MSG
	REQUIRED_VARS 
		YAGE_INCLUDE_DIR
		YAGE_LIBRARY)
	
