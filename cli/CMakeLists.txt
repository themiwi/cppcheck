# build the command line executable
###################################

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(CPPCHECKCLI)

if(CPPCHECK_USE_SUPERBUILD)
  list(APPEND CMAKE_MODULE_PATH "${CPPCHECKCLI_SOURCE_DIR}/../cmake/modules")
  set(CPPCHECK_USE_STATIC ${CPPCHECK_CLI_USE_STATIC_LIBRARY})
  find_package(CPPCHECK COMPONENTS LIB REQUIRED NO_MODULE)
  find_package(PCRE)
else()
  set(CPPCHECK_USE_STATIC ${CPPCHECK_CLI_USE_STATIC_LIBRARY})
  set(CPPCHECK_FIND_COMPONENTS LIB)
  include("${CPPCHECKCLI_BINARY_DIR}/../lib/CPPCHECKConfig.cmake")
endif()

set(CPPCHECKCLI_LIBS ${CPPCHECK_LIBRARIES})

if(PCRE_FOUND)
  add_definitions(-DHAVE_RULES)
endif()

set(CPPCHECKCLI_SRCS
  cmdlineparser.cpp
  cppcheckexecutor.cpp
  filelister.cpp
  main.cpp
  pathmatch.cpp
  threadexecutor.cpp)

if(PCRE_FOUND)
  list(APPEND CPPCHECKCLI_SRCS
    ../externals/tinyxml/tinystr.cpp
    ../externals/tinyxml/tinyxml.cpp
    ../externals/tinyxml/tinyxmlerror.cpp
    ../externals/tinyxml/tinyxmlparser.cpp)
endif()

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
  RUNTIME DESTINATION bin COMPONENT Executables
  LIBRARY DESTINATION lib COMPONENT Executables
  ARCHIVE DESTINATION lib COMPONENT Executables)

install(EXPORT cppcheckCLIExports
  DESTINATION share/cppcheck/cmake COMPONENT Development)

if(WIN32)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION bin COMPONENT Executables)
else()
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION lib COMPONENT Executables)
endif()
include(InstallRequiredSystemLibraries)
