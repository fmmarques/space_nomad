# dependencies.cmake
# author: Frederico Marques
# purpose: a useful mechanism to manage the multiple dependencies
# uses: include()

include_guard(GLOBAL)

# Required packages for this helper to run sucessfully
message(TRACE "dependencies.helper: about to find git.")
find_package(Git QUIET)
if(NOT GIT_FOUND)
  message(WARNING "git was not found. Automatic dependency resolution will not work")
endif()

# Helper functions.
# update_dependencies
# Updates a list of dependencies.
function(update_dependencies)
 if(NOT GIT_FOUND)
    message(FATAL_ERROR "update_dependencies: git was not found. Automatic dependency is not working.")
  endif()
#  git submodule foreach 'git remote update ; git stash ; git stash drop ; git checkout master ; git pull origin master'
  execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  RESULT_VARIABLE GIT_SUBMOD_RESULT)
  if(NOT GIT_SUBMOD_RESULT EQUAL "0")
    message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
  endif()
endfunction()


