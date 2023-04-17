/*
 * Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022, 2023 Parkhomenko Stanislav
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

#include "string.h"
#include "strncasecmp.h"
#include "../boolean.h"
#include "rules.h"

lemonError appendJsonPathElementOfHttpRequest(httpRequest *r, const string *s, const ruleType type) {
    const string emptyString = getEmptyString();
    if ((NULL == r) || (NULL == s) || (NULL == s->data) ||
        (0 > r->elementsCount) ||
        (((emptyString.length == s->length) && (emptyString.data != s->data)) || ((emptyString.length != s->length) && (emptyString.data == s->data))) ||
        ((emptyString.length == s->length) && (emptyString.data == s->data) && (JSONPATH_REQUEST_ROOT != type) && (JSONPATH_REQUEST_ANY != type) && (JSONPATH_REQUEST_ANYINDEX != type) && (JSONPATH_REQUEST_RECURSIVE != type)) ||
        (emptyString.length > s->length)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        const size_t elementNo = (r->elementsCount)++;
        ((r->elements)[elementNo]).type = type;

        switch (type) {
            case JSONPATH_REQUEST_NAME:
                ((r->elements)[elementNo]).data.name.data = s->data;
                ((r->elements)[elementNo]).data.name.length = s->length;
                break;
            case JSONPATH_REQUEST_INDEX:
                /* Not C89 */
                ((r->elements)[elementNo]).data.index.containerStartPosition = NULL;
                sscanf(s->data, "%zu", &(((r->elements)[elementNo]).data.index.index));
                break;
            case JSONPATH_REQUEST_ROOT:
            case JSONPATH_REQUEST_ANY:
            case JSONPATH_REQUEST_ANYINDEX:
            case JSONPATH_REQUEST_RECURSIVE:
                break;
            default:
                return LE_INCORRECT_INPUT_VALUES;
        }
        return LE_OK;
    }
}

/* static lemonError printStack(jsonPathElement *lastStack, const char *name) {
    printf("OOOUUUTTT STACK %s <<< ", name);

    if ((lastStack -> type == PARSED_JSON_FIELD_WITH_OBJECT) || (lastStack -> type == PARSED_JSON_FIELD) || (lastStack -> type == PARSED_JSON_RESOLVED_FIELD)) {
        printf(" %d(%.*s) ", lastStack -> type, lastStack->data.name.length, lastStack->data.name.data);
    } else {
        printf(" %d ", lastStack -> type);
    }
    while ((--lastStack)->type != PARSED_JSON_ROOT ) {
        if ((lastStack -> type == PARSED_JSON_FIELD_WITH_OBJECT) || (lastStack -> type == PARSED_JSON_FIELD) || (lastStack -> type == PARSED_JSON_RESOLVED_FIELD)) {
            printf(" %d(%.*s) ", lastStack -> type, lastStack->data.name.length, lastStack->data.name.data);
        } else {
            printf(" %d ", lastStack -> type);
        }
    }
    if ((lastStack -> type == PARSED_JSON_FIELD_WITH_OBJECT) || (lastStack -> type == PARSED_JSON_FIELD) || (lastStack -> type == PARSED_JSON_RESOLVED_FIELD)) {
        printf(" %d(%.*s) ", lastStack -> type, lastStack->data.name.length, lastStack->data.name.data);
    } else {
        printf(" %d ", lastStack -> type);
    }
    puts("");
    return LE_OK;
} */

