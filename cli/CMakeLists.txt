# Minimal CMake build file to build cppcheck command line executable

include_directories("${CPPCHECK_SOURCE_DIR}/lib")

if(PCRE_FOUND)
   add_definitions(-DHAVE_RULES)
   set(TINYXML_INCLUDE_DIR "${CPPCHECK_SOURCE_DIR}/externals/tinyxml/")
   include_directories("${PCRE_INCLUDE_DIR}" "${TINYXML_INCLUDE_DIR}")
   set(CHECK_LIBS ${PCRE_LIBRARIES})
endif()

SET(CHECKCLI_SRCS
  cmdlineparser.cpp
  cppcheckexecutor.cpp
  filelister.cpp
  main.cpp
  pathmatch.cpp
  threadexecutor.cpp)

if(PCRE_FOUND)
   set(CHECKCLI_SRCS ${CHECKCLI_SRCS}
       "${TINYXML_INCLUDE_DIR}tinystr.cpp"
       "${TINYXML_INCLUDE_DIR}tinyxml.cpp"
       "${TINYXML_INCLUDE_DIR}tinyxmlerror.cpp"
       "${TINYXML_INCLUDE_DIR}tinyxmlparser.cpp")
endif()

set(CPPCHECK_LIB_DIR "${CPPCHECK_SOURCE_DIR}/lib/")
include("${CPPCHECK_LIB_DIR}library_sources.cmake")

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

add_executable(cppcheck ${CHECKCLI_SRCS} ${CPPCHECK_LIB_SOURCES})
TARGET_LINK_LIBRARIES(cppcheck ${CHECK_LIBS})
