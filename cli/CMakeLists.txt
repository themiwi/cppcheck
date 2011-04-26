# Minimal CMake build file to build cppcheck command line executable

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(CPPCHECKCLI)

# for non-superbuild stand-alone use
option(CPPCHECK_CLI_USE_STATIC_LIBRARY
  "Link CLI against static cppcheck library." OFF)

set(CPPCHECK_USE_STATIC ${CPPCHECK_CLI_USE_STATIC_LIBRARY})
find_package(CPPCHECK COMPONENTS LIB REQUIRED NO_MODULE)

set(CPPCHECKCLI_LIBS ${CPPCHECK_LIBRARIES})

list(APPEND CMAKE_MODULE_PATH "${CPPCHECKCLI_SOURCE_DIR}/../cmake/modules")
find_package(PCRE)
if(PCRE_FOUND)
   add_definitions(-DHAVE_RULES)
   include_directories("${PCRE_INCLUDE_DIR}")
   list(APPEND CPPCHECKCLI_LIBS ${PCRE_LIBRARIES})
endif()

set(CPPCHECKCLI_SRCS
  cmdlineparser.cpp
  cppcheckexecutor.cpp
  filelister.cpp
  main.cpp
  threadexecutor.cpp
  pathmatch.cpp
  ../externals/tinyxml/tinystr.cpp
  ../externals/tinyxml/tinyxml.cpp
  ../externals/tinyxml/tinyxmlerror.cpp
  ../externals/tinyxml/tinyxmlparser.cpp)

if(WIN32)
   # Add Windows resource file
   set(CPPCHECKCLI_SRCS ${CPPCHECKCLI_SRCS} cppcheck.rc)

   if(NOT CYGWIN)
      # Windows needs additional shlwapi library.
      list(APPEND CPPCHECKCLI_LIBS shlwapi)
   endif()
endif()

if(NOT CPPCHECK_CLI_USE_STATIC_LIBRARY)
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

include_directories(${CPPCHECK_INCLUDE_DIRS} ../externals/tinyxml)
add_executable(cppcheck ${CPPCHECKCLI_SRCS})
target_link_libraries(cppcheck ${CPPCHECKCLI_LIBS})

if (CMAKE_COMPILER_IS_GNUCXX)
  set_target_properties(cppcheck PROPERTIES
    COMPILE_FLAGS "-Wall -Wextra -pedantic -Wshadow -Wno-long-long -Wfloat-equal -Wcast-qual")
endif (CMAKE_COMPILER_IS_GNUCXX)

if(CPPCHECK_CLI_USE_STATIC_LIBRARY)
  add_definitions(${CPPCHECK_STATIC_COMPILE_DEFINITIONS})
  set_target_properties(cppcheck PROPERTIES
    LINK_FLAGS "${CPPCHECK_STATIC_LINK_FLAGS}")
endif()

install(TARGETS cppcheck
  EXPORT cppcheckCLIExports
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(EXPORT cppcheckCLIExports
  DESTINATION share/cppcheck/cmake)

include(InstallRequiredSystemLibraries)
