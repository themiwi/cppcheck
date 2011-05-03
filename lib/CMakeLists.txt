# build the library
###################

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

if(PCRE_FOUND)
  target_link_libraries(libcppcheck ${PCRE_LIBRARIES})
  target_link_libraries(libcppcheck_static ${PCRE_LIBRARIES})
endif()

configure_file(CPPCHECKConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/CPPCHECKConfig.cmake" COPYONLY)

install(TARGETS libcppcheck libcppcheck_static
  EXPORT libcppcheckExports
  RUNTIME DESTINATION bin COMPONENT Executables
  LIBRARY DESTINATION lib COMPONENT Executables
  ARCHIVE DESTINATION lib COMPONENT Development
  PUBLIC_HEADER DESTINATION include/cppcheck COMPONENT Development)

install(EXPORT libcppcheckExports
  DESTINATION share/cppcheck/cmake COMPONENT Development)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/CPPCHECKConfig.cmake"
  DESTINATION share/cppcheck/cmake COMPONENT Development)
