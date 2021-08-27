#
# Copyright (C) 2017, 2018, 2019, 2020, 2021 Parkhomenko Stanislav
#
# This file is part of Lemon Server.
#
# Lemon Server is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required (VERSION 2.4...3.18)
project (unittest C)

include(CheckIncludeFiles)
include(CheckSymbolExists)

set(allHeaders string.h stdio.h)

set(allSymbols strncpy fmemopen remove)

check_include_files("${allHeaders}" ${PROJECT_NAME}_commonHeaders)
if (NOT ${PROJECT_NAME}_commonHeaders)
  message(FATAL_ERROR "Headers ${allHeaders} are required.")
endif (NOT ${PROJECT_NAME}_commonHeaders)

foreach(s ${allSymbols})
  string(TOUPPER ${PROJECT_NAME}_${s} sup)
  check_symbol_exists(${s} "${allHeaders}" HAS_${sup})
  if (NOT HAS_${sup})
    message(FATAL_ERROR "${s} is required.")
  endif (NOT HAS_${sup})
endforeach(s)



file(GLOB_RECURSE allSrc RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} t_*.c)

foreach(s ${allSrc})
  get_filename_component(testname ${s} NAME_WE)
  file(STRINGS ${s} testDesription REGEX "\#define TESTNAME ")
  string(REPLACE "#define TESTNAME " "" testDesription ${testDesription} )
  string(STRIP ${testDesription} testDesription)
  add_executable(${testname} ${s})
  add_dependencies(${testname} unity lemonServer)
  target_link_libraries(${testname} unity lemonServer)
  add_test(NAME ${testDesription} COMMAND ${testname} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endforeach(s)

