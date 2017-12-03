#
# Copyright (C) 2017 Parkhomenko Stanislav
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

cmake_minimum_required (VERSION 2.4)
project (makeheaders C)

include(CheckIncludeFiles)
include(CheckSymbolExists)

set(allHeaders stdio.h stdlib.h ctype.h memory.h sys/stat.h assert.h)

set(allSymbols 
  exit fclose fopen fprintf fread free fwrite malloc memset printf realloc 
  sprintf stderr stdout strcmp strcpy strlen strncmp strncpy isspace isdigit 
  isalnum isalpha isxdigit getc EOF)

if (WIN32)
  add_definitions(-D__WIN32__)
  set(allHeaders "${allHeaders}" string.h)
else (WIN32)
  set(allHeaders "${allHeaders}" unistd.h)
  set(allSymbols "${allSymbols}" S_ISREG)
endif (WIN32)

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

add_executable(makeheaders makeheaders.c)
