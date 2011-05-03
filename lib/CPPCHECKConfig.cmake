# - Configuration module for CPPCHECK to be used with FIND_PACKAGE().
# This sets the following variables:
#  CPPCHECK_FOUND
#
# The following COMPONENTS are available:
#  LIB
#  CLI
#  GUI
# Not specifying COMPONENT is equivalent to requesting LIB and CLI.
#
# The LIB component sets the following variables if successful:
#  CPPCHECK_INCLUDE_DIRS
#  CPPCHECK_LIBRARIES
#
# The CLI component and the GUI components set the variables
# CPPCHECK_EXECUTABLE and CPPCHECK_GUI_EXECUTABLE, respectively.
#
# By default this module sets CPPCHECK_LIBRARIES to use the shared
# (dynamically linked) libraries. Setting the variable CPPCHECK_USE_STATIC
# to TRUE makes the module set CPPCHECK_LIBRARIES to use the static library
# instead.
#

# internal guarded include macro
macro(_cppcheck_include_once filename)
  get_filename_component(_cppcheck_var "${filename}" NAME_WE)
  string(TOUPPER "CPPCHECK_${_cppcheck_var}_INCLUDED" _cppcheck_var)
  get_property(${_cppcheck_var} DIRECTORY PROPERTY ${_cppcheck_var} SET)
  if(NOT ${_cppcheck_var})
    include("${filename}")
    define_property(DIRECTORY PROPERTY ${_cppcheck_var} INHERITED
      BRIEF_DOCS "Include-guard for ${filename}"
      FULL_DOCS "Include-guard for ${filename}")
    set_directory_properties(PROPERTIES ${_cppcheck_var} TRUE)
  endif()
endmacro()

get_filename_component(_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_PREFIX "${_DIR}" PATH)
get_filename_component(_PREFIX "${_PREFIX}" PATH)
get_filename_component(_PREFIX "${_PREFIX}" PATH)

if(NOT DEFINED CPPCHECK_FIND_COMPONENTS)
  set(CPPCHECK_FIND_COMPONENTS LIB CLI)
endif()
foreach(comp ${CPPCHECK_FIND_COMPONENTS})
  if(NOT comp MATCHES "^(LIB|CLI|GUI)$")
    message(SEND_ERROR "Unsupported component '${comp}'")
  endif()
endforeach()

set(CPPCHECK_FOUND TRUE)

if(CPPCHECK_FIND_COMPONENTS MATCHES LIB)
  if(EXISTS "${_DIR}/libcppcheckExports.cmake")
    set(CPPCHECK_INCLUDE_DIR "${_PREFIX}/include/cppcheck")
    set(CPPCHECK_INCLUDE_DIRS "${CPPCHECK_INCLUDE_DIR}")
    set(CPPCHECK_LIBRARY_DIR "${_PREFIX}/lib")
    set(CPPCHECK_LIBRARY_DIRS "${CPPCHECK_LIBRARY_DIR}")

    set(CPPCHECK_STATIC_LIBRARY libcppcheck_static)
    set(CPPCHECK_SHARED_LIBRARY libcppcheck)
    if(CPPCHECK_USE_STATIC)
      set(CPPCHECK_LIBRARIES "${CPPCHECK_STATIC_LIBRARY}")
    else()
      set(CPPCHECK_LIBRARIES "${CPPCHECK_SHARED_LIBRARY}")
    endif()

    _cppcheck_include_once("${_DIR}/libcppcheckExports.cmake")
  else()
    set(CPPCHECK_FOUND FALSE)
    message(SEND_ERROR "Failed to find component LIB")
  endif()
endif()

if(CPPCHECK_FIND_COMPONENTS MATCHES CLI)
  if(EXISTS "${_DIR}/cppcheckCLIExports.cmake")
    _cppcheck_include_once("${_DIR}/cppcheckCLIExports.cmake")
    set(CPPCHECK_EXECUTABLE cppcheck)
  else()
    set(CPPCHECK_FOUND FALSE)
    message(SEND_ERROR "Failed to find component CLI")
  endif()
endif()

if(CPPCHECK_FIND_COMPONENTS MATCHES GUI)
  if(EXISTS "${_DIR}/cppcheckGUIExports.cmake")
    _cppcheck_include_once("${_DIR}/cppcheckGUIExports.cmake")
    set(CPPCHECK_GUI_EXECUTABLE cppcheck-gui)
  else()
    set(CPPCHECK_FOUND FALSE)
    message(SEND_ERROR "Failed to find component GUI")
  endif()
endif()

# ------------------------- vim: set sw=2 sts=2 et: --------------- end-of-file
