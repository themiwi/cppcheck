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

find_package(Qt4 COMPONENTS QtMain QtCore QtGui QtHelp QtMain QtXml)

# helper function
include(CPPCHECKUtilities)

# set up arguments for external projects
set(EP_LIB_VARS
  CMAKE_BUILD_TYPE
  CMAKE_CXX_COMPILER CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG
  CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO
  CMAKE_CXX_FLAGS_MINSIZEREL
  CMAKE_EXE_LINKER_FLAGS CMAKE_EXE_LINKER_FLAGS_DEBUG
  CMAKE_EXE_LINKER_FLAGS_RELEASE CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
  CMAKE_EXE_LINKER_FLAGS_MINSIZEREL
  CMAKE_SHARED_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS_DEBUG
  CMAKE_SHARED_LINKER_FLAGS_RELEASE CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
  CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL
  PCRE_INCLUDE_DIR PCRE_PCRE_LIBRARY PCRE_PCREPOSIX_LIBRARY
  )
if(MSVC)
  find_package(Dumpbin REQUIRED)
  list(APPEND EP_LIB_VARS DUMPBIN_EXECUTABLE)
endif()

set(EP_CLI_VARS ${EP_LIB_VARS} CPPCHECK_CLI_USE_STATIC_LIBRARY)

set(EP_TEST_VARS ${EP_LIB_VARS} CPPCHECK_TEST_USE_STATIC_LIBRARY)

cppcheck_add_subdirectory(lib VARIABLES ${EP_LIB_VARS})
cppcheck_add_subdirectory(cli DEPENDS lib VARIABLES ${EP_CLI_VARS})

if(QT4_FOUND)
  set(EP_GUI_VARS ${EP_LIB_VARS} CPPCHECK_GUI_USE_STATIC_LIBRARY)
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

cppcheck_add_subdirectory(cppcheck-test SOURCE_DIR test DEPENDS lib
  VARIABLES ${EP_TEST_VARS} NO_INSTALL)
ExternalProject_Get_Property(cppcheck-test binary_dir)
if(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
  set(checkcmd "$(MAKE)" -C "${binary_dir}" check)
else()
  set(checkcmd "${CMAKE_COMMAND}" --build "${binary_dir}"
    --config ${CMAKE_CFG_INTDIR} --target check)
endif()
add_custom_target(check COMMAND ${checkcmd})
add_dependencies(check cppcheck-test)

# installation
set(INST_DIR "${CMAKE_BINARY_DIR}/inst${CFG_DIR}")
install(DIRECTORY ${INST_DIR}/ # leave that trailing slash where it is!
  DESTINATION ${CMAKE_INSTALL_PREFIX} USE_SOURCE_PERMISSIONS)
