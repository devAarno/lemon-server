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

set(CMAKE_VERBOSE_MAKEFILE on)

set(EXTRA_SANITIZE "-fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=leak -fsanitize=undefined -fsanitize-address-use-after-scope -fsanitize=shift-base -fsanitize=shift-exponent -fsanitize=integer-divide-by-zero -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=null -fsanitize=return -fsanitize=signed-integer-overflow -fsanitize=bounds -fsanitize=bounds-strict -fsanitize=alignment -fsanitize=object-size -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fsanitize=nonnull-attribute -fsanitize=returns-nonnull-attribute -fsanitize=bool -fsanitize=enum -fsanitize=vptr -fsanitize=pointer-overflow -fsanitize=builtin")
set(EXTRA_DEBUG_MODE "-O0 -ansi -pedantic -Wall -Wpedantic -Wextra -gdwarf-4 -ggdb -g3 -fvar-tracking-assignments -ginline-points -fcf-protection=full -fstack-protector-all ${EXTRA_SANITIZE}")

add_subdirectory (3rdParty/lemon)
add_subdirectory (src)

option(WITHTESTS "Compile tests")

if(WITHTESTS)
  enable_testing()
  add_subdirectory (3rdParty/unity)
  add_subdirectory (tests)
endif(WITHTESTS)

# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${EXTRA_DEBUG_MODE}")
# set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${EXTRA_DEBUG_MODE}")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${EXTRA_DEBUG_MODE}")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${EXTRA_DEBUG_MODE}")

add_executable(demo ./demoPage/hello.c)
add_dependencies(demo lemonServer)
target_link_libraries(demo lemonServer)
