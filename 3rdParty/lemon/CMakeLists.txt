#
# Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022, 2023 Parkhomenko Stanislav
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
project (lemon C)

include(CheckIncludeFiles)
include(CheckSymbolExists)

#add_definitions(-DTEST)

set(allHeaders stdio.h stdarg.h string.h ctype.h stdlib.h assert.h)
set(allSymbols 
  calloc exit fclose fgetc fgets fopen fprintf fputc fread free fseek 
  ftell getenv malloc memcmp memcpy memset printf qsort realloc rewind sprintf 
  strchr strcmp strlen strncmp strrchr strstr strtod strtol vfprintf 
  va_arg stderr va_start va_end assert putc NULL)

if (WIN32)
  add_definitions(-D__WIN32__)
  set(allSymbols "${allSymbols}" access)
else (WIN32)
  set(allHeaders "${allHeaders}" unistd.h)
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

add_executable(lemon lemon.c)
