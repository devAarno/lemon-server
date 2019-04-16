/*
 * Copyright (C) 2017, 2018, 2019 Parkhomenko Stanislav
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

#include "jsonPathInternal.h"

#include "../../string.h"

jsonPathElement *appendJsonPathElementOfHttpRequest(jsonPathRequest *r, const string *s, const ruleType type) {
    const string emptyString = getEmptyString();
    if ((NULL == r) || (NULL == s) || (NULL == s->data) ||
        (0 > r->elementsCount) ||
        (((emptyString.length == s->length) && (emptyString.data != s->data)) || ((emptyString.length != s->length) && (emptyString.data == s->data))) ||
        ((emptyString.length == s->length) && (emptyString.data == s->data) && (ROOT != type) && (ANY != type) && (ANYINDEX != type) && (RECURSIVE != type)) ||
        (emptyString.length > s->length)) {
        return NULL;
    }
    {
        const size_t elementNo = (r->elementsCount)++;
        ((r->elements)[elementNo]).type = type;
        ((r->elements)[elementNo]).value.data = s->data;
        ((r->elements)[elementNo]).value.length = s->length;
        return &((r->elements)[elementNo]);
    }
}

const string convertUtf16ToString(char *c1, char *c2, const char c3, const char c4) {
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
