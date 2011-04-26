/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2007-2011 Daniel Marjamäki and Cppcheck team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DLLVisibilityH
#define DLLVisibilityH

#if defined(_WIN32) || defined (__CYGWIN__)
  #ifdef CPPCHECK_STATIC
    #define CPPCHECK_API
  #else
    // GCC on WIN32 also supports the __declspec syntax
    #ifdef CPPCHECK_EXPORTS
      #define CPPCHECK_API __declspec(dllexport)
    #else
      #define CPPCHECK_API __declspec(dllimport)
    #endif
  #endif
  #define CPPCHECK_LOCAL
#else
  #if __GNUC__ >= 4
    #define CPPCHECK_API   __attribute__ ((visibility("default")))
    #define CPPCHECK_LOCAL __attribute__ ((visibility("hidden")))
  #else
    #define CPPCHECK_API
    #define CPPCHECK_LOCAL
  #endif
#endif

#endif
