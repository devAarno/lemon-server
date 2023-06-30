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

#include "httpRequest.h"

#include <stddef.h>
#include <string.h>

#include "string.h"
#include "../boolean.h"
#include "lemonError.h"
#include "httpRequestInternal.h"
#include "strncasecmp.h"
#include "changingData.h"

lemonError initHttpRequest(httpRequest *r) {
    if (NULL == r) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        r->elementsCount = r->body.length = 0;
        r->body.data = NULL;
        r->mode = 0;
        r->elementsCount = 0;
        r->parsedStackSize = 0;

        return LE_OK;
    }
}

lemonError appendHttpMethodRequest(httpRequest *r, const httpMethodExecutionHandler handler, changingData *data) {
    if ((NULL == r) || (NULL == handler) || (NULL == data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        requestElement element;

        httpMethodCallback callback;
        callback.handler = handler;
        callback.data = data;

        element.data.httpMethodCallback = callback;
        element.type = HTTP_REQUEST_METHOD;
        (r->elements)[(r->elementsCount)++] = element;
    }
    return LE_OK;
}

lemonError appendHttpUriRequest(httpRequest *r, const httpUriExecutionHandler handler, changingData *data) {
    if ((NULL == r) || (NULL == handler) || (NULL == data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        requestElement element;

        httpUriCallback callback;
        callback.handler = handler;
        callback.data = data;

        element.data.httpUriCallback = callback;
        element.type = HTTP_REQUEST_URI;
        (r->elements)[(r->elementsCount)++] = element;
    }
    return LE_OK;
}

lemonError appendHttpVersionRequest(httpRequest *r, const httpVersionExecutionHandler handler, changingData *data) {
    if ((NULL == r) || (NULL == handler) || (NULL == data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        requestElement element;

        httpVersionCallback callback;
        callback.handler = handler;
        callback.data = data;

        element.data.httpVersionCallback = callback;
        element.type = HTTP_REQUEST_HTTP_VERSION;
        (r->elements)[(r->elementsCount)++] = element;
    }
    return LE_OK;
}

lemonError appendHttpGetParameterQueryRequest(httpRequest *r, const char *b, const httpGetParameterQueryExecutionHandler handler, changingData *data) {
    if ((NULL == r) || (NULL == b) || (NULL == handler) || (NULL == data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        requestElement element;

        httpGetParameterCallback callback;
        callback.handler = handler;
        callback.data = data;
        callback.getParameter = createString(b);

        element.data.httpGetParameterCallback = callback;
        element.type = HTTP_REQUEST_GET_QUERY_ELEMENT;
        (r->elements)[(r->elementsCount)++] = element;
    }
    return LE_OK;
}

lemonError appendHttpHeaderQueryRequest(httpRequest *r, const char *b, const httpHeaderQueryExecutionHandler handler, changingData *data) {
    if ((NULL == r) || (NULL == b) || (NULL == handler) || (NULL == data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        requestElement element;

        httpHeaderQueryCallback callback;
        callback.handler = handler;
        callback.data = data;
        callback.headerName = createString(b);

        element.data.httpHeaderQueryCallback = callback;
        element.type = HTTP_REQUEST_HEADER;
        (r->elements)[(r->elementsCount)++] = element;
    }
    return LE_OK;
}

lemonError appendOnSuccess(httpRequest *r, const finalOnSuccessExecutionHandler handler, changingData *data) {
    if ((NULL == r) || (NULL == handler) || (NULL == data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        requestElement element;

        finalOnSuccessCallback callback;
        callback.handler = handler;
        callback.data = data;

        element.data.finalOnSuccessCallback = callback;
        element.type = FINAL_ON_SUCCESS_CALLBACK;
        (r->elements)[(r->elementsCount)++] = element;
    }
    return LE_OK;
}

lemonError appendOnStart(httpRequest *r, const onStartExecutionHandler handler, changingData *data) {
    if ((NULL == r) || (NULL == handler) || (NULL == data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        requestElement element;

        onStartCallback callback;
        callback.handler = handler;
        callback.data = data;

        element.data.onStartCallback = callback;
        element.type = ON_START_CALLBACK;
        (r->elements)[(r->elementsCount)++] = element;
    }
    return LE_OK;
}

boolean isStringEmpty(const string *s) {
    if (NULL == s) {
        return TRUE;
    }
    {
        const string empty = getEmptyString();
        return ((empty.length == s->length) && (empty.data == s->data)) ? TRUE : FALSE;
    }
}
