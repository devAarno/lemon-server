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
#include "../../strncasecmp.h"
#include "../../../boolean.h"

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

const lemonError updateJsonPathRequestStatusByFieldName(jsonPathRequest *jsonRequest, const string *key) {
    if ((NULL == jsonRequest) || (NULL == key)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /* empty string is not checked ! */
    {
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        jsonPathElement *currElement = currRoot;
        jsonPathElement *nextElement = currRoot;
        boolean isNextElementLast = FALSE;

        /*while ((0 != currElement->level) && (ROOT != currElement->type) && (i < jsonRequest->elementsCount)) {
            ++i;
        }*/

        while ((NULL != currElement) && (lastElement >= currElement)) {

            nextElement = currElement;
            ++nextElement;

            switch (currElement->type) {
                case ROOT: /* Not here ??????? */
                    currRoot = currElement; /* Callback is here */
                    switch (currElement->level) {
                        case 0:
                            currElement->level = 1;
                            /*if (lastElement == currElement) {
                                return (currRoot->callback.handler)(key, currRoot->callback.data); * No any another JSONPath'es *
                            } else if ((nextElement <= lastElement) && (ROOT == nextElement->type)) {
                                (currRoot->callback.handler)(key, currRoot->callback.data);
                                ++currElement; * A next element is ROOT of another JSONPath, so use next *
                            } else {
                                ++currElement; * ROOT is passed but it is not last element in a JSONPath, move on *
                            }*/
                            break;
                        case 1:
                            ++currElement; /* ROOT is passed, move on */
                            break;
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case NAME:
                    switch (currElement->level) {
                        case 0:
                            if ((currElement->value.length == key->length) && (0 == STRNCASECMP(currElement->value.data, key->data, key->length))) {
                                currElement->level = 1;
                                /*if (lastElement == currElement) {
                                    return (currRoot->callback.handler)(key, currRoot->callback.data);
                                } else if ((nextElement <= lastElement) && (ROOT == nextElement->type)) {
                                    (currRoot->callback.handler)(key, currRoot->callback.data);
                                    ++currElement; * A next element is ROOT of another JSONPath, so use next *
                                } else {
                                    ++currElement; * NAME is passed but it is not last element in a JSONPath, move on *
                                }*/
                            } else {
                                currElement = currRoot->next;
                            }
                            break;
                        case 1:
                            ++currElement;
                            break;
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                /*case RECURSIVE:
                    switch (currElement->level) {
                        case 0: * Means ANY is not passed and accepts key *
                            nextElement = currElement;
                            ++nextElement;
                            if (lastElement == currElement) {
                                return (currRoot->callback.handler)(key, currRoot->callback.data);
                            } else if (nextElement <= lastElement) {
                                switch (nextElement->type) {
                                    case ROOT:

                                    case NAME:
                                        if ((nextElement->value.length == key->length) && (STRNCASECMP(nextElement->value.data, key->data, key->length))) {
                                            currElement->level = 1;
                                            {
                                                const lemonError status = updateJsonPathRequestStatusByFieldName(jsonRequest, key);
                                                if (LE_OK != status) {
                                                    return status;
                                                }
                                            }
                                        }
                                        break;
                                    case ANY:

                                    default:
                                        return LE_INCORRECT_INPUT_VALUES; * Array may be here! *
                                }
                                ++currElement;
                            } else {
                                currElement = currRoot->next;
                            }
                            break;
                        case 1: * Means ANY is passed *
                            ++currElement;
                            break;
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;*/
                case ANY:
                    switch (currElement->level) {
                        case 0:
                            currElement->level = 1;
                            /*if (lastElement == currElement) {
                                return (currRoot->callback.handler)(key, currRoot->callback.data);
                            } else if ((nextElement <= lastElement) && (ROOT == nextElement->type)) {
                                (currRoot->callback.handler)(key, currRoot->callback.data);
                                ++currElement; * next is ROOT, so use next *
                            } else {
                                ++currElement;
                            }*/
                            break;
                        case 1:
                            ++currElement;
                            break;
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                default:
                    return LE_INCORRECT_INPUT_VALUES;
            }
        }
        return LE_OK;
    }
}

const lemonError rollbackJsonPathRequestStatusByFieldName(jsonPathRequest *jsonRequest, const string *key) {
    if (NULL == jsonRequest) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /* empty string is not checked ! */
    {
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        jsonPathElement *currElement = currRoot;
        jsonPathElement *nextElement = currRoot;
        boolean isNextRuleLast = FALSE;

        while ((NULL != currElement) && (lastElement >= currElement)) {

            nextElement = currElement;
            ++nextElement;

            if ((lastElement == currElement) || ((nextElement <= lastElement) && ((ROOT == nextElement->type) || (0 == nextElement->level)))) {

                /* Here we found a last rule of current JSONPath */

                switch (currElement->type) {
                    case ROOT: /* ????????????? */
                        /* Do nothing */
                        break;
                    case NAME:
                        if ((currElement->value.length == key->length) && (0 == STRNCASECMP(currElement->value.data, key->data, key->length))) {
                            currElement->level = 0;
                        } else {
                            currElement = currRoot->next;
                        }
                        break;
                    case ANY:
                        currElement->level = 0;
                        break;
                    default:
                        return LE_INCORRECT_INPUT_VALUES;
                }
                currElement = currRoot->next;
                currRoot = currElement;
            } else {
                ++currElement;
            }
        }
        return LE_OK;
    }
}

const lemonError executeJsonPathCallbackWithValue(jsonPathRequest *jsonRequest, const string *s) {
    if ((NULL == jsonRequest) || (NULL == s)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /* empty string is not checked ! */
    {
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        jsonPathElement *currElement = currRoot;
        jsonPathElement *nextElement = currRoot;

        while ((NULL != currElement) && (lastElement >= currElement)) {

            if (0 != currElement->level) {

                nextElement = currElement;
                ++nextElement;

                if ((lastElement == currElement) || ((nextElement <= lastElement) && (ROOT == nextElement->type))) {
                    return (currRoot->callback.handler)(s, currRoot->callback.data);
                }

                ++currElement;
            } else {
                currElement = currRoot->next;
                currRoot = currElement;
            }
        }
        return LE_OK;
    }
}