static lemonError isJsonPathResolved(const requestElement *currRoot, const requestElement *lastRule, const requestElement *lastStack, const string *s, requestElement *currRule, requestElement *currStack, const boolean isComplex) {

    /* Ugly hack, but test34 works */
    /*if ((JSONPATH_REQUEST_RECURSIVE == currRule->type) && (currStack > lastStack) && (TRUE == isComplex)) {
        return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
    }*/

    while ((currRule <= lastRule) && (currStack <= lastStack)) {
        switch (currRule->type) {
            case JSONPATH_REQUEST_ROOT:
                switch (currStack->type) {
                    case PARSED_JSON_ROOT:
                        if ((currRule == lastRule) && (currStack == lastStack)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currRule) <= lastRule) {
                            ++(currRule);
                        } else {
                            /* DO NOT KNOW */
                            printf("RETURN LE_OK 1 %.*s\r\n", s->length, s->data);
                            return LE_OK;
                        }

                        if ((1 + currStack) <= lastStack) {
                            ++(currStack);
                        } else {
                            switch (currRule->type) {
                                case JSONPATH_REQUEST_RECURSIVE:
                                    if ((TRUE == isComplex) && (currRule == lastRule)) {
                                        printf("RETURN DATA 2 %.*s\r\n", s->length, s->data);
                                        puts("CALL 1");
                                        return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                                    }
                                    break;
                                default:
                                    printf("RETURN LE_OK 3 %.*s\r\n", s->length, s->data);
                                    return LE_OK;
                            }
                        }
                        break;
                    default:
                        printf("RETURN LE_OK 4 %.*s\r\n", s->length, s->data);
                        return LE_OK;
                }
                break;
            case JSONPATH_REQUEST_ANY:
                /* printStack(lastStack, "INIT STATE JSONPATH_REQUEST_ANY"); */
                switch (currStack->type) {
                    case PARSED_JSON_OBJECT:
                    case PARSED_JSON_JOINED_OBJECT:
                    case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if (1 + currStack <= lastStack) {
                            ++(currStack);
                        } else {
                            return LE_OK; /* ????????????? */
                        }

                        break;
                    case PARSED_JSON_INDEX:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currRule == lastRule) && (JSONPATH_REQUEST_RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) && (TRUE == isComplex)) {
                            printf("RETURN DATA 2xxx %.*s\r\n", s->length, s->data);
                            puts("CALL 1xxx");
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currStack <= lastStack) && (1 + currRule <= lastRule)) {
                            currStack->type = PARSED_JSON_RESOLVED_FIELD;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex);
                            currStack->type = PARSED_JSON_INDEX;
                            return LE_OK;
                        } else {
                            return LE_OK;
                        }
                        break;
                    case PARSED_JSON_FIELD_WITH_OBJECT:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currRule == lastRule) && (JSONPATH_REQUEST_RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) && (TRUE == isComplex)) {
                            printf("RETURN DATA 2xx %.*s\r\n", s->length, s->data);
                            puts("CALL 1xx");
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currStack <= lastStack) && (1 + currRule <= lastRule)) {
                            currStack->type = PARSED_JSON_RESOLVED_FIELD;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex);
                            currStack->type = PARSED_JSON_FIELD_WITH_OBJECT;
                            return LE_OK;
                        } else {
                            return LE_OK;
                        }

                        break;
                    case PARSED_JSON_FIELD:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currRule == lastRule) && (JSONPATH_REQUEST_RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) && (TRUE == isComplex)) {
                            printf("RETURN DATA 2x %.*s\r\n", s->length, s->data);
                            puts("CALL 1x");
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currStack <= lastStack) && (1 + currRule <= lastRule)) {
                            currStack->type = PARSED_JSON_RESOLVED_FIELD;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex);
                            currStack->type = PARSED_JSON_FIELD;
                            return LE_OK;
                        } else {
                            return LE_OK;
                        }
                        break;
                    default:
                        printf("RETURN LE_OK 5 %.*s\r\n", s->length, s->data);
                        return LE_OK;
                }
                break;
            case JSONPATH_REQUEST_NAME:
                /* printStack(lastStack, "INIT STATE JSONPATH_REQUEST_NAME X2"); */
                switch (currStack->type) {
                    case PARSED_JSON_FIELD:
                        if (
                                (currRule->data.name.length == currStack->data.name.length) &&
                                (0 == STRNCASECMP(currStack->data.name.data, currRule->data.name.data,
                                                  currStack->data.name.length))
                                ) {
                            if ((lastStack == currStack) && (lastRule == currRule)) {
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            }

                            if ((1 + currRule == lastRule) && (JSONPATH_REQUEST_RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) /*&& (TRUE == isComplex)*/) {
                                printf("RETURN DATA 2xxxx %.*s\r\n", s->length, s->data);
                                puts("CALL 1xxxb");
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            }

                            /* JSONPATH_REQUEST_NAME can not resolve further stack itself */
                            if ((lastStack != currStack) && (lastRule == currRule)) {
                                return LE_OK;
                            }

                            /* Not all rules have been resolved */
                            if ((lastStack == currStack) && (lastRule != currRule)) {
                                return LE_OK;
                            }

                            if (((1 + currRule) <= lastRule) && ((1 + currStack) <= lastStack)) {
                                currStack->type = PARSED_JSON_RESOLVED_FIELD;
                                isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex);
                                currStack->type = PARSED_JSON_FIELD;
                                return LE_OK;
                            } else {
                                return LE_OK;
                            }
                        } else {
                            printf("RETURN LE_OK 10 %.*s\r\n", s->length, s->data);
                            return LE_OK;
                        }
                        break;
                    case PARSED_JSON_FIELD_WITH_OBJECT:
                        if (
                                (currRule->data.name.length == currStack->data.name.length) &&
                                (0 == STRNCASECMP(currStack->data.name.data, currRule->data.name.data,
                                                  currStack->data.name.length))
                                ) {

                            if ((lastStack == currStack) && (lastRule == currRule)) {
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            }

                            if ((1 + currRule == lastRule) && (JSONPATH_REQUEST_RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) /*&& (TRUE == isComplex)*/) {
                                printf("RETURN DATA 2xxxx %.*s\r\n", s->length, s->data);
                                puts("CALL 1xxxc");
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            }

                            /* JSONPATH_REQUEST_NAME can not resolve further stack itself */
                            if ((lastStack != currStack) && (lastRule == currRule)) {
                                return LE_OK;
                            }

                            /* Not all rules have been resolved */
                            if ((lastStack == currStack) && (lastRule != currRule)) {
                                return LE_OK;
                            }

                            if (((1 + currRule) <= lastRule) && ((1 + currStack) <= lastStack)) {
                                currStack->type = PARSED_JSON_RESOLVED_FIELD;
                                isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex);
                                currStack->type = PARSED_JSON_FIELD_WITH_OBJECT;
                                return LE_OK;
                            }
                        } else {
                            printf("RETURN LE_OK 15 %.*s\r\n", s->length, s->data);
                            return LE_OK;
                        }
                        break;
                    case PARSED_JSON_INDEX:
                        printf("RETURN LE_OK 16 %.*s\r\n", s->length, s->data);
                        return LE_OK;
                    case PARSED_JSON_OBJECT:
                    case PARSED_JSON_JOINED_OBJECT:
                    case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        ++(currStack);
                        break;
                    default:
                        /* ERROR */
                        printf("RETURN LE_OK 17 %.*s\r\n", s->length, s->data);
                        return LE_OK;;
                }
                break;
            case JSONPATH_REQUEST_INDEX:
                /* printStack(lastStack, "INIT STATE JSONPATH_REQUEST_INDEX"); */
                switch (currStack->type) {
                    case PARSED_JSON_OBJECT:
                    case PARSED_JSON_JOINED_OBJECT:
                    case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        ++(currStack);
                        break;
                    case PARSED_JSON_INDEX:

                        if (currRule->data.index.index == currStack->data.index.index) {

                            if ((lastStack == currStack) && (lastRule == currRule)) {
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            }

                            if (1 + currRule <= lastRule) {
                                ++(currRule);
                            } else {
                                return LE_OK;
                            }

                            if (1 + currStack <= lastStack) {
                                ++(currStack);
                            } else {
                                return LE_OK; /* ????????????? */
                            }

                        } else {
                            printf("RETURN LE_OK 18 %.*s\r\n", s->length, s->data);
                            return LE_OK;
                        }
                        break;
                    default:
                        printf("RETURN LE_OK 19 %.*s\r\n", s->length, s->data);
                        return LE_OK;
                }
                break;
            case JSONPATH_REQUEST_ANYINDEX:
                /* printStack(lastStack, "INIT STATE JSONPATH_REQUEST_ANYINDEX"); */
                switch (currStack->type) {
                    case PARSED_JSON_OBJECT:
                    case PARSED_JSON_JOINED_OBJECT:
                    case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if (1 + currStack <= lastStack) {
                            ++(currStack);
                        } else {
                            return LE_OK; /* ????????????? */
                        }

                        break;
                    case PARSED_JSON_INDEX:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currRule == lastRule) && (JSONPATH_REQUEST_RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) && (TRUE == isComplex)) {
                            printf("RETURN DATA 2xxx %.*s\r\n", s->length, s->data);
                            puts("CALL 1xxx");
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currStack <= lastStack) && (1 + currRule <= lastRule)) {
                            currStack->type = PARSED_JSON_RESOLVED_FIELD;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex);
                            currStack->type = PARSED_JSON_INDEX;
                            return LE_OK;
                        } else {
                            return LE_OK;
                        }
                        break;
                    case PARSED_JSON_FIELD_WITH_OBJECT:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currRule == lastRule) && (JSONPATH_REQUEST_RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) && (TRUE == isComplex)) {
                            printf("RETURN DATA 2xx %.*s\r\n", s->length, s->data);
                            puts("CALL 1xx");
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currStack <= lastStack) && (1 + currRule <= lastRule)) {
                            currStack->type = PARSED_JSON_RESOLVED_FIELD;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex);
                            currStack->type = PARSED_JSON_FIELD_WITH_OBJECT;
                            return LE_OK;
                        } else {
                            return LE_OK;
                        }

                        break;
                    case PARSED_JSON_FIELD:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currRule == lastRule) && (JSONPATH_REQUEST_RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) && (TRUE == isComplex)) {
                            printf("RETURN DATA 2x %.*s\r\n", s->length, s->data);
                            puts("CALL 1x");
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currStack <= lastStack) && (1 + currRule <= lastRule)) {
                            currStack->type = PARSED_JSON_RESOLVED_FIELD;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex);
                            currStack->type = PARSED_JSON_FIELD;
                            return LE_OK;
                        } else {
                            return LE_OK;
                        }
                        break;
                    default:
                        printf("RETURN LE_OK 20 %.*s\r\n", s->length, s->data);
                        return LE_OK;
                }
                break;
            case JSONPATH_REQUEST_RECURSIVE: /* currRule */
                switch (currStack->type) {
                    case PARSED_JSON_OBJECT:
                        if (currRule == lastRule) {
                            currStack->type = PARSED_JSON_HEAD_OF_JOINED_OBJECT;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule, currStack, isComplex);
                            currStack->type = PARSED_JSON_OBJECT;
                            printf("RETURN LE_OK 25 %.*s\r\n", s->length, s->data);
                            return LE_OK; /* by test 1  ??????????????????????????????????????????????? Can recursive be empty? */
                        } else {
                            currStack->type = PARSED_JSON_JOINED_OBJECT;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule, currStack, isComplex);
                            currStack->type = PARSED_JSON_HEAD_OF_JOINED_OBJECT;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule, currStack, isComplex);
                            currStack->type = PARSED_JSON_OBJECT;
                            printf("RETURN LE_OK 25 %.*s\r\n", s->length, s->data);
                            return LE_OK; /* by test 1  ??????????????????????????????????????????????? Can recursive be empty? */
                        }
                        return LE_OK;
                    case PARSED_JSON_JOINED_OBJECT:
                        if (lastStack == currStack) {
                            printf("ERROR STATE 1 %.*s\r\n", s->length, s->data);
                            /* printStack(lastStack, "ERROR STATE 1"); */
                            return LE_OK;
                        } else {
                            ++(currStack);
                        }
                        break;
                    case PARSED_JSON_HEAD_OF_JOINED_OBJECT: /* currStack */
                        if (lastRule == currRule) {
                            if (TRUE == isComplex) {
                                puts("CALL 6");
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            } else {
                                printf("ERROR STATE 2 %.*s\r\n", s->length, s->data);
                                /* printStack(lastStack, "ERROR STATE 2"); */
                                return LE_OK;
                            }
                        } else {
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex);
                            currStack->type = PARSED_JSON_OBJECT;
                            printf("RETURN LE_OK 28 %.*s\r\n", s->length, s->data);
                            return LE_OK; /* May be break ??? */
                        }
                        break;
                    case PARSED_JSON_FIELD:
                        return LE_OK;
                        break;
                    case PARSED_JSON_INDEX:
                        if ((currStack == lastStack) && (lastRule == currRule)) {
                            if (TRUE == isComplex) {
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            } else {
                                return LE_OK;
                            }

                            /* return LE_OK; */
                        } else {
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule, currStack + 1, isComplex);
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack, isComplex);
                            return LE_OK;
                        }
                        /*return LE_OK;*/
                        break;
                    case PARSED_JSON_FIELD_WITH_OBJECT:
                        if (currStack == lastStack) {
                            return LE_OK;
                        } else {
                            ++(currStack);
                        }
                        break;
                    case JSONPATH_REQUEST_ROOT:
                    case NONE:
                        printf("RETURN LE_OK 41 %.*s\r\n", s->length, s->data);
                        return LE_OK;
                    default:
                        printf("RETURN LE_OK 42 %.*s\r\n", s->length, s->data);
                        return LE_OK;
                }
                break;
            default:
                printf("RETURN LE_OK 43 %.*s\r\n", s->length, s->data);
                return LE_OK;
        }
    }

    if ((currRule > lastRule) || (currStack > lastStack)) {
        return LE_OK;
    }
    return LE_OK;
}


