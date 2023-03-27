/*
 * Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022, 2023 Parkhomenko Stanislav
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

#include "strncasecmp.h"

/** FreeBSD implementation
 * https://svnweb.freebsd.org/base/head/lib/libc/string/strcasecmp.c?view=markup
 */

#ifdef USE_INTERNAL_STRNCASECMP

#include <ctype.h>

static int strncasecmp_internal(const char *s1, const char *s2, size_t n) {
    if (n != 0) {
        do {
            if (tolower(*s1) != tolower(*s2++))
                return (tolower(*s1) - tolower(*--s2));
            if (*s1++ == '\0')
                break;
        } while (--n != 0);
    }
    return (0);
}
#endif