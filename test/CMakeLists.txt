# build the tests
#################

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(CPPCHECKTESTS)

# The tests must be linked statically against libcppcheck because they access
# otherwise hidden classes.
set(CPPCHECK_USE_STATIC TRUE)
if(CPPCHECK_USE_SUPERBUILD)
  list(APPEND CMAKE_MODULE_PATH "${CPPCHECKTESTS_SOURCE_DIR}/../cmake/modules")
  find_package(CPPCHECK COMPONENTS LIB REQUIRED NO_MODULE)
else()
  set(CPPCHECK_FIND_COMPONENTS LIB)
  include("${CPPCHECKTESTS_BINARY_DIR}/../lib/CPPCHECKConfig.cmake")
endif()

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
  testsuppressions.cpp
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

include_directories(${CPPCHECK_INCLUDE_DIRS} ../cli ../externals/tinyxml)
add_executable(cppcheck-test EXCLUDE_FROM_ALL ${CPPCHECKTEST_SRCS})
target_link_libraries(cppcheck-test ${CPPCHECKTEST_LIBS})

if (CMAKE_COMPILER_IS_GNUCXX)
  set_target_properties(cppcheck-test PROPERTIES COMPILE_FLAGS
    "-Wall -Wextra -pedantic -Wshadow -Wno-long-long -Wfloat-equal -Wcast-qual")
endif (CMAKE_COMPILER_IS_GNUCXX)

add_definitions(${CPPCHECK_STATIC_COMPILE_DEFINITIONS})
set_target_properties(cppcheck-test PROPERTIES
  LINK_FLAGS "${CPPCHECK_STATIC_LINK_FLAGS}")

# Add custom 'make check' target
# It compiles and runs tests
add_custom_target(check COMMAND cppcheck-test
  WORKING_DIRECTORY "${CPPECHECKTEST_SOURCE_DIR}/..")
add_dependencies(check cppcheck-test)