lemonError updateJsonPathRequestStatusByFieldName(httpRequest *jsonRequest, const string *key) {
    requestElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + jsonRequest->parsedStackSize]);
    currentElement->type = PARSED_JSON_FIELD;
    currentElement->data.name.data = key->data;
    currentElement->data.name.length = key->length;
    ++(jsonRequest->parsedStackSize);
    return LE_OK;
}

lemonError rollbackJsonPathRequestStatusByFieldName(httpRequest *jsonRequest, const string *key) {
    requestElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + (--(jsonRequest->parsedStackSize))]);
    currentElement->type = NONE;
    return LE_OK;
}

lemonError updateJsonPathRequestStatusByObject(httpRequest *jsonRequest, const char *startObjectPosition) {
    requestElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + jsonRequest->parsedStackSize]);
    currentElement->type = PARSED_JSON_OBJECT;
    currentElement->data.containerStartPosition = startObjectPosition;
    ++(jsonRequest->parsedStackSize);
    return LE_OK;
}

lemonError rollbackJsonPathRequestStatusByObject(httpRequest *jsonRequest, const char *endObjectPosition) {
    (&(jsonRequest->elements[jsonRequest->elementsCount + (--(jsonRequest->parsedStackSize))]))->type = NONE;
    return LE_OK;
}

