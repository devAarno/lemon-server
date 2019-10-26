/*
 * Copyright (C) 2017, 2018, 2019 Parkhomenko Stanislav
 *
 * This file is part of Lemon Server.
 *
 * Lemon Server is free software: you can redistribute it and/or modify
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

#ifndef LEMONSERVER_STRNCASECMP_H
#define LEMONSERVER_STRNCASECMP_H

#ifdef USE_STRINGS_H
#include <strings.h>
#endif

#undef STRNCASECMP

#ifdef USE_INTERNAL_STRNCASECMP

int strncasecmp_internal(const char *s1, const char *s2, size_t n);

#define STRNCASECMP strncasecmp_internal
#else
#define STRNCASECMP strncasecmp
#endif

#endif /* LEMONSERVER_STRNCASECMP_H */
