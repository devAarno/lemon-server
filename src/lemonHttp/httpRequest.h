/*
 * Copyright (C) 2017, 2018, 2019, 2020, 2021 Parkhomenko Stanislav
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

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <stddef.h>

#include "string.h"
#include "../boolean.h"
#include "lemonError.h"

#define MAX_ELEMENTS 128

#define PRIVATE_BUFFER_SIZE 2048

typedef enum {
    METHOD,
    URI,
    GET_QUERY_ELEMENT,
    VALUE,
    HTTP_VERSION,
    HEADER
} elementType;

typedef struct _linkedDataString {
    string str;
    struct _linkedDataString *nextVal;
} linkedDataString;

typedef struct {
    linkedDataString value;
    elementType type;
} requestElement;

typedef struct {
    requestElement elements[MAX_ELEMENTS];
    char privateBuffer[PRIVATE_BUFFER_SIZE];
    string body;
    size_t elementsCount;
    int descriptor;
} httpRequest;

const lemonError initHttpRequest(httpRequest *r, const int fd);

const string *getMethodOfHttpRequest(const httpRequest *r);

const string *getUriOfHttpRequest(const httpRequest *r);

const string *getVersionOfHttpRequest(const httpRequest *r);

const string *getQueryParameterOfHttpRequest(const httpRequest *r, const char *name);

const string *getHeaderOfHttpRequest(const httpRequest *r, const char *name);

const string *getBodyBufferOfHttpRequest(const httpRequest *r);

const boolean isStringEmpty(const string *s);

#endif /* HTTP_REQUEST_H */

