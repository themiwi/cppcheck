#- Contains utilities for the buildsystem of CPPCHECK
#
#  function CPPCHECK_ADD_SUBDIRECTORY(<name>
#                                     [SOURCE_DIR <dir>]
#                                     [DEPENDS <project> ...]
#                                     [VARIABLES <varname>[=<value>] ...]
#                                     [NO_INSTALL])
# This function creates an ExternalProject for the named sub-directory. It
# takes the following optional arguments:
#  SOURCE_DIR : Name of the directory if different from the target name.
#  DEPENDS    : List of other ExternalProjects this subdirectory depends on
#  VARIABLES  : List of variable names to be passed to the ExternalProject. The
#               entries take the form "VARNAME[=VARVALUE]", where the
#               assignment part is optional. If it is not present, the value
#               from the current scope is used. You must not pass lists in the
#               assignment form.
#  NO_INSTALL : If specified, skip the install step.

include(ExternalProject)
include(CMakeParseArguments)
_ep_get_configuration_subdir_suffix(CFG_DIR)

function(cppcheck_add_subdirectory name)
  # argument parsing
  cmake_parse_arguments(cas "NO_INSTALL" "SOURCE_DIR" "DEPENDS;VARIABLES" ${ARGN})
  set(instcmd)
  if(cas_NO_INSTALL)
    set(instcmd INSTALL_COMMAND "")
  endif()
  if(NOT cas_SOURCE_DIR)
    set(cas_SOURCE_DIR ${name})
  endif()
  if(NOT IS_ABSOLUTE "${cas_SOURCE_DIR}")
    set(cas_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${cas_SOURCE_DIR}")
  endif()
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
  # 2.8.4 warns about unused CLI-defined variables...
  if(${CMAKE_VERSION} VERSION_GREATER 2.8.3)
    set(cmwarnopts --no-warn-unused-cli)
  else()
    set(cmwarnopts)
  endif()
  # add external project
  ExternalProject_Add(${name}
    ${dep}
    PREFIX "${CMAKE_BINARY_DIR}/${name}"
    SOURCE_DIR "${cas_SOURCE_DIR}"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/inst${CFG_DIR}"
    CMAKE_ARGS "${cmwarnopts};-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>;${cmargs}"
    "${instcmd}"
    )
  # remove bogus directory created by ExternalProject_Add
  if(CFG_DIR)
    file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/inst${CFG_DIR}")
  endif()
  ExternalProject_Add_Step(${name} forceconfigure
    COMMAND ${CMAKE_COMMAND} -E echo "Force re-configure for later builds..."
    DEPENDEES download
    DEPENDERS configure
    ALWAYS 1
    )
endfunction()
