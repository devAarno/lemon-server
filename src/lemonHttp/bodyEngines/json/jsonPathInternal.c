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
        ((r->elements)[elementNo]).value.data = s->data;
        ((r->elements)[elementNo]).value.length = s->length;
        ((r->elements)[elementNo]).next = NULL;
        ((r->elements)[elementNo]).level = 0;
        ((r->elements)[elementNo]).index = 0;
        ((r->elements)[elementNo]).realRootSize = 0;
        ((r->elements)[elementNo]).containerStartPosition = NULL;
        if (RECURSIVE == ((r->elements)[elementNo - 1]).type) {
            ((r->elements)[elementNo - 1]).next = &((r->elements)[elementNo]);
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

static const boolean ifItIsLastRule2(const jsonPathElement *currElement, const jsonPathElement *currRoot) {
    return &(currRoot[currRoot->realRootSize - 1]) == currElement ? TRUE : FALSE;
}

static const searchResult getLastPassedRule(const jsonPathElement *root, const jsonPathElement *lastElement) {
    if ((NULL == root) || (NULL == lastElement)) {
        /* ERROR ? */
    }
    {
        /* const jsonPathElement *end = (NULL == root->next) ? lastElement : (root->next) - 1;*/
        const jsonPathElement *end = &(root[root->index - 1]);
        searchResult result;
        jsonPathElement *currElement = root;
        jsonPathElement *recursionElement = root;

        result.level = 0;
        while ((0 != currElement->level) && (currElement <= end)) {
            if (RECURSIVE != currElement->type) {
                ++(result.level);
            } else {
                recursionElement = currElement;
                while (recursionElement->type == RECURSIVE) {
                    recursionElement = recursionElement->next;
                    ++(result.level);
                }
                --(result.level);
            }
            ++currElement;
        }
        --currElement;
        --(result.level);
        result.rule = currElement;
        return result;
    }
}

static const jsonPathElement *getElement(const jsonPathElement *currRoot, const size_t parsingLevel) {
    jsonPathElement *result = currRoot;
    jsonPathElement *currentRecursionElement = currRoot;
    size_t ruleLevel = 0; /* ????????????? */

    while (ruleLevel < parsingLevel) {
        switch (result->type) {
            case RECURSIVE:
                if (RECURSIVE != result->next->type) {
                    result = result->next;
                } else {
                    result = result->next;
                    ++ruleLevel;
                }
                break;
            default:
                ++result;
                ++ruleLevel;
                break;
        }
    }
    --result;
    return result;
}

const lemonError updateJsonPathRequestStatusByFieldName(jsonPathRequest *jsonRequest, const string *key) {
    if ((NULL == jsonRequest) || (NULL == key)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /* empty string is not checked ! */
    {
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        jsonPathElement *currElement;

        while (NULL != currRoot) {
            if (currRoot->level <= currRoot->index) {
                currElement = getElement(currRoot, currRoot->level);
                switch (currElement->type) {
                    case NAME:
                        if ((currElement->value.length == key->length) && (0 == STRNCASECMP(currElement->value.data, key->data, key->length))) {
                            /* A key has been found, so mark it*/
                            currElement->level = 1;
                            if (RECURSIVE == (currElement - 1)->type) {
                                (currElement - 1)->level = 1;
                            }
                        } else {
                            if (RECURSIVE == (currElement - 1)->type) {
                                (jsonRequest->elements)[jsonRequest->elementsCount].type = RECURSIVE;
                                (jsonRequest->elements)[jsonRequest->elementsCount].level = 0;
                                (jsonRequest->elements)[jsonRequest->elementsCount].index = 0;
                                (jsonRequest->elements)[jsonRequest->elementsCount].next = (currElement - 1)->next;
                                (currElement - 1)->next = &((jsonRequest->elements)[jsonRequest->elementsCount]);
                                ++(currRoot->index);
                                ++(jsonRequest->elementsCount);
                            }
                        }
                        break;
                    case ANY:
                    case ANYINDEX:
                        /* An any key has been found, so mark it*/
                        currElement->level = 1;
                        currElement->value = *key;
                        break;
                    default:
                        break;
                }
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
        jsonPathElement *currElement;

        while (NULL != currRoot) {
            if (currRoot->level <= currRoot->index) {
                currElement = getElement(currRoot, currRoot->level);
                switch (currElement->type) {
                    case NAME:
                        if ((currElement->value.length == key->length) &&
                            (0 == STRNCASECMP(currElement->value.data, key->data, key->length))) {
                            /* A key has been found, so mark it*/
                            currElement->level = 0;
                        }
                        break;
                    case ANY:
                    case ANYINDEX:
                        /* An any key has been found, so mark it*/
                        if (0 == STRNCASECMP(currElement->value.data, key->data, key->length)) {
                            currElement->level = 0;
                            currElement->value = getEmptyString();
                        }
                        break;
                    default:
                        break;
                }
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
        jsonPathElement *currElement;

        while (NULL != currRoot) {
            if (currRoot->level <= currRoot->index) {
                currElement = getElement(currRoot, currRoot->level);
                switch (currElement->type) {
                    case ANY:
                    case NAME:
                        /* The NAME/ANY is last chain of rule and it has been found already, so let's return entire object. */
                        if ((TRUE == ifItIsLastRule2(currElement, currRoot)) && (0 != currElement->level)) {
                            /* The object may be complex,
                             * so let's increment level if an inner object would be found and decrease otherwise. */
                            if (1 == currElement->level) {
                                currElement->containerStartPosition = startObjectPosition;
                            }
                            ++(currElement->level);
                        }
                        break;
                    case ROOT:
                    case ANYINDEX:
                        currElement->level = 1;
                        currElement->containerStartPosition = startObjectPosition;
                        break;
                    case INDEX:
                        /*passed.rule->index = 0;*/
                        if (currElement->index == atoi(currElement->value.data)) {
                            currElement->level = 1;
                            currElement->containerStartPosition = startObjectPosition;
                        }
                        break;
                    default:
                        break;
                }
            }

            ++(currRoot->level);
            if (currRoot->level <= currRoot->index) {
                /* Let's see forward rule. INDEX, ANY, ANYINDEX may be here. */
                /* Try to resolve zero indexed element */
                ++(currElement);
                switch (currElement->type) {
                    case ANYINDEX:
                    case ANY:
                        currElement->level = 1;
                        break;
                    case INDEX:
                        if (currElement->index == atoi(currElement->value.data)) {
                            currElement->level = 1;
                            currElement->containerStartPosition = startObjectPosition;
                        }
                        break;
                    default:
                        break;
                }
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
        searchResult passed;
        jsonPathElement *currElement;

        while (NULL != currRoot) {
            if (currRoot->level <= currRoot->index) {
                currElement = getElement(currRoot, currRoot->level);
                switch (currElement->type) {
                    case ANY:
                    case ANYINDEX:
                    case INDEX:
                    case NAME: /* Research it */
                        currElement->index = currElement->level = 0;
                        currElement->containerStartPosition = NULL;
                        if (RECURSIVE == (currElement - 1)->type) {
                            (currElement - 1)->level = 0;
                        }
                        break;
                    default:
                        break;
                }
            }

            --(currRoot->level);
            if (currRoot->level <= currRoot->index) {
                currElement = getElement(currRoot, currRoot->level);

                if ((NULL != (passed = getLastPassedRule(currRoot, lastElement)).rule) && (TRUE == ifItIsLastRule2(passed.rule, currRoot)) && (passed.rule == currElement) && (0 != currElement->level)) {
                    string s;
                    s.data = currElement->containerStartPosition;
                    s.length = endObjectPosition - s.data + 1;
                    {
                        const lemonError err = (currRoot->callback.handler)(&s, currRoot->callback.data);
                        if (LE_OK != err) {
                            return err;
                        }
                    }
                }
            }
            currRoot = currRoot->next;
        }
        return LE_OK;
    }
}

const lemonError updateJsonPathRequestStatusByArray(jsonPathRequest *jsonRequest, const char *startArrayPosition) {
    /* May be collapse ? */
    return updateJsonPathRequestStatusByObject(jsonRequest, startArrayPosition);
}

const lemonError rollbackJsonPathRequestStatusByArray(jsonPathRequest *jsonRequest, const char *endArrayPosition) {
    /* May be collapse ? */
    return rollbackJsonPathRequestStatusByObject(jsonRequest, endArrayPosition);
}

const lemonError executeJsonPathCallbackWithValue(jsonPathRequest *jsonRequest, const string *s) {
    if ((NULL == jsonRequest) || (NULL == s)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /* empty string is not checked ! */
    {
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        searchResult passed;
        jsonPathElement *currElement;

        while (NULL != currRoot) {
            if (currRoot->level <= currRoot->index) {
                currElement = getElement(currRoot, currRoot->level);
                switch (currElement->type) {
                    case ANYINDEX:
                        /* Useless but fine */
                        currElement->level = 1;
                        break;
                    case INDEX:
                        if (currElement->index == atoi(currElement->value.data)) {
                            currElement->level = 1;
                        }
                        break;
                    default:
                        break;
                }
            }

            if (NULL != (passed = getLastPassedRule(currRoot, lastElement)).rule) {
                if ((TRUE == ifItIsLastRule2(passed.rule, currRoot)) && (currRoot->level - 1 == passed.level)) {
                    (currRoot->callback.handler)(s, currRoot->callback.data); /* Check return */
                }
            }
            currRoot = currRoot->next;
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
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        jsonPathElement *currElement;

        while (NULL != currRoot) {
            if (currRoot->level <= currRoot->index) {
                currElement = &(currRoot[currRoot->level - 1]);
                switch (currElement->type) {
                    case ANYINDEX:
                        /* Useless but fine */
                        ++(currElement->index);
                        break;
                    case INDEX:
                        if (currElement->index == atoi(currElement->value.data)) {
                            currElement->level = 0;
                        }
                        ++(currElement->index);
                        break;
                    default:
                        break;
                }
            }
            currRoot = currRoot->next;
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
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);

        while (NULL != currRoot) {
            switch (currRoot->type) {
                case ROOT:
                    currRoot->level = 1;
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
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);

        while (NULL != currRoot) {
            switch (currRoot->type) {
                case ROOT:
                    currRoot->level = 0;
                    currRoot->containerStartPosition = NULL;
                    break;
                default:
                    break;
            }
            currRoot = currRoot->next;
        }
        return LE_OK;
    }
}