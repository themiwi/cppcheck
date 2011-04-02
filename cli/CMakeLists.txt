# Minimal CMake build file to build cppcheck command line executable

if(PCRE_FOUND)
   add_definitions(-DHAVE_RULES)
   include_directories("${PCRE_INCLUDE_DIR}")
   set(CHECK_LIBS ${PCRE_LIBRARIES})
endif()

set(TINYXML_INCLUDE_DIR "${CPPCHECK_SOURCE_DIR}/externals/tinyxml/")

SET(CHECKCLI_SRCS
  cmdlineparser.cpp
  cppcheckexecutor.cpp
  filelister.cpp
  main.cpp
  threadexecutor.cpp
  pathmatch.cpp
  "${TINYXML_INCLUDE_DIR}tinystr.cpp"
  "${TINYXML_INCLUDE_DIR}tinyxml.cpp"
  "${TINYXML_INCLUDE_DIR}tinyxmlerror.cpp"
  "${TINYXML_INCLUDE_DIR}tinyxmlparser.cpp")

option(CPPCHECK_CLI_BUILD_EMBED_LIBRARY_SOURCE_CODE
       "The source files of Cppchecks's library should be directly included
for the compilation of the command line interface.")
mark_as_advanced(CPPCHECK_CLI_BUILD_EMBED_LIBRARY_SOURCE_CODE)

if(CPPCHECK_CLI_BUILD_EMBED_LIBRARY_SOURCE_CODE)
   set(CPPCHECK_LIB_DIR "${CPPCHECK_SOURCE_DIR}/lib/")
   include("${CPPCHECK_LIB_DIR}library_sources.cmake")
   set(CHECKCLI_SRCS ${CHECKCLI_SRCS} ${CPPCHECK_LIB_SOURCES})
else()
   option(CPPCHECK_CLI_BUILD_USE_STATICALLY_LINKED_LIBRARY
          "The static library file that was generated for Cppchecks's class library should be
used for the link step of the command line interface.")
   mark_as_advanced(CPPCHECK_CLI_BUILD_USE_STATICALLY_LINKED_LIBRARY)

   if(CPPCHECK_CLI_BUILD_USE_STATICALLY_LINKED_LIBRARY)
       set(CHECK_LIBS libchecks ${CHECK_LIBS})   # static approach
   else()
      set(CHECK_LIBS checks ${CHECK_LIBS})   # shared approach
   endif()
endif()

if(WIN32)
   # Add Windows resource file
   set(CHECKCLI_SRCS ${CHECKCLI_SRCS} cppcheck.rc)

   if(NOT CYGWIN)
      # Windows needs additional shlwapi library.
      set(CHECK_LIBS ${CHECK_LIBS} shlwapi)
   endif()
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wshadow -Wno-long-long -Wfloat-equal -Wcast-qual")
endif (CMAKE_COMPILER_IS_GNUCXX)

include_directories("${CPPCHECK_SOURCE_DIR}/lib"
                    "${TINYXML_INCLUDE_DIR}")
add_executable(cppcheck ${CHECKCLI_SRCS})
TARGET_LINK_LIBRARIES(cppcheck ${CHECK_LIBS})
