/*
 * Copyright (C) 2017, 2018, 2019, 2020, 2021 Parkhomenko Stanislav
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

#include <stdlib.h>
#include <stdio.h>
#include "jsonPathInternal.h"

#include "../../string.h"
#include "../../strncasecmp.h"
#include "../../../boolean.h"

typedef struct {
    jsonPathElement *rule;
    size_t level;
} searchResult;

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

        switch (type) {
            case NAME:
                ((r->elements)[elementNo]).data.name.data = s->data;
                ((r->elements)[elementNo]).data.name.length = s->length;
                break;
            case INDEX:
                /* Not C89 */
                ((r->elements)[elementNo]).data.index.containerStartPosition = NULL;
                sscanf(s->data, "%zu", &(((r->elements)[elementNo]).data.index.index));
                break;
            default:
                break;
        }
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

static size_t isJsonPathResolved(jsonPathElement *currRule, jsonPathElement *currStack) {
    size_t r = 0;
    while ((ROOT != currRule->type) || (PARSED_ROOT != currStack->type)) {
        switch (currRule->type) {
            case ROOT:
                switch (currStack->type) {
                    case PARSED_OBJECT:
                        --(currStack);
                        break;
                    default:
                        return 0;
                }
                break;
            case ANY:
                switch (currStack->type) {
                    case PARSED_OBJECT:
                        --(currStack);
                        break;
                    case PARSED_INDEX:
                    case PARSED_FIELD:
                        --(currRule);
                        --(currStack);
                        break;
                    case PARSED_ROOT:
                        return 0;
                    default:
                        break;
                }
                break;
            case NAME:
                switch (currStack->type) {
                    case PARSED_FIELD:
                        if (
                                (currRule->data.name.length == currStack->data.name.length) &&
                                (0 == STRNCASECMP(currStack->data.name.data, currRule->data.name.data,
                                                  currStack->data.name.length))
                                ) {
                            --(currRule);
                            --(currStack);
                        } else {
                            return 0;
                        }
                        break;
                    case PARSED_INDEX:
                    case PARSED_ROOT:
                        return 0;
                    case PARSED_OBJECT:
                        --(currStack);
                        break;
                    default:
                        /* ERROR */
                        break;
                }
                break;
            case INDEX:
                switch (currStack->type) {
                    case PARSED_OBJECT:
                        --(currStack);
                        break;
                    case PARSED_INDEX:
                        if (currRule->data.index.index == currStack->data.index.index) {
                            --(currRule);
                            --(currStack);
                        } else {
                            return 0;
                        }
                        break;
                    default:
                        return 0;
                }
                break;
            case ANYINDEX:
                switch (currStack->type) {
                    case PARSED_OBJECT:
                        --(currStack);
                        break;
                    case PARSED_INDEX:
                    case PARSED_FIELD:
                        --(currRule);
                        --(currStack);
                        break;
                    default:
                        return 0;
                }
                break;
            case RECURSIVE:
                --(currRule);
                switch (currRule->type) {
                    case ANY:
                        break;
                    case ANYINDEX:
                        break;
                    case NAME:
                        while (
                                (PARSED_ROOT != currStack->type) &&
                                !(
                                        (PARSED_FIELD == currStack->type) &&
                                        (currRule->data.name.length == currStack->data.name.length) &&
                                        (0 == STRNCASECMP(currStack->data.name.data, currRule->data.name.data,
                                                          currStack->data.name.length))
                                        )
                                ) {
                            --(currStack);
                        }
                        break;
                    case INDEX:
                        break;
                    case ROOT:
                        while (PARSED_ROOT != currStack->type) {
                            --(currStack);
                        }
                        break;
                    default:
                        /* ERROR */
                        break;
                }
                break;
            default:
                /* ERROR */
                break;
        }
    }

    return ((ROOT == currRule->type) && (PARSED_ROOT == currStack->type)) ? 1 : 0;
}

const lemonError updateJsonPathRequestStatusByFieldName(jsonPathRequest *jsonRequest, const string *key) {
    jsonPathElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + jsonRequest->parsedStackSize]);
    currentElement->type = PARSED_FIELD;
    currentElement->data.name.data = key->data;
    currentElement->data.name.length = key->length;
    ++(jsonRequest->parsedStackSize);
    return LE_OK;
}

const lemonError rollbackJsonPathRequestStatusByFieldName(jsonPathRequest *jsonRequest, const string *key) {
    jsonPathElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + (--(jsonRequest->parsedStackSize))]);
    currentElement->type = NONE;
    return LE_OK;
}

