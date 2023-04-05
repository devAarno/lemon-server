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

const lemonError initHttpRequest(httpRequest *r, const int fd) {
    if (NULL == r) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        r->descriptor = fd;
        r->elementsCount = r->body.length = 0;
        r->body.data = NULL;

        /* Adds fake EMPTY element into zero position */
        ((r->elements)[0]).type = ZERO;
        /* ((r->elements)[0]).value.str = getEmptyString();
        ((r->elements)[0]).value.nextVal = NULL; */
        r->elementsCount = 1;
        /*appendElementOfHttpRequest(r, getEmptyString(), 0, VALUE);*/

        r->mode = 0;

        return LE_OK;
    }
}

const lemonError appendHttpMethodRequest(httpRequest *r, httpMethodExecutionHandler handler, changingData *data) {
    if ((NULL == r) || (NULL == handler) || (NULL == data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        const size_t newRootPlace = r->elementsCount;
        (r->elements)[newRootPlace].type = HTTP_REQUEST_METHOD;
        (r->elements)[newRootPlace].data.httpMethodCallback.handler = handler;
        (r->elements)[newRootPlace].data.httpMethodCallback.data = data;
        ++(r->elementsCount);
    }
    return LE_OK;
}

const lemonError appendHttpUriRequest(httpRequest *r, httpUriExecutionHandler handler, changingData *data) {
    if ((NULL == r) || (NULL == handler) || (NULL == data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        const size_t newRootPlace = r->elementsCount;
        (r->elements)[newRootPlace].type = HTTP_REQUEST_URI;
        (r->elements)[newRootPlace].data.httpUriCallback.handler = handler;
        (r->elements)[newRootPlace].data.httpUriCallback.data = data;
        ++(r->elementsCount);
    }
    return LE_OK;
}

const lemonError appendHttpVersionRequest(httpRequest *r, httpVersionExecutionHandler handler, changingData *data) {
    if ((NULL == r) || (NULL == handler) || (NULL == data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        const size_t newRootPlace = r->elementsCount;
        (r->elements)[newRootPlace].type = HTTP_REQUEST_HTTP_VERSION;
        (r->elements)[newRootPlace].data.httpVersionCallback.handler = handler;
        (r->elements)[newRootPlace].data.httpVersionCallback.data = data;
        ++(r->elementsCount);
    }
    return LE_OK;
}

const lemonError appendHttpGetParameterQueryRequest(httpRequest *r, char *b, httpGetParameterQueryExecutionHandler handler, changingData *data) {
    if ((NULL == r) || (NULL == b) || (NULL == handler) || (NULL == data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        const size_t newRootPlace = r->elementsCount;
        (r->elements)[newRootPlace].type = HTTP_REQUEST_GET_QUERY_ELEMENT;
        (r->elements)[newRootPlace].data.httpGetParameterCallback.handler = handler;
        (r->elements)[newRootPlace].data.httpGetParameterCallback.data = data;
        (r->elements)[newRootPlace].data.httpGetParameterCallback.getParameter = createString(b);
        ++(r->elementsCount);
    }
    return LE_OK;
}

const lemonError appendHttpHeaderQueryRequest(httpRequest *r, char *b, httpHeaderQueryExecutionHandler handler, changingData *data) {
    if ((NULL == r) || (NULL == b) || (NULL == handler) || (NULL == data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        const size_t newRootPlace = r->elementsCount;
        (r->elements)[newRootPlace].type = HTTP_REQUEST_HEADER;
        (r->elements)[newRootPlace].data.httpHeaderQueryCallback.handler = handler;
        (r->elements)[newRootPlace].data.httpHeaderQueryCallback.data = data;
        (r->elements)[newRootPlace].data.httpHeaderQueryCallback.headerName = createString(b);
        ++(r->elementsCount);
    }
    return LE_OK;
}

const boolean isStringEmpty(const string *s) {
    if (NULL == s) {
        return TRUE;
    }
    {
        const string empty = getEmptyString();
        return ((empty.length == s->length) && (empty.data == s->data)) ? TRUE : FALSE;
    }
}
