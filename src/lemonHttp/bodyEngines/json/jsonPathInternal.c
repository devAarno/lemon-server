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
        ((r->elements)[elementNo]).containerStartPosition = NULL;
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

static const searchResult getLastUnpassedRule(const jsonPathElement *root, const jsonPathElement *lastElement) {
    if ((NULL == root) || (NULL == lastElement)) {
        /* ERROR ? */
    }
    {
        const jsonPathElement *end = (NULL == root->next) ? lastElement : root->next;
        searchResult result;
        jsonPathElement *currElement = root;

        result.level = 0;
        while ((0 != currElement->level) && (currElement != end)) {
            ++currElement;
            ++(result.level);
        }
        if (0 == currElement->level) {
            result.rule = currElement;
            return result;
        }
    }
    searchResult result;
    result.rule = NULL;
    result.level = 0;
    return result;
}

static const searchResult getLastPassedRule(const jsonPathElement *root, const jsonPathElement *lastElement) {
    if ((NULL == root) || (NULL == lastElement)) {
        /* ERROR ? */
    }
    {
        const jsonPathElement *end = (NULL == root->next) ? lastElement : (root->next) - 1;
        searchResult result;
        jsonPathElement *currElement = root;

        result.level = 0;
        while ((0 != currElement->level) && (currElement <= end)) {
            ++currElement;
            ++(result.level);
        }
        --currElement;
        --(result.level);
        result.rule = currElement;
        return result;
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
        searchResult unpassed;

        while (NULL != currRoot) {
            unpassed = getLastUnpassedRule(currRoot, lastElement);
            if (((NULL != unpassed.rule)) && (currRoot->level - 1 == unpassed.level)) {
                switch (unpassed.rule->type) {
                    case NAME:
                        if ((unpassed.rule->value.length == key->length) && (0 == STRNCASECMP(unpassed.rule->value.data, key->data, key->length))) {
                            /* A key has been found, so mark it*/
                            unpassed.rule->level = 1;
                        }
                        break;
                    case ANY:
                        /* An any key has been found, so mark it*/
                        unpassed.rule->level = 1;
                        unpassed.rule->value = *key;
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
        searchResult passed;

        while ((NULL != currRoot) && (NULL != (passed = getLastPassedRule(currRoot, lastElement)).rule)) {
            if (currRoot->level - 1 == passed.level) {
                switch (passed.rule->type) {
                    case NAME:
                        if ((passed.rule->value.length == key->length) &&
                            (0 == STRNCASECMP(passed.rule->value.data, key->data, key->length))) {
                            /* A key has been found, so mark it*/
                            passed.rule->level = 0;
                        }
                        break;
                    case ANY:
                        /* An any key has been found, so mark it*/
                        if (0 == STRNCASECMP(passed.rule->value.data, key->data, key->length)) {
                            passed.rule->level = 0;
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
        searchResult passed;

        while (NULL != currRoot) {
            if ((NULL != (passed = getLastPassedRule(currRoot, lastElement)).rule) && (currRoot->level - 1 == passed.level)) {
                if ((TRUE == ifItIsLastRule(passed.rule, lastElement)) && (0 != passed.rule->level)) {
                    passed.rule->containerStartPosition = startObjectPosition;
                }
            }
            ++(currRoot->level);
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

        while (NULL != currRoot) {
            /*if (((NULL != (passed = getLastPassedRule(currRoot, lastElement)).rule) && (currRoot->level - 1 == passed.level)) || ((ROOT == passed.rule->type) && (2 == currRoot->level))) {*/
            if ((NULL != (passed = getLastPassedRule(currRoot, lastElement)).rule) && (((currRoot->level - 1 == passed.level) && (NULL == passed.rule->containerStartPosition)) || ((currRoot->level - 2 == passed.level) && (NULL != passed.rule->containerStartPosition)))) {
                if ((TRUE == ifItIsLastRule(passed.rule, lastElement)) && (0 != passed.rule->level)) {
                    string s;
                    s.data = passed.rule->containerStartPosition;
                    s.length = endObjectPosition - s.data + 1;
                    {
                        const lemonError err = (currRoot->callback.handler)(&s, currRoot->callback.data);
                        if (LE_OK != err) {
                            return err;
                        }
                    }
                }
            }
            --(currRoot->level);
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
        searchResult passed;

        while (NULL != currRoot) {
            if ((NULL != (passed = getLastPassedRule(currRoot, lastElement)).rule) && (currRoot->level - 1 == passed.level)) {
                switch (passed.rule->type) {
                    case ANY:
                    case NAME:
                        /* The NAME/ANY is last chain of rule and it has been found already, so let's return entire object. */
                        if ((TRUE == ifItIsLastRule(passed.rule, lastElement)) && (0 != passed.rule->level)) {
                            /* The object may be complex,
                             * so let's increment level if an inner object would be found and decrease otherwise. */
                            if (1 == passed.rule->level) {
                                passed.rule->containerStartPosition = startArrayPosition;
                            }
                            ++(passed.rule->level);
                        }
                        break;
                    case ROOT:
                    case ANYINDEX:
                    case INDEX:
                        /*passed.rule->index = 0;*/
                        passed.rule->containerStartPosition = startArrayPosition;
                        break;
                    default:
                        break;
                }
            }
            ++(currRoot->level);

            if (INDEX == ((passed.rule) + 1)->type) {
                /* passed should become INDEX */
                ++(passed.rule);

                /* `level` here means the position in the JSON path */
                ++(passed.level);

                if (currRoot->level - 1 == passed.level) {
                    if (passed.rule->index == atoi(passed.rule->value.data)) {
                        passed.rule->level = 1;
                    }
                }
            }

            if (ANYINDEX == ((passed.rule) + 1)->type) {
                ++(passed.rule);
                ++(passed.level);

                if (currRoot->level - 1 == passed.level) {
                    passed.rule->level = 1;
                }
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
        searchResult passed;

        /*while ((NULL != currRoot) && (NULL != (passed = getLastPassedRule(currRoot, lastElement)).rule)) {*/
        while (NULL != currRoot) {
            /*if (((NULL != (passed = getLastPassedRule(currRoot, lastElement)).rule) && (currRoot->level - 1 == passed.level)) || ((ROOT == passed.rule->type) && (2 == currRoot->level))) {*/
            if (NULL != (passed = getLastPassedRule(currRoot, lastElement)).rule) {
                if (((currRoot->level - 1 == passed.level) && (NULL == passed.rule->containerStartPosition)) ||
                    ((currRoot->level - 2 == passed.level) && (NULL != passed.rule->containerStartPosition))) {
                    switch (passed.rule->type) {
                        case ROOT:
                        case ANY:
                        case NAME:
                            /* The NAME/ANY is last chain of rule and it has been found already, so let's return entire object. */
                            if ((TRUE == ifItIsLastRule(passed.rule, lastElement)) && (0 != passed.rule->level)) {
                                string s;
                                s.data = passed.rule->containerStartPosition;
                                s.length = endArrayPosition - s.data + 1;
                                {
                                    const lemonError err = (currRoot->callback.handler)(&s, currRoot->callback.data);
                                    if (LE_OK != err) {
                                        return err;
                                    }
                                }
                            }
                            break;
                        case ANYINDEX:
                        case INDEX:
                            /*passed.rule->index = 0;*/
                            if ((TRUE == ifItIsLastRule(passed.rule, lastElement)) && (0 != passed.rule->level) && (NULL != passed.rule->containerStartPosition)) {
                                string s;
                                s.data = passed.rule->containerStartPosition;
                                s.length = endArrayPosition - s.data + 1;
                                {
                                    /* Bug here */
                                    const lemonError err = (currRoot->callback.handler)(&s, currRoot->callback.data);
                                    if (LE_OK != err) {
                                        return err;
                                    }
                                }
                            }
                            passed.rule->level = 0;
                            break;
                        default:
                            break;
                    }
                }

                if (INDEX == ((passed.rule) + 1)->type) {
                    /* passed should become INDEX */
                    ++(passed.rule);

                    /* `level` here means the position in the JSON path */
                    ++(passed.level);

                    if (currRoot->level - 1 == passed.level) {
                        passed.rule->index = 0;
                        passed.rule->level = 0;
                    }
                }

                if (ANYINDEX == ((passed.rule) + 1)->type) {
                    ++(passed.rule);
                    ++(passed.level);

                    if (currRoot->level - 1 == passed.level) {
                        passed.rule->index = 0;
                        passed.rule->level = 0;

                    }
                }
            }
            --(currRoot->level);
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
        searchResult passed;

        while ((NULL != currRoot) && (NULL != (passed = getLastPassedRule(currRoot, lastElement)).rule)) {
            if ((TRUE == ifItIsLastRule(passed.rule, lastElement)) && (currRoot->level - 1 == passed.level)) {
                (currRoot->callback.handler)(s, currRoot->callback.data); /* Check return */
            }
            currRoot = currRoot->next;
        }
        return LE_OK;
    }
}

const lemonError updateJsonPathRequestStatusByZeroArrayElement(jsonPathRequest *jsonRequest) {
    if (NULL == jsonRequest) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /* empty string is not checked ! */
    {
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        searchResult passed;

        while ((NULL != currRoot) && (NULL != (passed = getLastPassedRule(currRoot, lastElement)).rule)) {

            if (INDEX == ((passed.rule) + 1)->type) {
                /* passed should become INDEX */
                ++(passed.rule);

                /* `level` here means the position in the JSON path */
                ++(passed.level);

                if (currRoot->level - 1 == passed.level) {
                    if (passed.rule->index == atoi(passed.rule->value.data)) {
                        passed.rule->level = 1;
                    }
                }
            }

            if (ANYINDEX == ((passed.rule) + 1)->type) {
                ++(passed.rule);
                ++(passed.level);

                if (currRoot->level - 1 == passed.level) {
                    passed.rule->level = 1;
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
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        searchResult passed;

        while ((NULL != currRoot) && (NULL != (passed = getLastPassedRule(currRoot, lastElement)).rule)) {

            switch (passed.rule->type) {
                /*case ANYINDEX:
                    if (currRoot->level - 1 != passed.level) {
                        passed.rule->level = 0;
                    }
                    break;*/
                case INDEX:
                    if (currRoot->level - 1 == passed.level) {
                        if (passed.rule->index == atoi(passed.rule->value.data)) {
                            passed.rule->level = 0;
                        }
                    }
                    break;
                default:
                    break;
            }

            if (INDEX == ((passed.rule) + 1)->type) {
                /* passed should become INDEX */
                ++(passed.rule);

                /* `level` here means the position in the JSON path */
                ++(passed.level);

                if (currRoot->level - 1 == passed.level) {
                    ++(passed.rule->index);
                    if (passed.rule->index == atoi(passed.rule->value.data)) {
                        passed.rule->level = 1;
                    } else {
                        passed.rule->level = 0;
                    }
                }
            }

            if (ANYINDEX == ((passed.rule) + 1)->type) {
                ++(passed.rule);
                ++(passed.level);

                if (currRoot->level - 1 == passed.level) {
                    ++(passed.rule->index);
                    passed.rule->level = 1;
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
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        searchResult unpassed;

        while (NULL != currRoot) {
            unpassed = getLastUnpassedRule(currRoot, lastElement);
            if (NULL != unpassed.rule) {
                switch (unpassed.rule->type) {
                    case ROOT:
                        unpassed.rule->level = 1;
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

const lemonError rollbackJsonPathRequestStatusByRoot(jsonPathRequest *jsonRequest) {
    if (NULL == jsonRequest) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /* empty string is not checked ! */
    {
        const jsonPathElement *lastElement = &((jsonRequest->elements)[jsonRequest->elementsCount - 1]);
        jsonPathElement *currRoot = &((jsonRequest->elements)[0]);
        searchResult passed;

        while ((NULL != currRoot) && (NULL != (passed = getLastPassedRule(currRoot, lastElement)).rule)) {
            switch (passed.rule->type) {
                case ROOT:
                    passed.rule->level = 0;
                    break;
                default:
                    break;
            }
            currRoot = currRoot->next;
        }
        return LE_OK;
    }
}