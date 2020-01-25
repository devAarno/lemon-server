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

#include "httpRequest.h"

#include <stddef.h>
#include <string.h>

#include "string.h"
#include "../boolean.h"
#include "lemonError.h"
#include "httpRequestInternal.h"
#include "strncasecmp.h"

const lemonError initHttpRequest(httpRequest *r, const int fd) {
    if (NULL == r) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        r->descriptor = fd;
        r->elementsCount = r->body.length = 0;
        r->body.data = NULL;

        /* Adds fake EMPTY element into zero position */
        ((r->elements)[0]).type = VALUE;
        ((r->elements)[0]).value.str = getEmptyString();
        ((r->elements)[0]).value.nextVal = NULL;
        r->elementsCount = 1;
        /*appendElementOfHttpRequest(r, getEmptyString(), 0, VALUE);*/

        return LE_OK;
    }
}

const string *getMethodOfHttpRequest(const httpRequest *r) {
    if ((NULL == r) || (0 >= r->elementsCount)) {
        return NULL;
    }
    {
        const size_t elementsCount = r->elementsCount;
        size_t i;
        for (i = 0; i < elementsCount; ++i) {
            if (METHOD == ((r->elements)[i]).type) {
                return &(((r->elements)[i]).value.str);
            };
        };
        return NULL;
    }
}

const string *getUriOfHttpRequest(const httpRequest *r) {
    if ((NULL == r) || (0 >= r->elementsCount)) {
        return NULL;
    }
    {
        const size_t elementsCount = r->elementsCount;
        size_t i;
        for (i = 0; i < elementsCount; ++i) {
            if (URI == ((r->elements)[i]).type) {
                return &(((r->elements)[i]).value.str);
            };
        };
        return NULL;
    }
}

const string *getVersionOfHttpRequest(const httpRequest *r) {
    if ((NULL == r) || (0 >= r->elementsCount)) {
        return NULL;
    }
    {
        const size_t elementsCount = r->elementsCount;
        size_t i;
        for (i = 0; i < elementsCount; ++i) {
            if (HTTP_VERSION == ((r->elements)[i]).type) {
                return &(((r->elements)[i]).value.str);
            };
        };
        return NULL;
    }
}

const string *getQueryParameterOfHttpRequest(const httpRequest *r, const char *name) {
    if ((NULL == r) || (NULL == name) || (0 >= r->elementsCount)) {
        return NULL;
    }
    {
        const size_t nameLength = strlen(name);
        const size_t elementsCount = r->elementsCount;
        size_t i;
        for (i = 0; i < elementsCount; ++i) {
            if ((GET_QUERY_ELEMENT == ((r->elements)[i]).type) && (nameLength == ((r->elements)[i]).value.str.length) && (0 == STRNCASECMP(name, ((r->elements)[i]).value.str.data, nameLength))) {
                return &(((r->elements)[i]).value.nextVal->str);
            };
        };
        return NULL;
    }
}

const string *getHeaderOfHttpRequest(const httpRequest *r, const char *name) {
    if ((NULL == r) || (NULL == name) || (0 >= r->elementsCount)) {
        return NULL;
    }
    {
        const size_t nameLength = strlen(name);
        const size_t elementsCount = r->elementsCount;
        size_t i;
        for (i = 0; i < elementsCount; ++i) {
            if ((HEADER == ((r->elements)[i]).type) && (nameLength == ((r->elements)[i]).value.str.length) && (0 == STRNCASECMP(name, ((r->elements)[i]).value.str.data, nameLength))) {
                return &(((r->elements)[i]).value.nextVal->str);
            };
        };
        return NULL;
    }
}

const string *getBodyBufferOfHttpRequest(const httpRequest *r) {
    if ((NULL == r) || (0 >= r->elementsCount)) {
        return NULL;
    }
    return &(r->body);
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