lemonError updateJsonPathRequestStatusByArray(httpRequest *jsonRequest, const char *startArrayPosition) {
    /* May be collapse ? */
    requestElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + jsonRequest->parsedStackSize]);
    currentElement->type = PARSED_JSON_INDEX;
    currentElement->data.index.containerStartPosition = startArrayPosition;
    currentElement->data.index.index = 0;
    ++(jsonRequest->parsedStackSize);
    return LE_OK;
}

lemonError rollbackJsonPathRequestStatusByArray(httpRequest *jsonRequest, const char *endArrayPosition) {
    /* May be collapse ? */
    return rollbackJsonPathRequestStatusByObject(jsonRequest, endArrayPosition);
}

lemonError executeJsonPathCallbackWithValue(httpRequest *jsonRequest, const string *s, const boolean isComplex) {
    requestElement *currElement = jsonRequest->elements;

    const requestElement *currStack = &((jsonRequest->elements)[jsonRequest->elementsCount]);
    requestElement *lastStack = &((jsonRequest->elements)[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);

    if ((TRUE == isComplex) && (PARSED_JSON_FIELD == lastStack->type)) {
        lastStack->type = PARSED_JSON_FIELD_WITH_OBJECT;
    }

    while (lastStack != currStack) {
        if (((PARSED_JSON_OBJECT == lastStack->type) || (PARSED_JSON_INDEX == lastStack->type)) && (PARSED_JSON_FIELD == (lastStack - 1)->type)) {
            (lastStack - 1)->type = PARSED_JSON_FIELD_WITH_OBJECT;
        }
        --(lastStack);
    }


    lastStack = &((jsonRequest->elements)[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);

    while (JSONPATH_REQUEST_ROOT == currElement->type) { /* Be carefull */
        const rootRule *currRoot = &(currElement->data.root);


        isJsonPathResolved(currRoot, &(currElement[currRoot->ruleSize - 1]), lastStack, s, currRoot, currStack, isComplex);

        currElement = &(currElement[currRoot->ruleSize]);;
    }

    return LE_OK;
}

lemonError updateJsonPathRequestStatusByArrayElement(httpRequest *jsonRequest) {
    requestElement *currStack = &((jsonRequest->elements)[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);
    if (PARSED_JSON_INDEX == currStack->type) {
        ++(currStack->data.index.index);
    }
    return LE_OK;
}

lemonError updateJsonPathRequestStatusByRoot(httpRequest *jsonRequest) {
    requestElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + jsonRequest->parsedStackSize]);
    currentElement->type = PARSED_JSON_ROOT;
    ++(jsonRequest->parsedStackSize);
    return LE_OK;
}

lemonError rollbackJsonPathRequestStatusByRoot(httpRequest *jsonRequest) {
    requestElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + (--(jsonRequest->parsedStackSize))]);
    currentElement->type = NONE;
    return LE_OK;
}