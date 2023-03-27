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
project (lemonServer C)

include(CheckIncludeFiles)
include(CheckIncludeFile)
include(CheckSymbolExists)

set(allHeaders stdio.h assert.h stddef.h string.h ctype.h stdlib.h stdint.h 
  sys/socket.h sys/types.h arpa/inet.h unistd.h)

set(allSymbols 
  accept bind fprintf fwrite htonl htons listen memset printf puts read socket 
  strlen tolower NULL AF_INET SOCK_STREAM INADDR_ANY)

set(httpRequestDefs "")

check_include_files("${allHeaders}" ${PROJECT_NAME}_commonHeaders)
if (NOT ${PROJECT_NAME}_commonHeaders)
  message(FATAL_ERROR "Headers ${allHeaders} are required.")
endif (NOT ${PROJECT_NAME}_commonHeaders)

check_include_file("strings.h" ${PROJECT_NAME}_strings)
if (${PROJECT_NAME}_strings)
  set(allHeaders "${allHeaders}" "strings.h")
  set(httpRequestDefs "${httpRequestDefs} -DUSE_STRINGS_H")
endif (${PROJECT_NAME}_strings)
# END: Append to allHeaders some possible headers
message("${allHeaders}")

foreach(s ${allSymbols})
  string(TOUPPER ${PROJECT_NAME}_${s} sup)
  check_symbol_exists(${s} "${allHeaders}" HAS_${sup})
  if (NOT HAS_${sup})
    message(FATAL_ERROR "${s} is required.")
  endif (NOT HAS_${sup})
endforeach(s)

# START: Use internal strncasecmp or get it from a system library
set(s strncasecmp)
string(TOUPPER ${PROJECT_NAME}_${s} sup_strncasecmp)
check_symbol_exists(${s} "${allHeaders}" HAS_${sup_strncasecmp})


if (NOT HAS_${sup_strncasecmp})
  message(FATAL_ERROR "strncasecmp is not found.")
elseif (NOT HAS_${sup_strncasecmp})
  set(httpRequestDefs "${httpRequestDefs} -DUSE_INTERNAL_STRNCASECMP")
endif (NOT HAS_${sup_strncasecmp})
# END: Use internal strncasecmp or get it from a system library

# https://samthursfield.wordpress.com/2015/11/21/cmake-dependencies-between-targets-and-files-and-custom-commands/

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/http11Lemon.c ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/http11Lemon.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/http11Lemon.y ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/lempar.c lemon
  COMMAND lemon ARGS -DYYSTACKDEPTH=480 -DParseHTTP11_ENGINEALWAYSONSTACK -T${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/lempar.c ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/http11Lemon.y
  COMMENT "Generating a HTTP v1.1 parser."
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp)

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/bodyEngines/json/jsonLemon.c ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/bodyEngines/json/jsonLemon.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/bodyEngines/json/jsonLemon.y ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/lempar.c lemon
  COMMAND lemon ARGS -DYYSTACKDEPTH=480 -DParseJSON_ENGINEALWAYSONSTACK -T${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/lempar.c ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/bodyEngines/json/jsonLemon.y
  COMMENT "Generating a JSON parser."
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp)

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/bodyEngines/json/jsonPathLemon.c ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/bodyEngines/json/jsonPathLemon.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/bodyEngines/json/jsonPathLemon.y ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/lempar.c lemon
  COMMAND lemon ARGS -DYYSTACKDEPTH=480 -DParseJSONPath_ENGINEALWAYSONSTACK -T${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/lempar.c ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/bodyEngines/json/jsonPathLemon.y
  COMMENT "Generating a limited JSONPath parser."
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp)

add_custom_target(parser
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/http11Lemon.c ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/http11Lemon.h)

add_custom_target(json
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/bodyEngines/json/jsonLemon.c ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/bodyEngines/json/jsonLemon.h)

add_custom_target(jsonPath
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/bodyEngines/json/jsonPathLemon.c ${CMAKE_CURRENT_SOURCE_DIR}/lemonHttp/bodyEngines/json/jsonPathLemon.h)

add_dependencies(parser jsonPath json lemon)

add_dependencies(parser lemon)

file(GLOB_RECURSE allSrc RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.[c|h])
list(REMOVE_ITEM allSrc lemonHttp/lempar.c)
list(REMOVE_ITEM allSrc lemonHttp/http11Lemon.c lemonHttp/http11Lemon.h)
list(REMOVE_ITEM allSrc lemonHttp/bodyEngines/json/jsonLemon.c lemonHttp/bodyEngines/json/jsonLemon.h)
list(REMOVE_ITEM allSrc lemonHttp/bodyEngines/json/jsonPathLemon.c lemonHttp/bodyEngines/json/jsonPathLemonmake.h)

set_source_files_properties(lemonHttp/http_request.c PROPERTIES COMPILE_FLAGS "${httpRequestDefs}")

# add_definitions(-DYYSTACKDEPTH=480 -DParseHTTP11_ENGINEALWAYSONSTACK -DParseJSON_ENGINEALWAYSONSTACK -DParseJSONPath_ENGINEALWAYSONSTACK)

add_library(${PROJECT_NAME} SHARED ${allSrc})
add_dependencies(${PROJECT_NAME} parser)
