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

static const jsonPathElement *getLastUnpassedRule(const jsonPathElement *root, const jsonPathElement *lastElement) {
    if ((NULL == root) || (NULL == lastElement)) {
        /* ERROR ? */
    }
    {
        const jsonPathElement *end = (NULL == root->next) ? lastElement : root->next;
        jsonPathElement *currElement = root;
        while ((0 != currElement->level) && (currElement != end)) {
            ++currElement;
        }
        if (0 == currElement->level) {
            return currElement;
        }
    }
    return NULL;
}

static const jsonPathElement *getLastPassedRule(const jsonPathElement *root, const jsonPathElement *lastElement) {
    if ((NULL == root) || (NULL == lastElement)) {
        /* ERROR ? */
    }
    {
        const jsonPathElement *end = (NULL == root->next) ? lastElement : (root->next) - 1;
        jsonPathElement *currElement = root;
        while ((1 == currElement->level) && (currElement <= end)) {
            ++currElement;
        }
        --currElement;
        return currElement;
    }
    return NULL;
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
        jsonPathElement *unpassed = currRoot;

        while ((NULL != currRoot) && (NULL != (unpassed = getLastUnpassedRule(currRoot, lastElement)))) {
            switch (unpassed->type) {
                case NAME:
                    if ((unpassed->value.length == key->length) && (0 == STRNCASECMP(unpassed->value.data, key->data, key->length))) {
                        /* A key has been found, so mark it*/
                        unpassed->level = 1;
                    }
                    break;
                case ANY:
                    /* An any key has been found, so mark it*/
                    unpassed->level = 1;
                    break;
                default:
                    break;
            }
            currRoot = currRoot->next;
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
        jsonPathElement *passed = currRoot;

        while ((NULL != currRoot) && (NULL != (passed = getLastPassedRule(currRoot, lastElement)))) {
            switch (passed->type) {
                case NAME:
                    if ((passed->value.length == key->length) && (0 == STRNCASECMP(passed->value.data, key->data, key->length))) {
                        /* A key has been found, so mark it*/
                        passed->level = 0;
                    }
                    break;
                case ANY:
                    /* An any key has been found, so mark it*/
                    passed->level = 0;
                    break;
                default:
                    break;
            }
            currRoot = currRoot->next;
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
        jsonPathElement *unpassed = currRoot;

        while ((NULL != currRoot) && (NULL != (unpassed = getLastUnpassedRule(currRoot, lastElement)))) {
            switch (unpassed->type) {
                case ANY:
                case NAME:
                    /* The NAME/ANY is last chain of rule and it has been found already, so let's return entire object. */
                    if ((TRUE == ifItIsLastRule(unpassed, lastElement)) && (0 != unpassed->level)) {
                        /* The object may be complex,
                         * so let's increment level if an inner object would be found and decrease otherwise. */
                        if (1 == unpassed->level) {
                            unpassed->containerStartPosition = startObjectPosition;
                        }
                        ++(unpassed->level);
                    }
                    break;
                default:
                    break;
            }
            currRoot = currRoot->next;
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
        jsonPathElement *passed = currRoot;

        while ((NULL != currRoot) && (NULL != (passed = getLastPassedRule(currRoot, lastElement)))) {
            switch (passed->type) {
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
                    break;
                default:
                    break;
            }
            currRoot = currRoot->next;
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
        jsonPathElement *unpassed = currRoot;

        while ((NULL != currRoot) && (NULL != (unpassed = getLastUnpassedRule(currRoot, lastElement)))) {
            switch (unpassed->type) {
                case ANY:
                case NAME:
                    /* The NAME/ANY is last chain of rule and it has been found already, so let's return entire object. */
                    if ((TRUE == ifItIsLastRule(unpassed, lastElement)) && (0 != unpassed->level)) {
                        /* The object may be complex,
                         * so let's increment level if an inner object would be found and decrease otherwise. */
                        if (1 == unpassed->level) {
                            unpassed->containerStartPosition = startArrayPosition;
                        }
                        ++(unpassed->level);
                    }
                    break;
                default:
                    break;
            }
            currRoot = currRoot->next;
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
        jsonPathElement *passed = currRoot;

        while ((NULL != currRoot) && (NULL != (passed = getLastPassedRule(currRoot, lastElement)))) {
            switch (passed->type) {
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
                            s.length = endArrayPosition - currElement->value.data;
                            {
                                const lemonError err = (currRoot->callback.handler)(&s, currRoot->callback.data);
                                if (LE_OK != err) {
                                    return err;
                                }
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
            currRoot = currRoot->next;
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
        jsonPathElement *passed = currRoot;

        while ((NULL != currRoot) && (NULL != (passed = getLastPassedRule(currRoot, lastElement)))) {
            if (TRUE == ifItIsLastRule(passed, lastElement)) {
                (currRoot->callback.handler)(s, currRoot->callback.data); /* Check return */
            }
            currRoot = currRoot->next;
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
        jsonPathElement *unpassed = currRoot;

        while ((NULL != currRoot) && (NULL != (unpassed = getLastUnpassedRule(currRoot, lastElement)))) {
            switch (unpassed->type) {
                case ROOT:
                    unpassed->level = 1;
                    break;
                default:
                    break;
            }
            currRoot = currRoot->next;
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
        jsonPathElement *passed = currRoot;

        while ((NULL != currRoot) && (NULL != (passed = getLastPassedRule(currRoot, lastElement)))) {
            switch (passed->type) {
                case ROOT:
                    passed->level = 0;
                    break;
                default:
                    break;
            }
            currRoot = currRoot->next;
        }
        return LE_OK;
    }
}
