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

#ifndef HTTP_REQUEST_INT_H
#define HTTP_REQUEST_INT_H

#include <stddef.h>

#include "httpRequest.h"
#include "lemonError.h"
#include "string.h"

lemonError decodeValue(string *s, boolean replacePlusWithSpace);

lemonError trim(string *s);

lemonError executeHttpMethodCallback(const httpRequest *r, const string *s);

lemonError executeHttpUriCallback(const httpRequest *r, const string *s);

lemonError executeHttpVersionCallback(const httpRequest *r, const string *s);

lemonError executeHeaderCallback(const httpRequest *r, const string *key, const string *s);

lemonError executeGetParameterCallback(const httpRequest *r, const string *key, const string *s);

string convertUtf16ToString(char *c1, char *c2, const char c3, const char c4);

#endif /* HTTP_REQUEST_INT_H */

