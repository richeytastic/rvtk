# =============================================================================
# The rVTK CMake configuration file.
#
#           ** File generated automatically, DO NOT MODIFY! ***

# To use from an external project, in your project's CMakeLists.txt add:
#   FIND_PACKAGE( rVTK REQUIRED)
#   INCLUDE_DIRECTORIES( rVTK ${rVTK_INCLUDE_DIRS})
#   LINK_DIRECTORIES( ${rVTK_LIBRARY_DIR})
#   TARGET_LINK_LIBRARIES( MY_TARGET_NAME ${rVTK_LIBRARIES})
#
# This module defines the following variables:
#   - rVTK_FOUND         : True if rVTK is found.
#   - rVTK_ROOT_DIR      : The root directory where rVTK is installed.
#   - rVTK_INCLUDE_DIRS  : The rVTK include directories.
#   - rVTK_LIBRARY_DIR   : The rVTK library directory.
#   - rVTK_LIBRARIES     : The rVTK imported libraries to link to.
#
# =============================================================================

get_filename_component( rVTK_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component( rVTK_ROOT_DIR  "${rVTK_CMAKE_DIR}"           PATH)

set( rVTK_INCLUDE_DIRS "${rVTK_ROOT_DIR}/include" CACHE PATH "The rVTK include directories.")
set( rVTK_LIBRARY_DIR  "${rVTK_ROOT_DIR}/lib"     CACHE PATH "The rVTK library directory.")

include( "${CMAKE_CURRENT_LIST_DIR}/Macros.cmake")
get_library_suffix( _lsuff)
set( _hints rVTK${_lsuff} librVTK${_lsuff})
find_library( rVTK_LIBRARIES NAMES ${_hints} PATHS "${rVTK_LIBRARY_DIR}/static" "${rVTK_LIBRARY_DIR}")
set( rVTK_LIBRARIES     ${rVTK_LIBRARIES}         CACHE FILE "The rVTK imported libraries to link to.")

# handle QUIETLY and REQUIRED args and set rVTK_FOUND to TRUE if all listed variables are TRUE
include( "${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake")
find_package_handle_standard_args( rVTK "Found:\t${rVTK_LIBRARIES}" rVTK_LIBRARIES rVTK_INCLUDE_DIRS)
