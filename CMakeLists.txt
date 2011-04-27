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

set(CPPCHECK_QT_COMPONENTS QtMain QtCore QtGui QtMain)
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

# create documentation (a bit tedious, but I don't see how functions would
# simplify this significantly)
find_program(XSLTPROC_EXECUTABLE xsltproc)
if(XSLTPROC_EXECUTABLE)
  # man-page
  ##########
  find_file(DOCBOOK2MAN_STYLESHEET manpages/docbook.xsl
    PATHS /usr/share/xml/docbook/stylesheet/docbook-xsl
    DOC "XSL style sheet to convert DocBook to groff")
  if(DOCBOOK2MAN_STYLESHEET)
    add_custom_command(OUTPUT "${CPPCHECK_BINARY_DIR}/cppcheck.1"
      COMMAND "${XSLTPROC_EXECUTABLE}" "${DOCBOOK2MAN_STYLESHEET}"
        "${CPPCHECK_SOURCE_DIR}/man/cppcheck.1.xml"
      WORKING_DIRECTORY "${CPPCHECK_BINARY_DIR}"
      DEPENDS "${XSLTPROC_EXECUTABLE}" "${DOCBOOK2MAN_STYLESHEET}"
        "${CPPCHECK_SOURCE_DIR}/man/cppcheck.1.xml"
      COMMENT "Generating cppcheck.1")
    add_custom_target(man ALL DEPENDS "${CPPCHECK_BINARY_DIR}/cppcheck.1")
    install(FILES "${CPPCHECK_BINARY_DIR}/cppcheck.1"
      DESTINATION share/man/man1 COMPONENT Documentation)
  else()
    message("Not generating man-page, missing DOCBOOK2MAN_STYLESHEET")
  endif()

  # manuals
  #########
  add_custom_target(manuals ALL)
  find_file(DOCBOOK2XHTML_STYLESHEET xhtml/docbook.xsl
    PATHS /usr/share/xml/docbook/stylesheet/docbook-xsl
    DOC "XSL style sheet to convert DocBook to XHTML")

  # XHTML of manual
  if(DOCBOOK2XHTML_STYLESHEET)
    add_custom_command(OUTPUT "${CPPCHECK_BINARY_DIR}/manual.html"
      COMMAND "${XSLTPROC_EXECUTABLE}" -o "${CPPCHECK_BINARY_DIR}/manual.html"
        "${DOCBOOK2XHTML_STYLESHEET}"
        "${CPPCHECK_SOURCE_DIR}/man/manual.docbook"
      WORKING_DIRECTORY "${CPPCHECK_BINARY_DIR}"
      DEPENDS "${XSLTPROC_EXECUTABLE}" "${DOCBOOK2XHTML_STYLESHEET}"
        "${CPPCHECK_SOURCE_DIR}/man/manual.docbook"
      COMMENT "Generating manual.html")
    add_custom_target(manual-xhtml ALL
      DEPENDS "${CPPCHECK_BINARY_DIR}/manual.html")
    add_dependencies(manuals manual-xhtml)
    install(FILES "${CPPCHECK_BINARY_DIR}/manual.html"
      DESTINATION share/doc/cppcheck COMPONENT Documentation)
  else()
    message("Not generating XHTML manuals, missing DOCBOOK2XHTML_STYLESHEET")
  endif()

  # PDFs of manual, writing-rules-{1..3} and cppcheck-design
  find_program(FOP_EXECUTABLE fop)
  if(FOP_EXECUTABLE)
    find_file(DOCBOOK2FO_STYLESHEET fo/docbook.xsl
      PATHS /usr/share/xml/docbook/stylesheet/docbook-xsl
      DOC "XSL style sheet to convert DocBook to FO")
    if(DOCBOOK2FO_STYLESHEET)
      foreach(doc manual writing-rules-1 writing-rules-2 writing-rules-3
          cppcheck-design)
        if(doc STREQUAL manual)
          set(params)
        else()
          set(params --stringparam generate.toc "article nop")
        endif()
        add_custom_command(OUTPUT "${CPPCHECK_BINARY_DIR}/${doc}.pdf"
          COMMAND "${XSLTPROC_EXECUTABLE}"
            ${params} -o "${CPPCHECK_BINARY_DIR}/${doc}.fo"
            "${DOCBOOK2FO_STYLESHEET}"
            "${CPPCHECK_SOURCE_DIR}/man/${doc}.docbook"
          COMMAND "${FOP_EXECUTABLE}" -pdf "${CPPCHECK_BINARY_DIR}/${doc}.pdf"
            -fo "${CPPCHECK_BINARY_DIR}/${doc}.fo"
          COMMAND "${CMAKE_COMMAND}" -E remove -f
            "${CPPCHECK_BINARY_DIR}/${doc}.fo"
          WORKING_DIRECTORY "${CPPCHECK_BINARY_DIR}"
          DEPENDS "${XSLTPROC_EXECUTABLE}" "${FOP_EXECUTABLE}"
            "${DOCBOOK2FO_STYLESHEET}"
            "${CPPCHECK_SOURCE_DIR}/man/${doc}.docbook"
          COMMENT "Generating ${doc}.pdf")
        add_custom_target(${doc}-pdf ALL
          DEPENDS "${CPPCHECK_BINARY_DIR}/${doc}.pdf")
        add_dependencies(manuals ${doc}-pdf)
        install(FILES
          "${CPPCHECK_BINARY_DIR}/${doc}.pdf"
          DESTINATION share/doc/cppcheck COMPONENT Documentation)
      endforeach()
    else()
      message("Not generating PDF manuals, missing DOCBOOK2FO_STYLESHEET")
    endif()
  else()
    message("Not generating PDF manuals, missing FOP_EXECUTABLE")
  endif()
else()
  message("Not generating documentation, missing XSLTPROC_EXECUTABLE")
endif()
