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
  set(outfile "${LIBCPPCHECK_BINARY_DIR}/LIBCPPCHECKStaticLinkFlags.cmake")
  add_custom_target(OUTPUT "${outfile}"
    COMMAND "${CMAKE_COMMAND}" "-DDUMPBIN=${DUMPBIN_EXECUTABLE}"
    "-DOUTPUT=${outfile}" "-DLIBRARY=$<TARGET_FILE:static_checks>"
    -P "${LIBCPPCHECK_SOURCE_DIR}/generateStaticLinkFlags.cmake"
    COMMENT "Generating LIBCPPCHECKStaticLinkFlags.cmake"
    VERBATIM)
  add_custom_target(staticLinkFlags ALL DEPENDS "${outfile}")
  add_dependencies(staticLinkFlags static_checks)
  install(FILES "${outfile}" DESTINATION share/LIBCPPCHECK/cmake)
endif()

if(PCRE_FOUND)
  message("DEBUG: PCRE_LIBRARIES = ${PCRE_LIBRARIES}")
  target_link_libraries(checks ${PCRE_LIBRARIES})
  target_link_libraries(static_checks ${PCRE_LIBRARIES})
endif()

set(CONF_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/cppcheck")
set(CONF_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set(CONF_CMAKE_DIR "${CMAKE_INSTALL_PREFIX}/share/LIBCPPCHECK/cmake")
if(APPLE)
  set(CONF_STATIC_LINK_FLAGS
    "set(LIBCPPCHECK_STATIC_LINK_FLAGS \"-Wl,-force_load,${CMAKE_INSTALL_PREFIX}/lib/libchecks.a\")")
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux OR CYGWIN OR MINGW)
  set(CONF_STATIC_LINK_FLAGS
    "set(LIBCPPCHECK_STATIC_LINK_FLAGS \"-Wl,--whole-archive,${CMAKE_INSTALL_PREFIX}/lib/libchecks.a,--no-whole-archive\")")
elseif(MSVC)
  set(CONF_STATIC_LINK_FLAGS "include(\"${CONF_CMAKE_DIR}/LIBCPPCHECKStaticLinkFlags.cmake\")")
else()
  message(SEND_ERROR "FIXME: Don't know how to do static linking")
endif()
configure_file(LIBCPPCHECKConfig.cmake.in
  "${LIBCPPCHECK_BINARY_DIR}/LIBCPPCHECKConfig.cmake" @ONLY)

install(TARGETS checks static_checks
  EXPORT LIBCPPCHECKExports
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(DIRECTORY "${LIBCPPCHECK_SOURCE_DIR}/"
  DESTINATION include/cppcheck
  FILES_MATCHING PATTERN *.h)

install(EXPORT LIBCPPCHECKExports
  DESTINATION share/LIBCPPCHECK/cmake)

install(FILES "${LIBCPPCHECK_BINARY_DIR}/LIBCPPCHECKConfig.cmake"
  DESTINATION share/LIBCPPCHECK/cmake)
