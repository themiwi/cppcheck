cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

project(CPPCHECK)

# user options
##############

option(CPPCHECK_CLI_USE_STATIC_LIBRARY
  "Link CLI against static cppcheck library." OFF)
option(CPPCHECK_GUI_USE_STATIC_LIBRARY
  "Link GUI against static cppcheck library." OFF)
mark_as_advanced(CPPCHECK_CLI_USE_STATIC_LIBRARY
  CPPCHECK_GUI_USE_STATIC_LIBRARY)

# find dependencies
###################

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

find_package(PCRE)
if(NOT PCRE_FOUND)
  message("PCRE not found, disabling user-defined rules")
endif()

set(CPPCHECK_QT_COMPONENTS QtMain QtCore QtGui QtMain)
find_package(Qt4 COMPONENTS ${CPPCHECK_QT_COMPONENTS})
if(NOT QT4_FOUND)
  message("GUI not built since QT4 not found.")
endif()

# build the binaries
####################

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
if(DEFINED CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG
    "${CMAKE_CURRENT_BINARY_DIR}/Debug/bin")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE
    "${CMAKE_CURRENT_BINARY_DIR}/Release/bin")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO
    "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/bin")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL
    "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG
    "${CMAKE_CURRENT_BINARY_DIR}/Debug/lib")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE
    "${CMAKE_CURRENT_BINARY_DIR}/Release/lib")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO
    "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/lib")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL
    "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG
    "${CMAKE_CURRENT_BINARY_DIR}/Debug/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE
    "${CMAKE_CURRENT_BINARY_DIR}/Release/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO
    "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL
    "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/lib")
endif()
add_subdirectory(lib)
add_subdirectory(cli)
if(QT4_FOUND)
  add_subdirectory(gui)
endif()
add_subdirectory(test)

# build the documentation
#########################

# a bit tedious, but I don't see how functions would simplify this
# significantly
find_program(XSLTPROC_EXECUTABLE xsltproc)
if(XSLTPROC_EXECUTABLE)
  # man-page
  ##########
  find_file(DOCBOOK2MAN_STYLESHEET manpages/docbook.xsl
    PATHS /usr/share/xml/docbook/stylesheet/docbook-xsl
    DOC "XSL style sheet to convert DocBook to groff")
  if(DOCBOOK2MAN_STYLESHEET)
    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/cppcheck.1"
      COMMAND "${XSLTPROC_EXECUTABLE}" "${DOCBOOK2MAN_STYLESHEET}"
        "${CMAKE_CURRENT_SOURCE_DIR}/man/cppcheck.1.xml"
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
      DEPENDS "${XSLTPROC_EXECUTABLE}" "${DOCBOOK2MAN_STYLESHEET}"
        "${CMAKE_CURRENT_SOURCE_DIR}/man/cppcheck.1.xml"
      COMMENT "Generating cppcheck.1")
    add_custom_target(man ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/cppcheck.1")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cppcheck.1"
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
    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/manual.html"
      COMMAND "${XSLTPROC_EXECUTABLE}" -o "${CMAKE_CURRENT_BINARY_DIR}/manual.html"
        "${DOCBOOK2XHTML_STYLESHEET}"
        "${CMAKE_CURRENT_SOURCE_DIR}/man/manual.docbook"
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
      DEPENDS "${XSLTPROC_EXECUTABLE}" "${DOCBOOK2XHTML_STYLESHEET}"
        "${CMAKE_CURRENT_SOURCE_DIR}/man/manual.docbook"
      COMMENT "Generating manual.html")
    add_custom_target(manual-xhtml ALL
      DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/manual.html")
    add_dependencies(manuals manual-xhtml)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/manual.html"
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
        add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${doc}.pdf"
          COMMAND "${XSLTPROC_EXECUTABLE}"
            ${params} -o "${CMAKE_CURRENT_BINARY_DIR}/${doc}.fo"
            "${DOCBOOK2FO_STYLESHEET}"
            "${CMAKE_CURRENT_SOURCE_DIR}/man/${doc}.docbook"
          COMMAND "${FOP_EXECUTABLE}" -pdf "${CMAKE_CURRENT_BINARY_DIR}/${doc}.pdf"
            -fo "${CMAKE_CURRENT_BINARY_DIR}/${doc}.fo"
          COMMAND "${CMAKE_COMMAND}" -E remove -f
            "${CMAKE_CURRENT_BINARY_DIR}/${doc}.fo"
          WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
          DEPENDS "${XSLTPROC_EXECUTABLE}" "${FOP_EXECUTABLE}"
            "${DOCBOOK2FO_STYLESHEET}"
            "${CMAKE_CURRENT_SOURCE_DIR}/man/${doc}.docbook"
          COMMENT "Generating ${doc}.pdf")
        add_custom_target(${doc}-pdf ALL
          DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${doc}.pdf")
        add_dependencies(manuals ${doc}-pdf)
        install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/${doc}.pdf"
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
