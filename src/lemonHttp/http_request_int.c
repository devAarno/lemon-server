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

const lemonHttpError appendElementOfHttpRequest(httpRequest *r, const char *startPos, const size_t len, const elementType type) {
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
    if ((NULL == r) || (NULL == startPos)) {
        return NULL_IN_INPUT_VALUES;
    }
    if ((0 >= r->elementsCount) || ((0 == len) && (emptyString != startPos) && (VALUE != type)) || (0 > len)) {
        return INCORRECT_INPUT_VALUES;
    }
    {
        const size_t elementNo = (r->elementsCount)++;
        ((r->elements)[elementNo]).type = type;
        ((r->elements)[elementNo]).value.str.data = (char *) startPos;
        ((r->elements)[elementNo]).value.str.length = len;
        ((r->elements)[elementNo]).value.nextVal = NULL;
        return OK;
    }
}

const char *getEmptyString() {
    return emptyString;
}
