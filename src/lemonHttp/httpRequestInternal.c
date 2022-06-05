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

#include "httpRequestInternal.h"

#include <stddef.h>
#include <string.h>


#include "string.h"
#include "strncasecmp.h"
#include "lemonError.h"

/* inspired by https://stackoverflow.com/questions/2673207/c-c-url-decode-library */
#undef NA
#define NA 127
static const char tbl[256] = {
    NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, NA, NA, NA, NA, NA, NA,
    NA, 10, 11, 12, 13, 14, 15, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    NA, 10, 11, 12, 13, 14, 15, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA
};

const lemonError decodeValue(string *s, boolean replacePlusWithSpace) {
    const string emptyString = getEmptyString();
    if ((NULL == s) || (NULL == s->data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (((emptyString.length == s->length) && (emptyString.data != s->data)) ||
            ((emptyString.length != s->length) && (emptyString.data == s->data))) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    {
        const size_t oldSize = s->length;
        size_t readPos = 0;
        size_t writePos = 0;
        char a, b;
        while (readPos < oldSize) {
            switch ((s->data)[readPos]) {
                case '%':
                    switch ((readPos < oldSize - 2) ? (a = tbl[(s->data)[readPos + 1]]) : NA) {
                        case NA:
                            return LE_PARSING_IS_FAILED;
                            break;
                        default:
                            switch (b = tbl[(s->data)[readPos + 2]]) {
                                case NA:
                                    return LE_PARSING_IS_FAILED;
                                    break;
                                default:
                                    /* (s->data)[writePos] = 16 * a + b; */
                                    (s->data)[writePos] = (a << 4) | b;
                                    readPos += 3;
                                    ++writePos;
                                    break;
                            }
                            break;
                    }
                    break;
                case '+':
                    (s->data)[writePos] = ((TRUE == replacePlusWithSpace) ? ' ' : (s->data)[readPos]);
                    ++writePos;
                    ++readPos;
                    break;
                default:
                    (s->data)[writePos] = (s->data)[readPos];
                    ++writePos;
                    ++readPos;
                    break;
            }
        }
        s->length = writePos;
    }
    return LE_OK;
}

requestElement *appendElementOfHttpRequest(httpRequest *r, const string *s, const ruleType type) {
    /*const string emptyString = getEmptyString();
    if ((NULL == r) || (NULL == s) || (NULL == s->data) ||
            (0 >= r->elementsCount) ||
            (((emptyString.length == s->length) && (emptyString.data != s->data)) || ((emptyString.length != s->length) && (emptyString.data == s->data))) ||
            ((emptyString.length == s->length) && (emptyString.data == s->data) && (VALUE != type)) ||
            (emptyString.length > s->length)) {
        return NULL;
    }
    {
        const size_t elementNo = (r->elementsCount)++;
        ((r->elements)[elementNo]).type = type;
        ((r->elements)[elementNo]).value.str.data = s->data;
        ((r->elements)[elementNo]).value.str.length = s->length;
        ((r->elements)[elementNo]).value.nextVal = NULL;
        return &((r->elements)[elementNo]);
    }*/
    return LE_OK;
}

const lemonError linkRequestElement(requestElement *key, const requestElement *value) {
    /*if ((NULL == key) || (NULL == key->value.str.data) ||
            (NULL == value) || (NULL == value->value.str.data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        linkedDataString *pos = &(key->value);
        while (NULL != pos->nextVal) {
            pos = pos->nextVal;
        };
        pos->nextVal = (linkedDataString *)(&(value->value));
        return LE_OK;
    }*/
    return LE_OK;
}

const requestElement *getEmptyValueElement(const httpRequest *r) {
    if ((NULL == r) || (0 >= r->elementsCount)) {
        return NULL;
    }
    return &(r->elements[0]);
}

const lemonError trim(string *s) {
    const string emptyString = getEmptyString();
    if ((NULL == s) || (NULL == s->data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (((emptyString.length == s->length) && (emptyString.data != s->data)) ||
            ((emptyString.length != s->length) && (emptyString.data == s->data))) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    while ((((s->data)[0] == ' ') || ((s->data)[0] == '\t')) && (0 < s->length)) {
        ++(s->data);
        --(s->length);
    };
    while ((((s->data)[s->length - 1] == ' ') || ((s->data)[s->length - 1] == '\t')) && (0 < s->length)) {
        --(s->length);
    };

    if (0 == s->length) {
        s->data = emptyString.data;
    }

    return LE_OK;
}

const lemonError executeHttpMethodCallback(httpRequest *r, const string *s) {
    if (NULL == r) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        const size_t lastElement = r->elementsCount;
        size_t i;
        for (i = 0; i < lastElement; ++i) {
            if (HTTP_REQUEST_METHOD == (r->elements)[i].type) {
                const httpMethodCallback callback = (r->elements)[i].data.httpMethodCallback;
                callback.handler(s, callback.data);
            }
        }
    }
    return LE_OK;
}

const lemonError executeHttpUriCallback(httpRequest *r, const string *s) {
    if (NULL == r) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        const size_t lastElement = r->elementsCount;
        size_t i;
        for (i = 0; i < lastElement; ++i) {
            if (HTTP_REQUEST_URI == (r->elements)[i].type) {
                const httpUriCallback callback = (r->elements)[i].data.httpUriCallback;
                callback.handler(s, callback.data);
            }
        }
    }
    return LE_OK;
}

const lemonError executeHttpVersionCallback(httpRequest *r, const string *s) {
    if (NULL == r) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        const size_t lastElement = r->elementsCount;
        size_t i;
        for (i = 0; i < lastElement; ++i) {
            if (HTTP_REQUEST_HTTP_VERSION == (r->elements)[i].type) {
                const httpVersionCallback callback = (r->elements)[i].data.httpVersionCallback;
                callback.handler(s, callback.data);
            }
        }
    }
    return LE_OK;
}

const lemonError executeHeaderCallback(httpRequest *r, const string *key, const string *s) {
    if (NULL == r) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        const size_t lastElement = r->elementsCount;
        size_t i;
        for (i = 0; i < lastElement; ++i) {
            if ((HTTP_REQUEST_HEADER == (r->elements)[i].type) && ( key->length == (r->elements)[i].data.httpHeaderQueryCallback.headerName.length) && (0 == STRNCASECMP(key->data, (r->elements)[i].data.httpHeaderQueryCallback.headerName.data, key->length))) {
                const httpHeaderQueryCallback callback = (r->elements)[i].data.httpHeaderQueryCallback;
                callback.handler(s, callback.data);
            }
        }
    }
    return LE_OK;
}

const lemonError executeGetParameterCallback(httpRequest *r, const string *key, const string *s) {
    if (NULL == r) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        const size_t lastElement = r->elementsCount;
        size_t i;
        for (i = 0; i < lastElement; ++i) {
            if ((HTTP_REQUEST_GET_QUERY_ELEMENT == (r->elements)[i].type) && ( key->length == (r->elements)[i].data.httpGetParameterCallback.getParameter.length) && (0 == STRNCASECMP(key->data, (r->elements)[i].data.httpGetParameterCallback.getParameter.data, key->length))) {
                const httpGetParameterCallback callback = (r->elements)[i].data.httpGetParameterCallback;
                callback.handler(s, callback.data);
            }
        }
    }
    return LE_OK;
}