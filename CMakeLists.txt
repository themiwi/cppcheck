cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

project(CPPCHECK)

option(CPPCHECK_CLI_USE_STATIC_LIBRARY
  "Link CLI against static cppcheck library." OFF)
option(CPPCHECK_GUI_USE_STATIC_LIBRARY
  "Link GUI against static cppcheck library." OFF)
mark_as_advanced(CPPCHECK_CLI_USE_STATIC_LIBRARY
  CPPCHECK_GUI_USE_STATIC_LIBRARY)

# must do a super-build on MSVC in order to be able to determine the /INCLUDE
# options required for static linking
if(NOT MSVC)
  option(CPPCHECK_USE_SUPERBUILD
    "Do a super-build of CPPCHECK." OFF)
  mark_as_advanced(CPPCHECK_USE_SUPERBUILD)
else()
  set(CPPCHECK_USE_SUPERBUILD TRUE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CPPCHECK_SOURCE_DIR}/cmake/modules")

find_package(PCRE)
if(NOT PCRE_FOUND)
  message("PCRE not found, disabling user-defined rules")
endif()

set(CPPCHECK_QT_COMPONENTS QtMain QtCore QtGui QtHelp QtMain QtXml)
find_package(Qt4 COMPONENTS ${CPPCHECK_QT_COMPONENTS})
if(NOT QT4_FOUND)
  message("GUI not built since QT4 not found.")
endif()

if(NOT CPPCHECK_USE_SUPERBUILD)
  # do a normal build, much simpler...

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CPPCHECK_BINARY_DIR}/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CPPCHECK_BINARY_DIR}/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CPPCHECK_BINARY_DIR}/lib")
  if(DEFINED CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CPPCHECK_BINARY_DIR}/Debug/bin")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CPPCHECK_BINARY_DIR}/Release/bin")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CPPCHECK_BINARY_DIR}/RelWithDebInfo/bin")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CPPCHECK_BINARY_DIR}/MinSizeRel/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CPPCHECK_BINARY_DIR}/Debug/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CPPCHECK_BINARY_DIR}/Release/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CPPCHECK_BINARY_DIR}/RelWithDebInfo/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${CPPCHECK_BINARY_DIR}/MinSizeRel/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CPPCHECK_BINARY_DIR}/Debug/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CPPCHECK_BINARY_DIR}/Release/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CPPCHECK_BINARY_DIR}/RelWithDebInfo/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${CPPCHECK_BINARY_DIR}/MinSizeRel/lib")
  endif()
  add_subdirectory(lib)
  add_subdirectory(cli)
  if(QT4_FOUND)
    add_subdirectory(gui)
  endif()
  add_subdirectory(test)

else(NOT CPPCHECK_USE_SUPERBUILD)
  # do super-build!

  # helper function
  include(CPPCHECKUtilities)

  # set up arguments for external projects
  set(EP_LIB_VARS
    CMAKE_BUILD_TYPE
    CMAKE_CXX_COMPILER CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO
    CMAKE_CXX_FLAGS_MINSIZEREL
    CMAKE_EXE_LINKER_FLAGS CMAKE_EXE_LINKER_FLAGS_DEBUG
    CMAKE_EXE_LINKER_FLAGS_RELEASE CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
    CMAKE_EXE_LINKER_FLAGS_MINSIZEREL
    CMAKE_SHARED_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS_DEBUG
    CMAKE_SHARED_LINKER_FLAGS_RELEASE CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
    CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL
    PCRE_INCLUDE_DIR PCRE_PCRE_LIBRARY PCRE_PCREPOSIX_LIBRARY
    CPPCHECK_USE_SUPERBUILD
    )
  if(MSVC)
    find_package(Dumpbin REQUIRED)
    list(APPEND EP_LIB_VARS DUMPBIN_EXECUTABLE)
  endif()

  set(EP_CLI_VARS ${EP_LIB_VARS} CPPCHECK_CLI_USE_STATIC_LIBRARY)

  set(EP_TEST_VARS ${EP_LIB_VARS} CPPCHECK_TEST_USE_STATIC_LIBRARY)

  cppcheck_add_subdirectory(lib VARIABLES ${EP_LIB_VARS})
  cppcheck_add_subdirectory(cli DEPENDS lib VARIABLES ${EP_CLI_VARS})

  if(QT4_FOUND)
    set(EP_GUI_VARS ${EP_LIB_VARS}
      CPPCHECK_GUI_USE_STATIC_LIBRARY
      CPPCHECK_QT_COMPONENTS)
    get_cmake_property(vars CACHE_VARIABLES)
    foreach(v IN LISTS vars)
      if(v MATCHES "^QT4?_")
        list(APPEND EP_GUI_VARS ${v})
      endif()
    endforeach()
    cppcheck_add_subdirectory(gui DEPENDS lib VARIABLES ${EP_GUI_VARS})
  endif()

  cppcheck_add_subdirectory(cppcheck-test SOURCE_DIR test DEPENDS lib
    VARIABLES ${EP_TEST_VARS} NO_INSTALL)
  ExternalProject_Get_Property(cppcheck-test binary_dir)
  if(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    set(checkcmd "$(MAKE)" -C "${binary_dir}" check)
  else()
    set(checkcmd "${CMAKE_COMMAND}" --build "${binary_dir}"
      --config ${CMAKE_CFG_INTDIR} --target check)
  endif()
  add_custom_target(check COMMAND ${checkcmd})
  add_dependencies(check cppcheck-test)

  # installation
  set(SUPER_DIR "${CPPCHECK_BINARY_DIR}/superbuild/inst/${CMAKE_CFG_INTDIR}")
  install(DIRECTORY
    "${SUPER_DIR}/bin"
    DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Executables
    USE_SOURCE_PERMISSIONS)
  install(DIRECTORY
    "${SUPER_DIR}/share/cppcheck/nls"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/cppcheck COMPONENT Translations
    USE_SOURCE_PERMISSIONS)
  install(DIRECTORY "${SUPER_DIR}/lib"
    DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Executables
    USE_SOURCE_PERMISSIONS FILES_MATCHING PATTERN *.so* PATTERN *.dylib)
  install(DIRECTORY "${SUPER_DIR}/lib"
    DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Development
    FILES_MATCHING PATTERN *.lib PATTERN *.a)
  install(DIRECTORY "${SUPER_DIR}/include"
    DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Development)
  install(DIRECTORY "${SUPER_DIR}/share/cppcheck/cmake"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/cppcheck COMPONENT Development)

endif(NOT CPPCHECK_USE_SUPERBUILD)
