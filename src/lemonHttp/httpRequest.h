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
        onStartCallback onStartCallback;
        finalOnSuccessCallback finalOnSuccessCallback;

        /* HTTP */
        httpMethodCallback httpMethodCallback;
        httpUriCallback httpUriCallback;
        httpVersionCallback httpVersionCallback;
        httpGetParameterCallback httpGetParameterCallback;
        httpHeaderQueryCallback httpHeaderQueryCallback;

        /* JSON Path */
        rootRule root;
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
    unsigned char mode;
} httpRequest;

lemonError initHttpRequest(httpRequest *r);

lemonError appendHttpMethodRequest(httpRequest *r, httpMethodExecutionHandler handler, changingData *data);

lemonError appendHttpUriRequest(httpRequest *r, httpUriExecutionHandler handler, changingData *data);

lemonError appendHttpVersionRequest(httpRequest *r, httpVersionExecutionHandler handler, changingData *data);

lemonError appendHttpGetParameterQueryRequest(httpRequest *r, const char *b, httpGetParameterQueryExecutionHandler handler, changingData *data);

lemonError appendHttpHeaderQueryRequest(httpRequest *r, const char *b, httpHeaderQueryExecutionHandler handler, changingData *data);

lemonError appendOnSuccess(httpRequest *r, finalOnSuccessExecutionHandler handler, changingData *data);

lemonError appendOnStart(httpRequest *r, onStartExecutionHandler handler, changingData *data);

boolean isStringEmpty(const string *s);

#endif /* HTTP_REQUEST_H */

