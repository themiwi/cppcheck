# Instructions for building of the cppcheck Qt GUI

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(CPPCHECKGUI)

# for non-superbuild stand-alone use
option(CPPCHECK_GUI_USE_STATIC_LIBRARY
  "Link GUI against static cppcheck library." OFF)

set(CPPCHECK_USE_STATIC ${CPPCHECK_GUI_USE_STATIC_LIBRARY})
find_package(CPPCHECK COMPONENTS LIB REQUIRED NO_MODULE)

# find and setup Qt4 for this project
find_package(Qt4 COMPONENTS QtMain QtCore QtGui QtHelp QtMain QtXml REQUIRED)
include(${QT_USE_FILE})

set(CPPCHECKGUI_LIBS ${CPPCHECK_LIBRARIES} ${QT_LIBRARIES})

include_directories(
  ${CPPCHECK_INCLUDE_DIRS}
  .
  ${CMAKE_CURRENT_BINARY_DIR})

# Header files - listed for mocking
set(CPPCHECKGUI_HEADERS
  aboutdialog.h
  application.h
  applicationdialog.h
  applicationlist.h
  checkstatistics.h
  checkthread.h
  common.h
  csvreport.h
  erroritem.h
  filelist.h
  fileviewdialog.h
  logview.h
  mainwindow.h
  project.h
  projectfile.h
  projectfiledialog.h
  report.h
  resultstree.h
  resultsview.h
  settingsdialog.h
  statsdialog.h
  threadhandler.h
  threadresult.h
  translationhandler.h
  txtreport.h
  xmlreport.h
  xmlreportv1.h
  xmlreportv2.h
  )

# Source files
set(CPPCHECKGUI_SRCS
  aboutdialog.cpp
  application.cpp
  applicationdialog.cpp
  applicationlist.cpp
  checkstatistics.cpp
  checkthread.cpp
  csvreport.cpp
  erroritem.cpp
  filelist.cpp
  fileviewdialog.cpp
  logview.cpp
  main.cpp
  mainwindow.cpp
  project.cpp
  projectfile.cpp
  projectfiledialog.cpp
  report.cpp
  resultstree.cpp
  resultsview.cpp
  settingsdialog.cpp
  statsdialog.cpp
  threadhandler.cpp
  threadresult.cpp
  translationhandler.cpp
  txtreport.cpp
  xmlreport.cpp
  xmlreportv1.cpp
  xmlreportv2.cpp
  )

# UI files
set(CPPCHECKGUI_UIS
  about.ui
  application.ui
  file.ui
  logview.ui
  main.ui
  projectfile.ui
  resultsview.ui
  settings.ui
  stats.ui
  )

# Translation files
set(CPPCHECKGUI_TRANS
  cppcheck_de.ts
  cppcheck_en.ts
  cppcheck_es.ts
  cppcheck_fi.ts
  cppcheck_fr.ts
  cppcheck_ja.ts
  cppcheck_nl.ts
  cppcheck_pl.ts
  cppcheck_ru.ts
  cppcheck_sr.ts
  cppcheck_sv.ts
  )

set(CPPCHECKGUI_RCCS gui.qrc)

if(WIN32)
   # Add Windows resource file
   list(APPEND CPPCHECKGUI_SRCS cppcheck-gui.rc)

   if(NOT CYGWIN)
      # Windows needs additional shlwapi library.
      list(APPEND CPPCHECKGUI_LIBS shlwapi)
   endif()
endif()

if(NOT CPPCHECK_GUI_USE_STATIC_LIBRARY)
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

# Generate rules for building source files from the resources
qt4_add_resources(CPPCHECKGUI_RCC_SRCS ${CPPCHECKGUI_RCCS})

# Process UI files
qt4_wrap_ui(CPPCHECKGUI_UIS_H ${CPPCHECKGUI_UIS})

# Mock header files
qt4_wrap_cpp(CPPCHECKGUI_MOC_SRCS ${CPPCHECKGUI_HEADERS})

# add translations ...
qt4_add_translation(CPPCHECKGUI_QM ${CPPCHECKGUI_TRANS})

# Create folders for Visual Studio IDE
source_group("Header Files" FILES ${CPPCHECKGUI_HEADERS})
source_group("Ui Files" ".ui$")
source_group("Moc Files" "moc_.*cxx$")

add_executable(cppcheck-gui WIN32 ${CPPCHECKGUI_SRCS} ${CPPCHECKGUI_MOC_SRCS}
  ${CPPCHECKGUI_HEADERS} ${CPPCHECKGUI_UIS_H} ${CPPCHECKGUI_RCC_SRCS})
target_link_libraries(cppcheck-gui ${CPPCHECKGUI_LIBS})

if (CMAKE_COMPILER_IS_GNUCXX)
  set_target_properties(cppcheck-gui PROPERTIES COMPILE_FLAGS -Wall)
endif (CMAKE_COMPILER_IS_GNUCXX)

if(CPPCHECK_GUI_USE_STATIC_LIBRARY)
  add_definitions(${CPPCHECK_STATIC_COMPILE_DEFINITIONS})
  set_target_properties(cppcheck-gui PROPERTIES
    LINK_FLAGS "${CPPCHECK_STATIC_LINK_FLAGS}")
endif()

add_custom_target(translations ALL DEPENDS ${CPPCHECKGUI_QM})

install(TARGETS cppcheck-gui
  EXPORT CPPCHECKGUIExports
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(EXPORT CPPCHECKGUIExports
  DESTINATION share/cppcheck/cmake)

install(FILES ${CPPCHECKGUI_QM}
  DESTINATION share/cppcheck/nls)

# TODO app-bundle on APPLE

if(APPLE OR (WIN32 AND NOT CYGWIN))
  include(InstallRequiredSystemLibraries)
  set(APP ${CMAKE_INSTALL_PREFIX}/bin/cppcheck-gui${CMAKE_EXECUTABLE_SUFFIX})
  set(DIRS ${QT_BINARY_DIR} ${QT_LIBRARY_DIR})
  install(CODE "
include(BundleUtilities)
# That horrible MSVC env-variable seems to creep back in and breaks the
# super-build, so get rid of it again.
set(ENV{VS_UNICODE_OUTPUT})
fixup_bundle(\"${APP}\"  \"\"  \"${DIRS}\")
")
endif()
