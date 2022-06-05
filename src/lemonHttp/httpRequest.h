/*
 * Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022 Parkhomenko Stanislav
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

#ifndef MAX_ELEMENTS
#define MAX_ELEMENTS 128
#endif

#define PRIVATE_BUFFER_SIZE 2048

/* ----------------- */
typedef enum {
    HTTP_REQUEST_METHOD,
    HTTP_REQUEST_URI,
    HTTP_REQUEST_GET_QUERY_ELEMENT,
    HTTP_REQUEST_VALUE,
    HTTP_REQUEST_HTTP_VERSION,
    HTTP_REQUEST_HEADER,
    JSONPATH_REQUEST_ROOT,
    JSONPATH_REQUEST_ANY,
    JSONPATH_REQUEST_ANYINDEX,
    JSONPATH_REQUEST_NAME,
    JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY,
    JSONPATH_REQUEST_INDEX,
    JSONPATH_REQUEST_RECURSIVE,
    PARSED_JSON_ROOT,
    PARSED_JSON_OBJECT,
    PARSED_JSON_JOINED_OBJECT,
    PARSED_JSON_HEAD_OF_JOINED_OBJECT,
    PARSED_JSON_INDEX,
    PARSED_JSON_FIELD,
    PARSED_JSON_FIELD_WITH_OBJECT,
    PARSED_JSON_RESOLVED_FIELD,
    ZERO,
    NONE
} ruleType;

typedef void changingData;
typedef const lemonError (*jsonPathExecutionHandler)(const string *value, changingData *data);

/* HTTP */
typedef const lemonError (*httpMethodExecutionHandler)(const string *value, changingData *data);
typedef const lemonError (*httpUriExecutionHandler)(const string *value, changingData *data);
typedef const lemonError (*httpVersionExecutionHandler)(const string *value, changingData *data);
/*typedef char jsonPathQueryBuffer; */
typedef char httpGetParameterQueryBuffer;
typedef const lemonError (*httpGetParameterQueryExecutionHandler)(const string *value, changingData *data);
typedef char httpHeaderQueryBuffer;
typedef const lemonError (*httpHeaderQueryExecutionHandler)(const string *value, changingData *data);

typedef struct {
    httpMethodExecutionHandler handler;
    changingData *data;
} httpMethodCallback;

typedef struct {
    httpUriExecutionHandler handler;
    changingData *data;
} httpUriCallback;

typedef struct {
    httpVersionExecutionHandler handler;
    changingData *data;
} httpVersionCallback;

typedef struct {
    httpGetParameterQueryExecutionHandler handler;
    changingData *data;
    string getParameter;
} httpGetParameterCallback;

typedef struct {
    httpHeaderQueryExecutionHandler handler;
    changingData *data;
    string headerName;
} httpHeaderQueryCallback;

typedef struct {
    jsonPathExecutionHandler handler;
    changingData *data;
} jsonPathCallback;

typedef struct {
    jsonPathCallback callback;
    size_t ruleSize;
} rootRule;

typedef struct {
    char *containerStartPosition;
    size_t index;
} indexRule;

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
    string body;
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

