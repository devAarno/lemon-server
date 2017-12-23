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

requestElement *appendElementOfHttpRequest(httpRequest *r, const string s, const elementType type) {
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
    if ((NULL == r) || (NULL == s.data)) {
        return NULL_IN_INPUT_VALUES;
    }
    if ((0 >= r->elementsCount) || ((0 == s.length) && (emptyString != s.data) && (VALUE != type)) || (0 > s.length)) {
        return INCORRECT_INPUT_VALUES;
    }
    {
        const size_t elementNo = (r->elementsCount)++;
        ((r->elements)[elementNo]).type = type;
        ((r->elements)[elementNo]).value.str.data = s.data;
        ((r->elements)[elementNo]).value.str.length = s.length;
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
