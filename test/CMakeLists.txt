# Instructions for building of the cppcheck test suite

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(CPPCHECKTESTS)

# for non-superbuild stand-alone use
option(CPPCHECK_TEST_USE_STATIC_LIBRARY
  "Link cppcheck-test against static cppcheck library." OFF)

set(CPPCHECK_USE_STATIC ${CPPCHECK_TEST_USE_STATIC_LIBRARY})
find_package(CPPCHECK COMPONENTS LIB REQUIRED NO_MODULE)

set(CPPCHECKTEST_LIBS ${CPPCHECK_LIBRARIES})

SET(CPPCHECKTEST_SRCS
  ../cli/cmdlineparser.cpp
  ../cli/cppcheckexecutor.cpp
  ../cli/filelister.cpp
  ../cli/threadexecutor.cpp
  ../cli/pathmatch.cpp
  options.cpp
  testautovariables.cpp
  testbufferoverrun.cpp
  testcharvar.cpp
  testclass.cpp
  testcmdlineparser.cpp
  testconstructors.cpp
  testcppcheck.cpp
  testdivision.cpp
  testerrorlogger.cpp
  testexceptionsafety.cpp
  testfilelister.cpp
  testincompletestatement.cpp
  testmathlib.cpp
  testmemleak.cpp
  testnullpointer.cpp
  testobsoletefunctions.cpp
  testoptions.cpp
  testother.cpp
  testpath.cpp
  testpathmatch.cpp
  testpostfixoperator.cpp
  testpreprocessor.cpp
  testrunner.cpp
  testsettings.cpp
  testsimplifytokens.cpp
  teststl.cpp
  testsuite.cpp
  testsymboldatabase.cpp
  testthreadexecutor.cpp
  testtoken.cpp
  testtokenize.cpp
  testuninitvar.cpp
  testunusedfunctions.cpp
  testunusedprivfunc.cpp
  testunusedvar.cpp
  ../externals/tinyxml/tinystr.cpp
  ../externals/tinyxml/tinyxml.cpp
  ../externals/tinyxml/tinyxmlerror.cpp
  ../externals/tinyxml/tinyxmlparser.cpp)

if(WIN32)
   if(NOT CYGWIN)
      # Windows needs additional shlwapi library.
      list(APPEND CPPCHECKTEST_LIBS shlwapi)
   endif()
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wshadow -Wno-long-long -Wfloat-equal -Wcast-qual")
endif (CMAKE_COMPILER_IS_GNUCXX)

if(NOT CPPCHECK_TEST_USE_STATIC_LIBRARY)
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

include_directories(${CPPCHECK_INCLUDE_DIRS} ../cli ../externals/tinyxml)
add_executable(cppcheck-test EXCLUDE_FROM_ALL ${CPPCHECKTEST_SRCS})
target_link_libraries(cppcheck-test ${CPPCHECKTEST_LIBS})

# Add custom 'make check' -target
# It compiles and runs tests
add_custom_target(check COMMAND cppcheck-test)
add_dependencies(check cppcheck-test)
