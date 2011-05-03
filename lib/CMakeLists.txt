# build the library
###################

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(LIBCPPCHECK)

list(APPEND CMAKE_MODULE_PATH "${LIBCPPCHECK_SOURCE_DIR}/../cmake/modules")

# for non-superbuild standalone use
if(CPPCHECK_USE_SUPERBUILD)
  find_package(PCRE)

  if(MSVC)
    find_package(Dumpbin REQUIRED)
  endif()
else()
  # this is a hack for non-superbuild mode
  file(WRITE "${LIBCPPCHECK_BINARY_DIR}/libcppcheckExports.cmake" "
# Generated -- Do not edit!
set(CPPCHECK_INCLUDE_DIR \"${LIBCPPCHECK_SOURCE_DIR}\")
set(CPPCHECK_INCLUDE_DIRS \"\${CPPCHECK_INCLUDE_DIR}\")
")
endif()

if(PCRE_FOUND)
  add_definitions(-DHAVE_RULES)
  include_directories("${PCRE_INCLUDE_DIR}")
endif()

set(LIBCPPCHECK_SOURCES
  checkautovariables.cpp
  checkbufferoverrun.cpp
  check.cpp
  checkclass.cpp
  checkexceptionsafety.cpp
  checkmemoryleak.cpp
  checknullpointer.cpp
  checkobsoletefunctions.cpp
  checkother.cpp
  checkpostfixoperator.cpp
  checkstl.cpp
  checkuninitvar.cpp
  checkunusedfunctions.cpp
  cppcheck.cpp
  errorlogger.cpp
  executionpath.cpp
  mathlib.cpp
  path.cpp
  preprocessor.cpp
  settings.cpp
  symboldatabase.cpp
  registerchecks.cpp
  timer.cpp
  token.cpp
  tokenize.cpp)

set(LIBCPPCHECK_HEADERS
  checkautovariables.h
  checkbufferoverrun.h
  checkclass.h
  checkexceptionsafety.h
  check.h
  checkmemoryleak.h
  checknullpointer.h
  checkobsoletefunctions.h
  checkother.h
  checkpostfixoperator.h
  checkstl.h
  checkuninitvar.h
  checkunusedfunctions.h
  cppcheck.h
  DLLVisibility.h
  errorlogger.h
  executionpath.h
  mathlib.h
  path.h
  preprocessor.h
  settings.h
  symboldatabase.h
  registerchecks.h
  timer.h
  token.h
  tokenize.h)

set(CMAKE_INSTALL_NAME_DIR "@executable_path/../lib")

add_library(libcppcheck SHARED
  ${LIBCPPCHECK_SOURCES} ${LIBCPPCHECK_HEADERS})
add_library(libcppcheck_static STATIC
  ${LIBCPPCHECK_SOURCES} ${LIBCPPCHECK_HEADERS})
set_target_properties(libcppcheck PROPERTIES
  OUTPUT_NAME "cppcheck"
  DEFINE_SYMBOL CPPCHECK_EXPORTS
  PUBLIC_HEADER "${LIBCPPCHECK_HEADERS}")
set_target_properties(libcppcheck_static PROPERTIES
  COMPILE_DEFINITIONS CPPCHECK_STATIC
  OUTPUT_NAME "cppcheck"
  PREFIX "lib")

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CPPCHECK_FLAGS
    "-Wall -Wextra -pedantic -Wshadow -Wno-long-long -Wfloat-equal -Wcast-qual")
  set_target_properties(libcppcheck_static PROPERTIES
    COMPILE_FLAGS "${CPPCHECK_FLAGS}")

  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-fvisibility=hidden HAVE_FVISIBILITY_HIDDEN)
  if(HAVE_FVISIBILITY_HIDDEN)
    set(CPPCHECK_FLAGS "${CPPCHECK_FLAGS} -fvisibility=hidden")
  endif()
  set_target_properties(libcppcheck PROPERTIES
    COMPILE_FLAGS "${CPPCHECK_FLAGS}")
endif()

if(NOT ${CMAKE_VERSION} VERSION_GREATER 2.8)
   set_target_properties(libcppcheck libcppcheck_static PROPERTIES
     CLEAN_DIRECT_OUTPUT 1)
endif()

if(MSVC)
  # generate /INCLUDE flags for LINK.EXE
  set(outfile "${LIBCPPCHECK_BINARY_DIR}/libcppcheckStaticLinkFlags.cmake")
  add_custom_command(OUTPUT "${outfile}"
    COMMAND "${CMAKE_COMMAND}" "-DDUMPBIN=${DUMPBIN_EXECUTABLE}"
    "-DOUTPUT=${outfile}" "-DLIBRARY=$<TARGET_FILE:libcppcheck_static>"
    -P "${LIBCPPCHECK_SOURCE_DIR}/generateStaticLinkFlags.cmake"
    DEPENDS "${LIBCPPCHECK_SOURCE_DIR}/generateStaticLinkFlags.cmake"
      libcppcheck_static
    COMMENT "Generating libcppcheckStaticLinkFlags.cmake"
    VERBATIM)
  add_custom_target(staticLinkFlags ALL DEPENDS "${outfile}")
  add_dependencies(staticLinkFlags libcppcheck_static)
  install(FILES "${outfile}" DESTINATION share/cppcheck/cmake)
endif()

if(PCRE_FOUND)
  target_link_libraries(libcppcheck ${PCRE_LIBRARIES})
  target_link_libraries(libcppcheck_static ${PCRE_LIBRARIES})
endif()

set(_PREFIX "{_PREFIX}") # trick to get ${_PREFIX} into the configured file
set(_DIR "{_DIR}")       # ditto
if(APPLE)
  set(CONF_STATIC_LINK_FLAGS
"set(LIBCPPCHECK_STATIC_LINK_FLAGS
  \"-Wl,-force_load,$${_PREFIX}/lib/libcppcheck.a\")")
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux OR CYGWIN OR MINGW)
  set(CONF_STATIC_LINK_FLAGS
"set(LIBCPPCHECK_STATIC_LINK_FLAGS
  \"-Wl,--whole-archive,$${_PREFIX}/lib/libcppcheck.a,--no-whole-archive\")")
elseif(MSVC)
  set(CONF_STATIC_LINK_FLAGS
    "include(\"$${_DIR}/libcppcheckStaticLinkFlags.cmake\")")
else()
  message(SEND_ERROR "FIXME: Don't know how to do static linking")
endif()
configure_file(CPPCHECKConfig.cmake.in
  "${LIBCPPCHECK_BINARY_DIR}/CPPCHECKConfig.cmake" @ONLY)

install(TARGETS libcppcheck libcppcheck_static
  EXPORT libcppcheckExports
  RUNTIME DESTINATION bin COMPONENT Executables
  LIBRARY DESTINATION lib COMPONENT Executables
  ARCHIVE DESTINATION lib COMPONENT Development
  PUBLIC_HEADER DESTINATION include/cppcheck COMPONENT Development)

install(EXPORT libcppcheckExports
  DESTINATION share/cppcheck/cmake COMPONENT Development)

install(FILES "${LIBCPPCHECK_BINARY_DIR}/CPPCHECKConfig.cmake"
  DESTINATION share/cppcheck/cmake COMPONENT Development)
