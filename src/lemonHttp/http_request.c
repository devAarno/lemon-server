/*
 * Copyright (C) 2017 Parkhomenko Stanislav
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

#include "http_request.h"

#include <stddef.h>
#include <string.h>

#ifdef USE_STRINGS_H
#include <strings.h>
#endif

#include "string.h"
#include "../boolean.h"
#include "lemonHttpError.h"
#include "http_request_int.h"

/** FreeBSD implementation
 * https://svnweb.freebsd.org/base/head/lib/libc/string/strcasecmp.c?view=markup
 */

#undef STRNCASECMP

#ifdef USE_INTERNAL_STRNCASECMP

#include <ctype.h>

static int strncasecmp_internal(const char *s1, const char *s2, size_t n) {
    if (n != 0) {
        do {
            if (tolower(*s1) != tolower(*s2++))
                return (tolower(*s1) - tolower(*--s2));
            if (*s1++ == '\0')
                break;
        } while (--n != 0);
    }
    return (0);
}
#define STRNCASECMP strncasecmp_internal
#else
#define STRNCASECMP strncasecmp
#endif

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

static const lemonHttpError decodeValue(string *s, boolean replacePlusWithSpace) {
    if ((NULL == s) || (NULL == s->data)) {
        return NULL_IN_INPUT_VALUES;
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
                            return PARSING_IS_FAILED;
                            break;
                        default:
                            switch (b = tbl[(s->data)[readPos + 2]]) {
                                case NA:
                                    return PARSING_IS_FAILED;
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
    return OK;
}

static const lemonHttpError normalizePath(string *s) {
    if ((NULL == s) || (NULL == s->data)) {
        return NULL_IN_INPUT_VALUES;
    }
    {
        const size_t oldSize = s->length;
        size_t readPos = 0;
        size_t writePos = 0;
        /* char a, b; */
        while (readPos < oldSize) {
            switch ((s->data)[readPos]) {
                case '.':
                    switch ((readPos < oldSize - 1) ? ((s->data)[readPos + 1]) : NA) {
                        case '/':
                            readPos += 2;
                            break;
                        case '.':
                            switch ((readPos < oldSize - 2) ? ((s->data)[readPos + 2]) : NA) {
                                case '/':
                                    /* writePos at the end of path now. */
                                    --writePos;
                                    /* writePos discard last '/' if it is not first. */
                                    if (0 != writePos) {
                                        --writePos;
                                    }
                                    while ('/' != ((s->data)[writePos]) && (writePos > 0)) {
                                        --writePos;
                                    }
                                    readPos += 2;
                                    break;
                                case NA:
                                default:
                                    (s->data)[writePos] = tolower((s->data)[readPos]);
                                    ++writePos;
                                    ++readPos;
                                    break;
                            }
                            break;
                        case NA:
                        default:
                            (s->data)[writePos] = tolower((s->data)[readPos]);
                            ++writePos;
                            ++readPos;
                            break;
                    }
                    break;
                default:
                    (s->data)[writePos] = tolower((s->data)[readPos]);
                    ++writePos;
                    ++readPos;
                    break;
            }
        }
        s->length = writePos;
    }
    return OK;
}

const lemonHttpError initHttpRequest(httpRequest *r, const int fd) {
    if (NULL == r) {
        return NULL_IN_INPUT_VALUES;
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

        return OK;
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

const linkedDataString *getHeaderOfHttpRequestAsSplitStrings(const httpRequest *r, const char *name) {
    if ((NULL == r) || (NULL == name) || (0 >= r->elementsCount)) {
        return NULL;
    }
    {
        const size_t nameLength = strlen(name);
        const size_t elementsCount = r->elementsCount;
        size_t i;
        for (i = 0; i < elementsCount; ++i) {
            if ((HEADER == ((r->elements)[i]).type) && (nameLength == ((r->elements)[i]).value.str.length) && (0 == STRNCASECMP(name, ((r->elements)[i]).value.str.data, nameLength))) {
                /* skip HEADER_VALUE_AS_STRING_CHUNK */
                return ((r->elements)[i]).value.nextVal->nextVal;
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
    const string empty = getEmptyString();
    return ((empty.length == s->length) && (empty.data == s->data)) ? TRUE : FALSE;
}
