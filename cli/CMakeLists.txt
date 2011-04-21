# Minimal CMake build file to build cppcheck command line executable

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(CPPCHECKCLI)

# for non-superbuild stand-alone use
option(CPPCHECK_CLI_USE_STATIC_LIBRARY
  "Link CLI against static cppcheck library." OFF)

set(LIBCPPCHECK_USE_STATIC ${CPPCHECK_CLI_USE_STATIC_LIBRARY})
find_package(LIBCPPCHECK REQUIRED NO_MODULE)

set(CPPCHECKCLI_LIBS ${LIBCPPCHECK_LIBRARIES})

list(APPEND CMAKE_MODULE_PATH "${CPPCHECKCLI_SOURCE_DIR}/../cmake/modules")
find_package(PCRE)
if(PCRE_FOUND)
   add_definitions(-DHAVE_RULES)
   include_directories("${PCRE_INCLUDE_DIR}")
   list(APPEND CPPCHECKCLI_LIBS ${PCRE_LIBRARIES})
endif()

set(TINYXML_INCLUDE_DIR "${CPPCHECKCLI_SOURCE_DIR}/../externals/tinyxml/")

SET(CPPCHECKCLI_SRCS
  cmdlineparser.cpp
  cppcheckexecutor.cpp
  filelister.cpp
  main.cpp
  pathmatch.cpp
  threadexecutor.cpp)

if(PCRE_FOUND)
   list(APPEND CPPCHECKCLI_SRCS
     "${TINYXML_INCLUDE_DIR}tinystr.cpp"
     "${TINYXML_INCLUDE_DIR}tinyxml.cpp"
     "${TINYXML_INCLUDE_DIR}tinyxmlerror.cpp"
     "${TINYXML_INCLUDE_DIR}tinyxmlparser.cpp")
endif()

if(WIN32)
   # Add Windows resource file
   set(CPPCHECKCLI_SRCS ${CPPCHECKCLI_SRCS} cppcheck.rc)

   if(NOT CYGWIN)
      # Windows needs additional shlwapi library.
      list(APPEND CPPCHECKCLI_LIBS shlwapi)
   endif()
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wshadow -Wno-long-long -Wfloat-equal -Wcast-qual")
endif (CMAKE_COMPILER_IS_GNUCXX)

if(NOT CPPCHECK_CLI_USE_STATIC_LIBRARY)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()

include_directories(${LIBCPPCHECK_INCLUDE_DIRS} "${TINYXML_INCLUDE_DIR}")
add_executable(cppcheck ${CPPCHECKCLI_SRCS})
target_link_libraries(cppcheck ${CPPCHECKCLI_LIBS})

if(CPPCHECK_CLI_USE_STATIC_LIBRARY)
  set_target_properties(cppcheck PROPERTIES
    LINK_FLAGS "${LIBCPPCHECK_STATIC_LINK_FLAGS}")
endif()

install(TARGETS cppcheck
  EXPORT CPPCHECKCLIExports
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(EXPORT CPPCHECKCLIExports
  DESTINATION share/CPPCHECKCLI/cmake)
