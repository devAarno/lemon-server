#
# Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022 Parkhomenko Stanislav
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

cmake_minimum_required (VERSION 3.0)
project (unity C)

include(ExternalProject)
include(CheckIncludeFile)

#ExternalProject_Add(gitunity
#  PREFIX "unity"
#  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/git
#  TMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/tmp
#  STAMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/stamp
#  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin
#  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
#  GIT_TAG "v2.4.1"
#  CONFIGURE_COMMAND ""
#  BUILD_COMMAND ""
#  INSTALL_COMMAND "")

if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject.cmake)
download_project(PROJ                gitunity
                 GIT_REPOSITORY      https://github.com/ThrowTheSwitch/Unity.git
                 GIT_TAG             "v2.4.1"
                 SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/git
                 TMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/tmp
                 STAMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/stamp
                 BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

set(unitySrc ${CMAKE_CURRENT_BINARY_DIR}/git/src)

set(allHeaders stddef.h)

check_include_file(stddef.h HAS_${PROJECT_NAME}_HEADER_STDDEF)
if (NOT HAS_${PROJECT_NAME}_HEADER_STDDEF)
  message(FATAL_ERROR "Headers stddef.h are required.")
endif (NOT HAS_${PROJECT_NAME}_HEADER_STDDEF)

check_include_file(stdint.h HAS_${PROJECT_NAME}_HEADER_STDINT)
if (NOT HAS_${PROJECT_NAME}_HEADER_STDINT)
  add_definitions(-DUNITY_EXCLUDE_STDINT_H)
else (NOT HAS_${PROJECT_NAME}_HEADER_STDINT)
  set(allHeaders "${allHeaders}" stdint.h)
endif (NOT HAS_${PROJECT_NAME}_HEADER_STDINT)

check_include_file(limits.h HAS_${PROJECT_NAME}_HEADER_LIMITS)
if (NOT HAS_${PROJECT_NAME}_HEADER_LIMITS)
  add_definitions(-DUNITY_EXCLUDE_LIMITS_H)
else (NOT HAS_${PROJECT_NAME}_HEADER_LIMITS)
  set(allHeaders "${allHeaders}" limits.h)
endif (NOT HAS_${PROJECT_NAME}_HEADER_LIMITS)

check_include_file(setjmp.h HAS_${PROJECT_NAME}_HEADER_SETJMP)
if (NOT HAS_${PROJECT_NAME}_HEADER_SETJMP)
  add_definitions(-DUNITY_EXCLUDE_SETJMP_H)
else (NOT HAS_${PROJECT_NAME}_HEADER_SETJMP)
  set(allHeaders "${allHeaders}" setjmp.h)
endif (NOT HAS_${PROJECT_NAME}_HEADER_SETJMP)

check_include_file(math.h HAS_${PROJECT_NAME}_HEADER_MATH)
if (NOT HAS_${PROJECT_NAME}_HEADER_MATH)
  add_definitions(-DUNITY_EXCLUDE_MATH_H)
else (NOT HAS_${PROJECT_NAME}_HEADER_MATH)
  set(allHeaders "${allHeaders}" math.h)
endif (NOT HAS_${PROJECT_NAME}_HEADER_MATH)

check_include_file(stdio.h HAS_${PROJECT_NAME}_HEADER_STDIO)
if (NOT HAS_${PROJECT_NAME}_HEADER_STDIO)
  add_definitions(-DUNITY_EXCLUDE_MATH_H)
else (NOT HAS_${PROJECT_NAME}_HEADER_STDIO)
  set(allHeaders "${allHeaders}" stdio.h)
endif (NOT HAS_${PROJECT_NAME}_HEADER_STDIO)

check_symbol_exists(fflush "${allHeaders}" HAS_${PROJECT_NAME}_FFLUSH)
if (HAS_${PROJECT_NAME}_FFLUSH)
  add_definitions(-DUNITY_USE_FLUSH_STDOUT)
endif (HAS_${PROJECT_NAME}_FFLUSH)

check_symbol_exists(NULL "${allHeaders}" HAS_${PROJECT_NAME}_NULL)
if (NOT HAS_${PROJECT_NAME}_NULL)
  message(FATAL_ERROR "NULL is required.")
endif (NOT HAS_${PROJECT_NAME}_NULL)

add_definitions(-DUNITY_EXCLUDE_FLOAT -DUNITY_EXCLUDE_DOUBLE)

file(GLOB_RECURSE allSrc RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${unitySrc}/*.[c|h])

add_library(${PROJECT_NAME} SHARED ${allSrc})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)
