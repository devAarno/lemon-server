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

set(EXTRA_C_STD "-std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition  -Wall -Wpedantic -Wextra")
set(EXTRA_SANITIZE "-fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=leak -fsanitize=undefined -fsanitize-address-use-after-scope -fsanitize=shift-base -fsanitize=shift-exponent -fsanitize=integer-divide-by-zero -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=null -fsanitize=return -fsanitize=signed-integer-overflow -fsanitize=bounds -fsanitize=bounds-strict -fsanitize=alignment -fsanitize=object-size -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fsanitize=nonnull-attribute -fsanitize=returns-nonnull-attribute -fsanitize=bool -fsanitize=enum -fsanitize=vptr -fsanitize=pointer-overflow -fsanitize=builtin")
set(EXTRA_DEBUG_MODE "-O0 -gdwarf-4 -ggdb -g3 -fvar-tracking-assignments -gstatement-frontiers -ginline-points -fcf-protection=full -fstack-protector-all")
set(EXTRA_PERFORMANCE "-Ofast -ftree-vectorize -fweb -fno-semantic-interposition -fipa-pta -fdevirtualize-at-ltrans -fdevirtualize-speculatively -fira-region=all -floop-nest-optimize -floop-parallelize-all -fgraphite-identity -ftree-loop-im -ftree-loop-ivcanon -fivopts -ftree-vectorize -fmodulo-sched -fmodulo-sched-allow-regmoves -fgcse-sm -fgcse-las")
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${EXTRA_DEBUG_MODE}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${EXTRA_DEBUG_MODE}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${EXTRA_PERFORMANCE}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} ${EXTRA_PERFORMANCE}")
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Lemon fails with sanitizers :(
# ... and it is not fully C89 as the gcc says
add_subdirectory (3rdParty/lemon)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${EXTRA_SANITIZE} ${EXTRA_C_STD}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${EXTRA_SANITIZE} ${EXTRA_C_STD}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${EXTRA_C_STD}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${EXTRA_C_STD}")
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
