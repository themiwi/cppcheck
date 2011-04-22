cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

project(CPPCHECK)

option(CPPCHECK_CLI_USE_STATIC_LIBRARY
  "Link CLI against static cppcheck library." OFF)
option(CPPCHECK_GUI_USE_STATIC_LIBRARY
  "Link GUI against static cppcheck library." OFF)
mark_as_advanced(CPPCHECK_CLI_USE_STATIC_LIBRARY
  CPPCHECK_GUI_USE_STATIC_LIBRARY)

list(APPEND CMAKE_MODULE_PATH "${CPPCHECK_SOURCE_DIR}/cmake/modules")

find_package(PCRE)
if(NOT PCRE_FOUND)
  message("PCRE not found, disabling user-defined rules")
endif()

find_package(Qt4 COMPONENTS QtCore QtGui QtHelp QtMain QtXml)

# helper function
include(ExternalProject)
include(CMakeParseArguments)
_ep_get_configuration_subdir_suffix(CFG_DIR)

# This function creates an ExternalProject for the named subdirectory. It takes
# the following optional arguments:
#  DEPENDS   : List of other ExternalProjects this subdirectory depends on
#  VARIABLES : List of variable names to be passed to the ExternalProject. The
#              entries take the form "VARNAME[=VARVALUE]", where the assignment
#              part is optional. If it is not present, the value from the
#              current scope is used. You must not pass lists in the assignment
#              form.
function(cppcheck_add_subdirectory name)
  # argument parsing
  cmake_parse_arguments(cas "" "" "DEPENDS;VARIABLES" ${ARGN})
  if(cas_DEPENDS)
    set(dep DEPENDS ${cas_DEPENDS})
  else()
    set(dep)
  endif()
  set(cmargs)
  foreach(v IN LISTS cas_VARIABLES)
    if(NOT v MATCHES =)
      set(v "${v}=${${v}}")
    endif()
    list(APPEND cmargs "-D${v}")
  endforeach()
  # add external project
  ExternalProject_Add(${name}
    ${dep}
    PREFIX "${CMAKE_BINARY_DIR}/${name}"
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${name}"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/inst${CFG_DIR}"
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>;${cmargs}"
    )
  ExternalProject_Add_Step(${name} forceconfigure
    COMMAND ${CMAKE_COMMAND} -E echo "Force re-configure for later builds..."
    DEPENDEES download
    DEPENDERS configure
    ALWAYS 1
    )
endfunction()

# set up arguments for external projects
set(EP_COMMON_VARS
  CMAKE_BUILD_TYPE
  CMAKE_CXX_COMPILER CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG
  CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO
  CMAKE_CXX_FLAGS_MINSIZEREL
  PCRE_INCLUDE_DIR PCRE_PCRE_LIBRARY PCRE_PCREPOSIX_LIBRARY)

set(EP_LIB_VARS
  ${EP_COMMON_VARS}
  CMAKE_SHARED_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS_DEBUG
  CMAKE_SHARED_LINKER_FLAGS_RELEASE CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
  CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL
  )
if(MSVC)
  find_package(Dumpbin REQUIRED)
  list(APPEND EP_LIB_VARS DUMPBIN_EXECUTABLE)
endif()

set(EP_CLI_VARS
  ${EP_COMMON_VARS}
  CPPCHECK_CLI_USE_STATIC_LIBRARY
  CMAKE_EXE_LINKER_FLAGS CMAKE_EXE_LINKER_FLAGS_DEBUG
  CMAKE_EXE_LINKER_FLAGS_RELEASE CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
  CMAKE_EXE_LINKER_FLAGS_MINSIZEREL
  )

cppcheck_add_subdirectory(lib VARIABLES ${EP_LIB_VARS})
cppcheck_add_subdirectory(cli DEPENDS lib VARIABLES ${EP_CLI_VARS})

if(QT4_FOUND)
  string(REPLACE "CPPCHECK_CLI_USE_STATIC_LIBRARY"
    "CPPCHECK_GUI_USE_STATIC_LIBRARY" EP_GUI_VARS "${EP_CLI_VARS}")
  get_cmake_property(vars CACHE_VARIABLES)
  foreach(v IN LISTS vars)
    if(v MATCHES "^QT4?_")
      list(APPEND EP_GUI_VARS ${v})
    endif()
  endforeach()
  cppcheck_add_subdirectory(gui DEPENDS lib VARIABLES ${EP_GUI_VARS})
else()
  message("GUI not built since QT4 not found.")
endif()

## Exclude tests from command line targets but include them to VS IDE targets.
## There is 'make check' -target for compiling and running tests from
## command line.
#IF (MSVC_IDE)
#  ADD_SUBDIRECTORY(test)
#ELSE (MSVC_IDE)
#  ADD_SUBDIRECTORY(test EXCLUDE_FROM_ALL)
#ENDIF (MSVC_IDE)

# installation
set(INST_DIR "${CMAKE_BINARY_DIR}/inst${CFG_DIR}")
install(DIRECTORY ${INST_DIR}/ # leave that trailing slash where it is!
  DESTINATION ${CMAKE_INSTALL_PREFIX} USE_SOURCE_PERMISSIONS)
