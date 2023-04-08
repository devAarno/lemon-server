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

#include "string.h"

#include <string.h>
#include <stddef.h>

#include "../boolean.h"

static const char *emptyString = "";
static const char *trueString = "true";
static const char *falseString = "false";
static const char *nullString = "null";

string createString(char *chars) {
    string res;
    if (NULL == chars) {
        res.data = NULL;
        res.length = 0;
    } else {        
        res.data = chars;
        res.length = strlen(chars);
    }
    return res;
}

string getEmptyString() {
    string s;
    s.data = (char *)emptyString;
    s.length = 0;
    return s;
}

boolean isEmptyString(const string s) {
    return ((emptyString == s.data) && (0 == s.length)) ? TRUE : FALSE;
}

string getTrueString() {
    string s;
    s.data = (char *)trueString;
    s.length = 4;
    return s;
}

boolean isTrueString(const string s) {
    return ((trueString == s.data) && (4 == s.length)) ? TRUE : FALSE;
}

string getFalseString() {
    string s;
    s.data = (char *)falseString;
    s.length = 5;
    return s;
}

boolean isFalseString(const string s) {
    return ((falseString == s.data) && (5 == s.length)) ? TRUE : FALSE;
}

string getNullString() {
    string s;
    s.data = (char *)nullString;
    s.length = 4;
    return s;
}

boolean isNullString(const string s) {
    return ((nullString == s.data) && (4 == s.length)) ? TRUE : FALSE;
}
