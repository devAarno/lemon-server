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

#include "httpRequestInternal.h"

#include <stddef.h>
#include <string.h>


#include "parser.h"
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

lemonError decodeValue(string *s, const boolean replacePlusWithSpace) {
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

lemonError trim(string *s) {
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

lemonError executeHttpMethodCallback(const httpRequest *r, const string *s) {
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

lemonError executeHttpUriCallback(const httpRequest *r, const string *s) {
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

lemonError executeHttpVersionCallback(const httpRequest *r, const string *s) {
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

lemonError executeHeaderCallback(const httpRequest *r, const string *key, const string *s) {
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

lemonError executeGetParameterCallback(const httpRequest *r, const string *key, const string *s) {
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

string convertUtf16ToString(char *c1, char *c2, const char c3, const char c4) {
    string res;
    if ((NULL == c1) || (NULL == c2)) {
        res.data = NULL;
        res.length = 0;
        return res;
    }
    {
        res.data = c1;
        if (('0' == *c1) && ('0' == *c2)) {

            res.data[0] = (((c3 <= '9') ? (c3 - '0') : ((c3 - 'A') % 32)) << 4) | ((c4 <= '9') ? (c4 - '0') : ((c4 - 'A') % 32));
            res.length = 1;
        } else {
            res.data[0] = (((*c1 <= '9') ? (*c1 - '0') : ((*c1 - 'A') % 32)) << 4) | ((*c2 <= '9') ? (*c2 - '0') : ((*c2 - 'A') % 32));
            res.data[1] = (((c3 <= '9') ? (c3 - '0') : ((c3 - 'A') % 32)) << 4) | ((c4 <= '9') ? (c4 - '0') : ((c4 - 'A') % 32));
            res.length = 2;
        }
        return res;
    }
}