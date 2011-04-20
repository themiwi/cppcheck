# check arguments
foreach(v DUMPBIN OUTPUT LIBRARY)
  if(NOT DEFINED ${v})
    message(FATAL_ERROR "${v} not defined on command line")
  endif()
endforeach()

# run dumpbin on the library to extract the symbols table
execute_process(
  COMMAND "${DUMPBIN}" /SYMBOLS "${LIBRARY}" /OUT:dumpbinoutput.tmp
  RESULT_VARIABLE retvalE
  ERROR_STRIP_TRAILING_WHITESPACE)
if(retval)
  message(FATAL_ERROR "DUMPBIN failed")
endif()

set(str "set(LIBCPPCHECK_STATIC_LINK_FLAGS \"")
# escape semicolons and make each line a list item
file(READ dumpbinoutput.tmp output)
string(REPLACE ";" "<semicolon>" output "${output}")
string(REPLACE "\n" ";" output "${output}")
# search for Check* instances in then anonymous namespace
foreach(line ${output})
  string(REPLACE "<semicolon>" ";" line "${line}")
  if(NOT line MATCHES UNDEF AND
      line MATCHES "notype.*\\| (\\?instance[^s][^ \t]+Check[^ \t]+) ")
    set(str "${str} /INCLUDE:${CMAKE_MATCH_1}")
  endif()
endforeach()
set(str "${str}\")")

file(WRITE "${OUTPUT}" "# Automatically generated. Do not edit!\n\n${str}\n")
