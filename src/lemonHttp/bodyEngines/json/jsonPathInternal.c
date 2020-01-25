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

static const boolean ifItIsLastRule(const jsonPathElement *currElement, const jsonPathElement *lastElement) {
    const jsonPathElement *nextElement = currElement + 1;
    /* if ((lastElement == currElement) || ((nextElement <= lastElement) && (ROOT == nextElement->type))) */
    /*if ((currElement == lastElement) || (ROOT == (++(currElement))->type)) */
    if ((lastElement == currElement) || ((nextElement <= lastElement) && (ROOT == nextElement->type))) {
        return TRUE;
    } else {
        return FALSE;
    }
}

static const boolean ifItIsLastObserverChainOfRule(const jsonPathElement *currElement, const jsonPathElement *lastElement) {
    if ((currElement == lastElement) && (1 == currElement->level)) {
        return TRUE;
    } else {
        const jsonPathElement *nextElement = currElement + 1;
        if ((1 == currElement->level) && (((0 == nextElement->level) && (ROOT != nextElement->type)) || ((1 == nextElement->level) && (ROOT == nextElement->type))) ) {
            return TRUE;
        }
    }
    return FALSE;
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

        while ((NULL != currElement) && (lastElement >= currElement)) {

            nextElement = currElement;
            ++nextElement;

            switch (currElement->type) {
                case ROOT:
                    currRoot = currElement; /* Callback is here */
                    switch (currElement->level) {
                        case 1:
                            ++currElement; /* ROOT is passed, move on */
                            break;
                        case 0:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case NAME:
                    if ((currElement->value.length == key->length) && (0 == STRNCASECMP(currElement->value.data, key->data, key->length))) {
                        /* A key has been found, so mark it*/
                        currElement->level = 1;
                        ++currElement;
                    } else {
                        /* Skip rule */
                        currElement = currRoot->next;
                    }
                    break;
                case ANY:
                    /* An any key has been found, so mark it*/
                    currElement->level = 1;
                    ++currElement;
                    break;
                case ANYINDEX:
                case INDEX:
                case RECURSIVE:
                    /* NOTHING */
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

        while (lastElement >= currElement) {

            nextElement = currElement;
            ++nextElement;

            switch (currElement->type) {
                case ROOT:
                    currRoot = currElement; /* Callback is here */
                    switch (currElement->level) {
                        case 1:
                            ++currElement; /* ROOT is passed, move on */
                            break;
                        case 0:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case NAME:
                    /* Unmark last NAME element, if it is possible */
                    if ((currElement->value.length == key->length) && (0 == STRNCASECMP(currElement->value.data, key->data, key->length) && (TRUE == ifItIsLastObserverChainOfRule(currElement, lastElement)))) {
                        /* A key has been found, so mark it*/
                        currElement->level = 0;
                    } else {
                        /* May be skip ?? */
                    }
                    ++currElement;
                    break;
                case ANY:
                    currElement->level = 0;
                    ++currElement;
                    break;
                case ANYINDEX:
                case INDEX:
                case RECURSIVE:
                    /* NOTHING */
                    break;
                default:
                    return LE_INCORRECT_INPUT_VALUES;
            }
        }
        return LE_OK;
    }
}

const lemonError updateJsonPathRequestStatusByObject(jsonPathRequest *jsonRequest, const char *startObjectPosition) {
    if (NULL == jsonRequest) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /* empty string is not checked ! */
    {
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        jsonPathElement *currElement = currRoot;
        jsonPathElement *nextElement = currRoot;
        boolean isNextElementLast = FALSE;

        while (lastElement >= currElement) {

            nextElement = currElement;
            ++nextElement;

            switch (currElement->type) {
                case ROOT:
                    currRoot = currElement; /* Callback is here */
                    switch (currElement->level) {
                        case 1:
                            ++currElement; /* ROOT is passed, move on */
                            break;
                        case 0:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case ANYINDEX:

                case ANY:
                case NAME:
                    /* The NAME/ANY is last chain of rule and it has been found already, so let's return entire object. */
                    if ((TRUE == ifItIsLastRule(currElement, lastElement)) && (0 != currElement->level)) {
                        /* The object may be complex,
                         * so let's increment level if an inner object would be found and decrease otherwise. */
                        if (1 == currElement->level) {
                            currElement->containerStartPosition = startObjectPosition;
                        }
                        ++(currElement->level);
                    }
                    ++currElement;
                    break;
                case INDEX:
                case RECURSIVE:
                    /* NOTHING */
                    break;
                default:
                    return LE_INCORRECT_INPUT_VALUES;
            }
        }
        return LE_OK;
    }
}

const lemonError rollbackJsonPathRequestStatusByObject(jsonPathRequest *jsonRequest, const char *endObjectPosition) {
    if (NULL == jsonRequest) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /* empty string is not checked ! */
    {
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        jsonPathElement *currElement = currRoot;
        jsonPathElement *nextElement = currRoot;

        while (lastElement >= currElement) {

            nextElement = currElement;
            ++nextElement;

            switch (currElement->type) {
                case ROOT:
                    currRoot = currElement; /* Callback is here */
                    switch (currElement->level) {
                        case 1:
                            ++currElement; /* ROOT is passed, move on */
                            break;
                        case 0:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case ANY:
                case NAME:
                    /* The NAME/ANY is last chain of rule and it has been found already, so let's return entire object. */
                    if ((TRUE == ifItIsLastRule(currElement, lastElement)) && (0 != currElement->level)) {
                        /* The object may be complex,
                         * so let's increment level if an inner object would be found and decrease otherwise. */
                        --(currElement->level);
                        if (1 == currElement->level) {
                            string s;
                            s.data = currElement->containerStartPosition;
                            s.length = endObjectPosition - currElement->value.data;
                            {
                                const lemonError err = (currRoot->callback.handler)(&s, currRoot->callback.data);
                                if (LE_OK != err) {
                                    return err;
                                }
                            }

                        }
                    }
                    ++currElement;
                    break;
                default:
                    return LE_INCORRECT_INPUT_VALUES;
            }
        }
        return LE_OK;
    }
}

const lemonError updateJsonPathRequestStatusByArray(jsonPathRequest *jsonRequest, const char *startArrayPosition) {
    if (NULL == jsonRequest) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /* empty string is not checked ! */
    {
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        jsonPathElement *currElement = currRoot;
        jsonPathElement *nextElement = currRoot;
        boolean isNextElementLast = FALSE;

        while (lastElement >= currElement) {

            nextElement = currElement;
            ++nextElement;

            switch (currElement->type) {
                case ROOT:
                    currRoot = currElement; /* Callback is here */
                    switch (currElement->level) {
                        case 1:
                            ++currElement; /* ROOT is passed, move on */
                            break;
                        case 0:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case ANYINDEX:
                case ANY:
                case NAME:
                    /* Skip all with level > 0 */

                    /* We are waiting NAME but ARRAY has come. */
                    /* Skip rule */
                    currElement = currRoot->next;
                    break;
                case INDEX:
                    break;
                case RECURSIVE:
                    /* NOTHING */
                    break;
                default:
                    return LE_INCORRECT_INPUT_VALUES;
            }
        }
        return LE_OK;
    }
}

const lemonError rollbackJsonPathRequestStatusByArray(jsonPathRequest *jsonRequest, const char *endArrayPosition) {
    if (NULL == jsonRequest) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /* empty string is not checked ! */
    {
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        jsonPathElement *currElement = currRoot;
        jsonPathElement *nextElement = currRoot;

        while (lastElement >= currElement) {

            nextElement = currElement;
            ++nextElement;

            switch (currElement->type) {
                case ROOT:
                case ANY:
                case NAME:
                default:
                    return LE_INCORRECT_INPUT_VALUES;
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

                if (TRUE == ifItIsLastRule(currElement, lastElement)) {
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

const lemonError executeJsonPathCallbackByObject(jsonPathRequest *jsonRequest) {
    if (NULL == jsonRequest) {
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

                /*if ((lastElement == currElement) || ((nextElement <= lastElement) && (ROOT == nextElement->type))) {
                    return (currRoot->callback.handler)(s, currRoot->callback.data);
                } Temp commented. */

                ++currElement;
            } else {
                currElement = currRoot->next;
                currRoot = currElement;
            }
        }
        return LE_OK;
    }
}

const lemonError updateJsonPathRequestStatusByArrayElement(jsonPathRequest *jsonRequest) {
    if (NULL == jsonRequest) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /* empty string is not checked ! */
    {
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        jsonPathElement *currElement = currRoot;
        jsonPathElement *nextElement = currRoot;
        boolean isNextElementLast = FALSE;

        while ((NULL != currElement) && (lastElement >= currElement)) {

            nextElement = currElement;
            ++nextElement;

            switch (currElement->type) {
                case ROOT:
                case ANY:
                case ANYINDEX:
                case NAME:
                case INDEX:
                case RECURSIVE:
                default:
                    return LE_INCORRECT_INPUT_VALUES;
            }
        }
        return LE_OK;
    }
}

const lemonError updateJsonPathRequestStatusByRoot(jsonPathRequest *jsonRequest) {
    if (NULL == jsonRequest) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /* empty string is not checked ! */
    {
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        jsonPathElement *currElement = currRoot;
        jsonPathElement *nextElement = currRoot;
        boolean isNextElementLast = FALSE;

        while ((NULL != currElement) && (lastElement >= currElement)) {

            nextElement = currElement;
            ++nextElement;

            switch (currElement->type) {
                case ROOT:
                    currRoot = currElement; /* Callback is here */
                    switch (currElement->level) {
                        case 0:
                            currElement->level = 1;
                            currElement = currRoot->next;
                            break;
                        case 1:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case ANY:
                case ANYINDEX:
                case NAME:
                case INDEX:
                case RECURSIVE:
                default:
                    return LE_INCORRECT_INPUT_VALUES;
            }
        }
        return LE_OK;
    }
}

const lemonError rollbackJsonPathRequestStatusByRoot(jsonPathRequest *jsonRequest) {
    if (NULL == jsonRequest) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /* empty string is not checked ! */
    {
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        jsonPathElement *currElement = currRoot;
        jsonPathElement *nextElement = currRoot;
        boolean isNextElementLast = FALSE;

        while ((NULL != currElement) && (lastElement >= currElement)) {

            nextElement = currElement;
            ++nextElement;

            switch (currElement->type) {
                case ROOT:
                    currRoot = currElement; /* Callback is here */
                    switch (currElement->level) {
                        case 1:
                            currElement->level = 0;
                            currElement = currRoot->next;
                            break;
                        case 0:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case ANY:
                case ANYINDEX:
                case NAME:
                case INDEX:
                case RECURSIVE:
                default:
                    return LE_INCORRECT_INPUT_VALUES;
            }
        }
        return LE_OK;
    }
}
