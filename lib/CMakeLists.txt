# Instructions for building of the cppcheck library

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(LIBCPPCHECK)

list(APPEND CMAKE_MODULE_PATH "${LIBCPPCHECK_SOURCE_DIR}/../cmake/modules")

# for non-superbuild standalone use
find_package(PCRE)
if(PCRE_FOUND)
   add_definitions(-DHAVE_RULES)
   include_directories("${PCRE_INCLUDE_DIR}")
endif()

if(MSVC)
  find_package(Dumpbin REQUIRED)
endif()

set(CPPCHECK_LIB_DIR "")
include("library_sources.cmake")

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wshadow -Wno-long-long -Wfloat-equal -Wcast-qual")
endif (CMAKE_COMPILER_IS_GNUCXX)

set(CMAKE_INSTALL_NAME_DIR "@executable_path/../lib")

add_library(checks SHARED ${CPPCHECK_LIB_SOURCES})
add_library(static_checks STATIC ${CPPCHECK_LIB_SOURCES})
set_target_properties(static_checks PROPERTIES OUTPUT_NAME "checks")
set_target_properties(static_checks PROPERTIES PREFIX "lib")

if(NOT ${CMAKE_VERSION} VERSION_GREATER 2.8)
   set_target_properties(checks static_checks PROPERTIES
     CLEAN_DIRECT_OUTPUT 1)
endif()

if(MSVC)
  # generate /INCLUDE flags for LINK.EXE
  set(outfile "${LIBCPPCHECK_BINARY_DIR}/libcppcheckStaticLinkFlags.cmake")
  add_custom_command(OUTPUT "${outfile}"
    COMMAND "${CMAKE_COMMAND}" "-DDUMPBIN=${DUMPBIN_EXECUTABLE}"
    "-DOUTPUT=${outfile}" "-DLIBRARY=$<TARGET_FILE:static_checks>"
    -P "${LIBCPPCHECK_SOURCE_DIR}/generateStaticLinkFlags.cmake"
    DEPENDS "${LIBCPPCHECK_SOURCE_DIR}/generateStaticLinkFlags.cmake" static_checks
    COMMENT "Generating libcppcheckStaticLinkFlags.cmake"
    VERBATIM)
  add_custom_target(staticLinkFlags ALL DEPENDS "${outfile}")
  add_dependencies(staticLinkFlags static_checks)
  install(FILES "${outfile}" DESTINATION share/cppcheck/cmake)
endif()

if(PCRE_FOUND)
  target_link_libraries(checks ${PCRE_LIBRARIES})
  target_link_libraries(static_checks ${PCRE_LIBRARIES})
endif()

set(_PREFIX "{_PREFIX}") # trick to get ${_PREFIX} into the configured file
set(_DIR "{_DIR}") # ditto
if(APPLE)
  set(CONF_STATIC_LINK_FLAGS
    "set(LIBCPPCHECK_STATIC_LINK_FLAGS \"-Wl,-force_load,$${_PREFIX}/lib/libchecks.a\")")
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux OR CYGWIN OR MINGW)
  set(CONF_STATIC_LINK_FLAGS
    "set(LIBCPPCHECK_STATIC_LINK_FLAGS \"-Wl,--whole-archive,$${_PREFIX}/lib/libchecks.a,--no-whole-archive\")")
elseif(MSVC)
  set(CONF_STATIC_LINK_FLAGS "include(\"$${_DIR}/libcppcheckStaticLinkFlags.cmake\")")
else()
  message(SEND_ERROR "FIXME: Don't know how to do static linking")
endif()
configure_file(CPPCHECKConfig.cmake.in
  "${LIBCPPCHECK_BINARY_DIR}/CPPCHECKConfig.cmake" @ONLY)

install(TARGETS checks static_checks
  EXPORT libcppcheckExports
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(DIRECTORY "${LIBCPPCHECK_SOURCE_DIR}/"
  DESTINATION include/cppcheck
  FILES_MATCHING PATTERN *.h)

install(EXPORT libcppcheckExports
  DESTINATION share/cppcheck/cmake)

install(FILES "${LIBCPPCHECK_BINARY_DIR}/CPPCHECKConfig.cmake"
  DESTINATION share/cppcheck/cmake)
