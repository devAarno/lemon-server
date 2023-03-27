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

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <stddef.h>

#include "string.h"
#include "../boolean.h"
#include "lemonError.h"
#include "rules.h"
#include "httpCallbackType.h"
#include "changingData.h"

#ifndef MAX_ELEMENTS
#define MAX_ELEMENTS 128
#endif

#define PRIVATE_BUFFER_SIZE 2048

/* ----------------- */

typedef struct {
    union {
        rootRule root;
        httpMethodCallback httpMethodCallback;
        httpUriCallback httpUriCallback;
        httpVersionCallback httpVersionCallback;
        httpGetParameterCallback httpGetParameterCallback;
        httpHeaderQueryCallback httpHeaderQueryCallback;
        string name;
        indexRule index;
        char *containerStartPosition;
    } data;
    ruleType type;
} requestElement;

typedef struct {
    requestElement elements[MAX_ELEMENTS];
    char privateBuffer[PRIVATE_BUFFER_SIZE];
    string body; /* possible to delete */
    size_t elementsCount;
    size_t parsedStackSize;
    int descriptor;
    unsigned char mode;
} httpRequest;

/* const lemonError appendJsonPathRequest(jsonPathRequest *p, jsonPathQueryBuffer *b, jsonPathExecutionHandler handler, changingData *data); */

const lemonError initHttpRequest(httpRequest *r, const int fd);

const lemonError appendHttpMethodRequest(httpRequest *r, httpMethodExecutionHandler handler, changingData *data);

const lemonError appendHttpUriRequest(httpRequest *r, httpUriExecutionHandler handler, changingData *data);

const lemonError appendHttpVersionRequest(httpRequest *r, httpVersionExecutionHandler handler, changingData *data);

const lemonError appendHttpGetParameterQueryRequest(httpRequest *r, char *b, httpGetParameterQueryExecutionHandler handler, changingData *data);

const lemonError appendHttpHeaderQueryRequest(httpRequest *r, char *b, httpHeaderQueryExecutionHandler handler, changingData *data);



const string *getMethodOfHttpRequest(const httpRequest *r);

const string *getUriOfHttpRequest(const httpRequest *r);

const string *getVersionOfHttpRequest(const httpRequest *r);

const string *getQueryParameterOfHttpRequest(const httpRequest *r, const char *name);

const string *getHeaderOfHttpRequest(const httpRequest *r, const char *name);

const string *getBodyBufferOfHttpRequest(const httpRequest *r);

const boolean isStringEmpty(const string *s);

#endif /* HTTP_REQUEST_H */

