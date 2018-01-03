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

#include "http_request_int.h"

#include <stddef.h>
#include <string.h>


#include "string.h"
#include "lemonHttpError.h"

static const char *emptyString = "";

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

const lemonHttpError decodeValue(string *s, boolean replacePlusWithSpace) {
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

requestElement *appendElementOfHttpRequest(httpRequest *r, const string *s, const elementType type) {
    /*if ((NULL == r) || (NULL == startPos)) {
        return NULL_IN_INPUT_VALUES;
    }
    if ((0 >= r->elementsCount) || (0 >= len)) {
        return INCORRECT_INPUT_VALUES;
    }*/
    /*if ((NULL == r) || ((NULL == startPos) && (0 != len) && (VALUE != type)) ) {
        return NULL_IN_INPUT_VALUES;
    }
    if ((0 >= r->elementsCount) || ((0 == len) && (NULL != startPos) && (VALUE != type)) || (0 > len)) {
        return INCORRECT_INPUT_VALUES;
    }*/
    if ((NULL == r) || (NULL == s) || (NULL == s->data)) {
        return NULL;
    }
    if ((0 >= r->elementsCount) || ((0 == s->length) && (emptyString != s->data) && (VALUE != type)) || (0 > s->length)) {
        return NULL;
    }
    {
        const size_t elementNo = (r->elementsCount)++;
        ((r->elements)[elementNo]).type = type;
        ((r->elements)[elementNo]).value.str.data = s->data;
        ((r->elements)[elementNo]).value.str.length = s->length;
        ((r->elements)[elementNo]).value.nextVal = NULL;
        return &((r->elements)[elementNo]);
    }
}

const lemonHttpError linkRequestElement(requestElement *key, const requestElement *value) {
    if ((NULL == key) ||
       (NULL == value)) {
        return NULL_IN_INPUT_VALUES;
    }
    {
        linkedDataString *pos = &(key->value);
        while (NULL != pos->nextVal) { pos = pos->nextVal; };
        pos->nextVal = &(value->value.str);
    }
}

const string getEmptyString() {
    string s;
    s.data = emptyString;
    s.length = 0;
    return s;
}

const requestElement *getEmptyValueElement(const httpRequest *r) {
    return &(r->elements[0]);
}

const lemonHttpError trim(string *s) {
    while ((s->data == ' ') || (s->data == '\t')) {
        ++(s->data);
        --(s->length);
    };
    while (((s->data)[s->length - 1] == ' ') || ((s->data)[s->length - 1] == '\t')) {
        --(s->length);
    };
    return 0;
}
