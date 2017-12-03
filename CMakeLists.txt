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
project (lemonServer C)

set(CMAKE_VERBOSE_MAKEFILE on)
add_definitions(-g -O0)
#add_definitions(-O3 -flto -march=broadwell)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra")

add_subdirectory (3rdParty/lemon)
add_subdirectory (3rdParty/makeheaders)
add_subdirectory (src)

option(WITHTESTS "Compile tests")

if(WITHTESTS)
  enable_testing()
  add_subdirectory (3rdParty/unity)
  add_subdirectory (tests)
endif(WITHTESTS)

add_executable(demo ./demoPage/hello.c)
add_dependencies(demo lemonServer)
target_link_libraries(demo lemonServer)
