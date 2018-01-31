/*
 * Copyright (C) 2017, 2018 Parkhomenko Stanislav
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

#include "http_request.h"
#include "lemonHttpError.h"

const lemonError decodeValue(string *s, boolean replacePlusWithSpace);

requestElement *appendElementOfHttpRequest(httpRequest *r, const string *s, const elementType type);

const lemonError linkRequestElement(requestElement *key, const requestElement *value);

const string getEmptyString();

const requestElement *getEmptyValueElement(const httpRequest *r);

const lemonError trim(string *s);
#endif /* HTTP_REQUEST_INT_H */

