cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

project(CPPCHECK)

option(CPPCHECK_CLI_USE_STATIC_LIBRARY
  "Link CLI against static cppcheck library." OFF)
mark_as_advanced(CPPCHECK_CLI_USE_STATIC_LIBRARY)

list(APPEND CMAKE_MODULE_PATH "${CPPCHECK_SOURCE_DIR}/cmake/modules")

find_package(PCRE)

if(NOT PCRE_FOUND)
  message("PCRE not found, disabling user-defined rules")
endif()

if(MSVC)
  find_program(DUMPBIN_EXECUTABLE dumpbin.exe
    PATHS
    "[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\VisualStudio\\10.0;InstallDir]/../../VC/bin"
    "[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\VisualStudio\\9.0;InstallDir]/../../VC/bin"
    "[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\VisualStudio\\8.0;InstallDir]/../../VC/bin"
    "[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\VisualStudio\\7.0;InstallDir]/../../VC7/bin"
    "[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\VisualStudio\\6.0\\Setup\\Microsoft Visual C++;ProductDir]/bin"
    )
  if(NOT DUMPBIN_EXECUTABLE)
    message(SEND_ERROR "Failed to find DUMPBIN_EXECUTABLE")
  endif()
endif()

include(ExternalProject)

# set up arguments for external projects
set(configs "" _DEBUG _RELEASE _RELWITHDEBINFO _MINSIZEREL)
set(EP_CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>)
foreach(lang C CXX)
  list(APPEND EP_CMAKE_ARGS
    -DCMAKE_${lang}_COMPILER=${CMAKE_${lang}_COMPILER})
  foreach(conf IN LISTS configs)
    list(APPEND EP_CMAKE_ARGS
      -DCMAKE_${lang}_FLAGS${conf}=${CMAKE_${lang}_FLAGS${conf}})
  endforeach()
endforeach()
foreach(conf IN LISTS configs)
  foreach(type EXE MODULE SHARED)
    list(APPEND EP_CMAKE_ARGS
      -DCMAKE_${type}_LINKER_FLAGS${conf}=${CMAKE_${type}_LINKER_FLAGS${conf}})
  endforeach()
endforeach()
list(APPEND EP_CMAKE_ARGS
  -DPCRE_INCLUDE_DIR=${PCRE_INCLUDE_DIR}
  -DPCRE_PCRE_LIBRARY=${PCRE_PCRE_LIBRARY}
  -DPCRE_PCREPOSIX_LIBRARY=${PCRE_PCREPOSIX_LIBRARY}
  -DPCRE_FOUND=${PCRE_FOUND}
  -DCPPCHECK_CLI_USE_STATIC_LIBRARY=${CPPCHECK_CLI_USE_STATIC_LIBRARY})

ExternalProject_Add(libcppcheck
  PREFIX "${CPPCHECK_BINARY_DIR}/lib"
  SOURCE_DIR "${CPPCHECK_SOURCE_DIR}/lib"
  INSTALL_DIR "${CPPCHECK_BINARY_DIR}/inst"
  CMAKE_ARGS "${EP_CMAKE_ARGS}"
  )

ExternalProject_Add(cppcheck
  DEPENDS libcppcheck
  PREFIX "${CPPCHECK_BINARY_DIR}/cli"
  SOURCE_DIR "${CPPCHECK_SOURCE_DIR}/cli"
  INSTALL_DIR "${CPPCHECK_BINARY_DIR}/inst"
  CMAKE_ARGS "${EP_CMAKE_ARGS}"
  )

#ADD_SUBDIRECTORY(lib)
#ADD_SUBDIRECTORY(cli)
#ADD_SUBDIRECTORY(gui)
#
## Exclude tests from command line targets but include them to VS IDE targets.
## There is 'make check' -target for compiling and running tests from
## command line.
#IF (MSVC_IDE)
#  ADD_SUBDIRECTORY(test)
#ELSE (MSVC_IDE)
#  ADD_SUBDIRECTORY(test EXCLUDE_FROM_ALL)
#ENDIF (MSVC_IDE)
