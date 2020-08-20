/*
 * Copyright (C) 2017, 2018, 2019, 2020 Parkhomenko Stanislav
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

#ifndef STRING_H
#define STRING_H

#include <stddef.h>

#include "../boolean.h"

typedef struct {
    char *data;
    size_t length;
} string;

typedef string buffer;

string createString(char* chars);

const string getEmptyString();

const boolean isEmptyString(const string s);

const string getTrueString();

const boolean isTrueString(const string s);

const string getFalseString();

const boolean isFalseString(const string s);

const string getNullString();

const boolean isNullString(const string s);

#endif /* STRING_H */