const lemonError updateJsonPathRequestStatusByObject(jsonPathRequest *jsonRequest, const char *startObjectPosition) {
    jsonPathElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + jsonRequest->parsedStackSize]);
    currentElement->type = PARSED_OBJECT;
    currentElement->data.containerStartPosition = startObjectPosition;
    ++(jsonRequest->parsedStackSize);
    return LE_OK;
}

const lemonError rollbackJsonPathRequestStatusByObject(jsonPathRequest *jsonRequest, const char *endObjectPosition) {
    jsonPathElement *currElement = jsonRequest->elements;
    while ((ROOT == currElement->type) && (PARSED_ROOT != (1 + currElement)->type)) {
        const rootRule *currRoot = &(currElement->data.root);
        const jsonPathElement *currRule = &(currElement[currRoot->ruleSize - 1]);
        const jsonPathElement *currStack = &((jsonRequest->elements)[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);

        /* Many rules while */
        if ((RECURSIVE == currRule->type) && (PARSED_OBJECT == currStack->type) && (1 == isJsonPathResolved(currRule, currStack))) {
            string s;
            s.data = currStack->data.containerStartPosition;
            s.length = endObjectPosition - s.data + 1;
            const lemonError err = (currRoot->callback.handler)(&s, currRoot->callback.data);
            if (LE_OK != err) {
                return err;
            }
        }
        currElement = 1 + currRule;
    }

    (&(jsonRequest->elements[jsonRequest->elementsCount + (--(jsonRequest->parsedStackSize))]))->type = NONE;
    return LE_OK;
}

const lemonError updateJsonPathRequestStatusByArray(jsonPathRequest *jsonRequest, const char *startArrayPosition) {
    /* May be collapse ? */
    jsonPathElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + jsonRequest->parsedStackSize]);
    currentElement->type = PARSED_INDEX;
    currentElement->data.index.containerStartPosition = startArrayPosition;
    currentElement->data.index.index = 0;
    ++(jsonRequest->parsedStackSize);
    return LE_OK;
}

const lemonError rollbackJsonPathRequestStatusByArray(jsonPathRequest *jsonRequest, const char *endArrayPosition) {
    /* May be collapse ? */
    return rollbackJsonPathRequestStatusByObject(jsonRequest, endArrayPosition);
}

const lemonError executeJsonPathCallbackWithValue(jsonPathRequest *jsonRequest, const string *s) {
    jsonPathElement *currElement = jsonRequest->elements;
    while (ROOT == currElement->type) { /* Be carefull */
        const rootRule *currRoot = &(currElement->data.root);
        const jsonPathElement *currRule = &(currElement[currRoot->ruleSize - 1]);
        const jsonPathElement *currStack = &((jsonRequest->elements)[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);

        switch (currRule->type) {
            case NAME:
            case ANY:
            case INDEX:
            case ANYINDEX:
                switch (currStack->type) {
                    case PARSED_FIELD:
                    case PARSED_INDEX:
                        if (1 == isJsonPathResolved(currRule, currStack)) {
                            const lemonError err = (currRoot->callback.handler)(s, currRoot->callback.data);
                            if (LE_OK != err) {
                                return err;
                            }
                        }
                        break;
                    default:
                        break;
                }
                break;
            case ROOT: {
                    const lemonError err = (currRoot->callback.handler)(s, currRoot->callback.data);
                    if (LE_OK != err) {
                        return err;
                    }
                }
                break;
            default:
                break;
        }
        currElement = 1 + currRule;
    }

    return LE_OK;
}

const lemonError updateJsonPathRequestStatusByArrayElement(jsonPathRequest *jsonRequest) {
    jsonPathElement *currStack = &((jsonRequest->elements)[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);
    if (PARSED_INDEX == currStack->type) {
        ++(currStack->data.index.index);
    }
    return LE_OK;
}

const lemonError updateJsonPathRequestStatusByRoot(jsonPathRequest *jsonRequest) {
    jsonPathElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + jsonRequest->parsedStackSize]);
    currentElement->type = PARSED_ROOT;
    ++(jsonRequest->parsedStackSize);
    return LE_OK;
}

const lemonError rollbackJsonPathRequestStatusByRoot(jsonPathRequest *jsonRequest) {
    jsonPathElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + (--(jsonRequest->parsedStackSize))]);
    currentElement->type = NONE;
    return LE_OK;
}