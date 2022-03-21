/*
 * Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022 Parkhomenko Stanislav
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

static lemonError printStack(jsonPathElement *lastStack, const char *name) {
    printf("OOOUUUTTT STACK %s <<< ", name);

    if ((lastStack -> type == PARSED_FIELD_WITH_OBJECT) || (lastStack -> type == PARSED_FIELD) || (lastStack -> type == RESOLVED_FIELD)) {
        printf(" %d(%.*s) ", lastStack -> type, lastStack->data.name.length, lastStack->data.name.data);
    } else {
        printf(" %d ", lastStack -> type);
    }
    while ( (--lastStack)->type != PARSED_ROOT ) {
        if ((lastStack -> type == PARSED_FIELD_WITH_OBJECT) || (lastStack -> type == PARSED_FIELD) || (lastStack -> type == RESOLVED_FIELD)) {
            printf(" %d(%.*s) ", lastStack -> type, lastStack->data.name.length, lastStack->data.name.data);
        } else {
            printf(" %d ", lastStack -> type);
        }
    }
    if ((lastStack -> type == PARSED_FIELD_WITH_OBJECT) || (lastStack -> type == PARSED_FIELD) || (lastStack -> type == RESOLVED_FIELD)) {
        printf(" %d(%.*s) ", lastStack -> type, lastStack->data.name.length, lastStack->data.name.data);
    } else {
        printf(" %d ", lastStack -> type);
    }
    puts("");
    return LE_OK;
}

static boolean isAllRecursiveResolved(const jsonPathElement *lastRule, const jsonPathElement *lastStack) {
    size_t recursiveRules = 0;
    size_t heads = 0;

    jsonPathElement *e = lastRule;

    while (ROOT != e->type) {
        if (RECURSIVE == e->type) {
            ++recursiveRules;
        }
        --e;
    }

    e = lastStack;

    while (PARSED_ROOT != e->type) {
        if (HEAD_OF_JOINED_OBJECT == e->type) {
            ++heads;
        }
        --e;
    }

    return recursiveRules == heads ? TRUE : FALSE;
}

static boolean isAllFieldsResolved(const jsonPathElement *lastRule, const jsonPathElement *lastStack) {
    size_t nameRules = 0;
    size_t resolvedFields = 0;

    jsonPathElement *e = lastRule;

    while (ROOT != e->type) {
        if ((NAME == e->type) || (ANY == e->type) || (ANYINDEX == e->type)) {
            ++nameRules;
        }
        --e;
    }

    e = lastStack;

    while (PARSED_ROOT != e->type) {
        if (RESOLVED_FIELD == e->type) {
            ++resolvedFields;
        }
        --e;
    }

    return nameRules == resolvedFields ? TRUE : FALSE;
}

static boolean hasOpenRecursion(const jsonPathElement *lastStack) {
    jsonPathElement *e = lastStack;

    while (PARSED_ROOT != e->type) {
        switch (e->type) {
            case JOINED_OBJECT:
                return TRUE;
            case HEAD_OF_JOINED_OBJECT:
                return FALSE;
            default:
                --e;
        }
    }

    return FALSE;
}

static lemonError isJsonPathResolved(const jsonPathElement *currRoot, const jsonPathElement *lastRule, const jsonPathElement *lastStack, const string *s, jsonPathElement *currRule, jsonPathElement *currStack, const boolean isComplex, const boolean isLastJoined) {

    /* Ugly hack, but test34 works */
    /*if ((RECURSIVE == currRule->type) && (currStack > lastStack) && (TRUE == isComplex)) {
        return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
    }*/

    while ((currRule <= lastRule) && (currStack <= lastStack)) {
        switch (currRule->type) {
            case ROOT:
                switch (currStack->type) {
                    case PARSED_ROOT:
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
                                case RECURSIVE:
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
            case ANY:
                printStack(lastStack, "INIT STATE ANY");
                switch (currStack->type) {
                    case PARSED_OBJECT:
                    case JOINED_OBJECT:
                    case HEAD_OF_JOINED_OBJECT:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if (1 + currStack <= lastStack) {
                            ++(currStack);
                        } else {
                            return LE_OK; /* ????????????? */
                        }

                        break;
                    case PARSED_INDEX:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currRule == lastRule) && (RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) && (TRUE == isComplex)) {
                            printf("RETURN DATA 2xxx %.*s\r\n", s->length, s->data);
                            puts("CALL 1xxx");
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currStack <= lastStack) && (1 + currRule <= lastRule)) {
                            currStack->type = RESOLVED_FIELD;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex, FALSE);
                            currStack->type = PARSED_INDEX;
                            return LE_OK;
                        } else {
                            return LE_OK;
                        }
                        break;
                    case PARSED_FIELD_WITH_OBJECT:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currRule == lastRule) && (RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) && (TRUE == isComplex)) {
                            printf("RETURN DATA 2xx %.*s\r\n", s->length, s->data);
                            puts("CALL 1xx");
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currStack <= lastStack) && (1 + currRule <= lastRule)) {
                            currStack->type = RESOLVED_FIELD;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex, FALSE);
                            currStack->type = PARSED_FIELD_WITH_OBJECT;
                            return LE_OK;
                        } else {
                            return LE_OK;
                        }

                        break;
                    case PARSED_FIELD:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currRule == lastRule) && (RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) && (TRUE == isComplex)) {
                            printf("RETURN DATA 2x %.*s\r\n", s->length, s->data);
                            puts("CALL 1x");
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currStack <= lastStack) && (1 + currRule <= lastRule)) {
                            currStack->type = RESOLVED_FIELD;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex, FALSE);
                            currStack->type = PARSED_FIELD;
                            return LE_OK;
                        } else {
                            return LE_OK;
                        }

                        /*++(currRule);
                        ++(currStack);*/
                        break;
                    default:
                        printf("RETURN LE_OK 5 %.*s\r\n", s->length, s->data);
                        return LE_OK;
                }
                break;
            case NAME:
                printStack(lastStack, "INIT STATE NAME X2");
                switch (currStack->type) {
                    case PARSED_FIELD:
                        if (
                                (currRule->data.name.length == currStack->data.name.length) &&
                                (0 == STRNCASECMP(currStack->data.name.data, currRule->data.name.data,
                                                  currStack->data.name.length))
                                ) {

                            /* if ((currRule == lastRule) && (currStack == lastStack)) {
                                printf("RETURN DATA 6 %.*s\r\n", s->length, s->data);
                                puts("CALL 2");
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            } else {
                                printf("RETURN LE_OK 7 %.*s\r\n", s->length, s->data);
                                return LE_OK;
                            } due to test21 */

                            /*if (currStack == lastStack) {
                                if ((currRule == lastRule) || (((currRule + 1) == lastRule) && (RECURSIVE == (currRule + 1)->type))) {
                                    printf("RETURN DATA 6 %.*s\r\n", s->length, s->data);
                                    puts("CALL 2");
                                    return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                                } else {
                                    printf("RETURN LE_OK 7 %.*s\r\n", s->length, s->data);
                                    return LE_OK;
                                }
                            }*/

                            if ((lastStack == currStack) && (lastRule == currRule)) {
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            }

                            if ((1 + currRule == lastRule) && (RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) /*&& (TRUE == isComplex)*/) {
                                printf("RETURN DATA 2xxxx %.*s\r\n", s->length, s->data);
                                puts("CALL 1xxxb");
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            }

                            /* NAME can not resolve further stack itself */
                            if ((lastStack != currStack) && (lastRule == currRule)) {
                                return LE_OK;
                            }

                            /* Not all rules have been resolved */
                            if ((lastStack == currStack) && (lastRule != currRule)) {
                                return LE_OK;
                            }

                            if (((1 + currRule) <= lastRule) && ((1 + currStack) <= lastStack)) {
                                currStack->type = RESOLVED_FIELD;
                                isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex, FALSE);
                                currStack->type = PARSED_FIELD;
                                return LE_OK;
                            } else {
                                return LE_OK;
                            }

                            /*if ((1 + currStack) <= lastStack) {
                                ++(currStack);
                            } else {
                                switch (currRule->type) {
                                    case RECURSIVE:
                                        if ((currRule == lastRule)) {
                                            printf("RETURN DATA 8 %.*s\r\n", s->length, s->data);
                                            puts("CALL 3");
                                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                                        }
                                        break;
                                    default:
                                        printf("RETURN LE_OK 9 %.*s\r\n", s->length, s->data);
                                        return LE_OK;
                                }
                            }*/
                        } else {
                            printf("RETURN LE_OK 10 %.*s\r\n", s->length, s->data);
                            return LE_OK;
                        }
                        break;
                    case PARSED_FIELD_WITH_OBJECT:
                        if (
                                (currRule->data.name.length == currStack->data.name.length) &&
                                (0 == STRNCASECMP(currStack->data.name.data, currRule->data.name.data,
                                                  currStack->data.name.length))
                                ) {

                            if ((lastStack == currStack) && (lastRule == currRule)) {
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            }

                            if ((1 + currRule == lastRule) && (RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) /*&& (TRUE == isComplex)*/) {
                                printf("RETURN DATA 2xxxx %.*s\r\n", s->length, s->data);
                                puts("CALL 1xxxc");
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            }

                            /* NAME can not resolve further stack itself */
                            if ((lastStack != currStack) && (lastRule == currRule)) {
                                return LE_OK;
                            }

                            /* Not all rules have been resolved */
                            if ((lastStack == currStack) && (lastRule != currRule)) {
                                return LE_OK;
                            }

                            if (((1 + currRule) <= lastRule) && ((1 + currStack) <= lastStack)) {
                                currStack->type = RESOLVED_FIELD;
                                isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex, FALSE);
                                currStack->type = PARSED_FIELD_WITH_OBJECT;
                                return LE_OK;
                            }

                            /*if ((1 + currRule) <= lastRule) {
                                ++(currRule);
                            } else {
                                if ((TRUE == isComplex) && (currRule == lastRule) && (currStack == lastStack) && (FALSE == isLastJoined)) {
                                    printf("RETURN DATA 11 %.*s\r\n", s->length, s->data);
                                    puts("CALL 4");
                                    return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                                } else {
                                    printf("RETURN LE_OK 12 %.*s\r\n", s->length, s->data);
                                    return LE_OK;
                                }
                            }

                            if ((1 + currStack) <= lastStack) {
                                ++(currStack);
                            } else {
                                switch (currRule->type) {
                                    case RECURSIVE:
                                        if ((TRUE == isComplex) && (currRule == lastRule) && (FALSE == isLastJoined)) {
                                            printf("RETURN DATA 13 %.*s\r\n", s->length, s->data);
                                            puts("CALL 5");
                                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                                        }
                                        break;
                                    default:
                                        printf("RETURN LE_OK 14 %.*s\r\n", s->length, s->data);
                                        return LE_OK;
                                }
                                return LE_OK;
                            }*/
                        } else {
                            printf("RETURN LE_OK 15 %.*s\r\n", s->length, s->data);
                            return LE_OK;
                        }
                        break;
                    case PARSED_INDEX:
                        printf("RETURN LE_OK 16 %.*s\r\n", s->length, s->data);
                        return LE_OK;
                    case PARSED_OBJECT:
                    case JOINED_OBJECT:
                    case HEAD_OF_JOINED_OBJECT:
                        ++(currStack);
                        break;
                    default:
                        /* ERROR */
                        printf("RETURN LE_OK 17 %.*s\r\n", s->length, s->data);
                        return LE_OK;;
                }
                break;
            case INDEX:
                printStack(lastStack, "INIT STATE INDEX");
                switch (currStack->type) {
                    case PARSED_OBJECT:
                    case JOINED_OBJECT:
                    case HEAD_OF_JOINED_OBJECT:
                        ++(currStack);
                        break;
                    case PARSED_INDEX:

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
            case ANYINDEX:
                printStack(lastStack, "INIT STATE ANYINDEX");
                switch (currStack->type) {
                    case PARSED_OBJECT:
                    case JOINED_OBJECT:
                    case HEAD_OF_JOINED_OBJECT:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if (1 + currStack <= lastStack) {
                            ++(currStack);
                        } else {
                            return LE_OK; /* ????????????? */
                        }

                        break;
                    case PARSED_INDEX:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currRule == lastRule) && (RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) && (TRUE == isComplex)) {
                            printf("RETURN DATA 2xxx %.*s\r\n", s->length, s->data);
                            puts("CALL 1xxx");
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currStack <= lastStack) && (1 + currRule <= lastRule)) {
                            currStack->type = RESOLVED_FIELD;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex, FALSE);
                            currStack->type = PARSED_INDEX;
                            return LE_OK;
                        } else {
                            return LE_OK;
                        }
                        break;
                    case PARSED_FIELD_WITH_OBJECT:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currRule == lastRule) && (RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) && (TRUE == isComplex)) {
                            printf("RETURN DATA 2xx %.*s\r\n", s->length, s->data);
                            puts("CALL 1xx");
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currStack <= lastStack) && (1 + currRule <= lastRule)) {
                            currStack->type = RESOLVED_FIELD;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex, FALSE);
                            currStack->type = PARSED_FIELD_WITH_OBJECT;
                            return LE_OK;
                        } else {
                            return LE_OK;
                        }

                        break;
                    case PARSED_FIELD:
                        if ((lastStack == currStack) && (lastRule == currRule)) {
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currRule == lastRule) && (RECURSIVE == (1 + currRule)->type) && (currStack == lastStack) && (TRUE == isComplex)) {
                            printf("RETURN DATA 2x %.*s\r\n", s->length, s->data);
                            puts("CALL 1x");
                            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                        }

                        if ((1 + currStack <= lastStack) && (1 + currRule <= lastRule)) {
                            currStack->type = RESOLVED_FIELD;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex, FALSE);
                            currStack->type = PARSED_FIELD;
                            return LE_OK;
                        } else {
                            return LE_OK;
                        }

                        /*++(currRule);
                        ++(currStack);*/
                        break;
                    default:
                        printf("RETURN LE_OK 20 %.*s\r\n", s->length, s->data);
                        return LE_OK;
                }
                break;
            case RECURSIVE: /* currRule */
                /* printStack(lastStack, "INIT STATE RECURSIVE"); */

                /*if ((lastStack == currStack) && (lastRule == currRule) && (TRUE == isAllRecursiveResolved(lastRule - 1, lastStack)) && (TRUE == isAllFieldsResolved(lastRule, lastStack)) *&& (FALSE == hasOpenRecursion(lastStack))*) {
                     EMPTY RECURSIVE            test22 reject it
                    puts("CALL 8");
                    return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                }*/

                /*if ((lastStack == currStack + 1) && (lastRule == currRule) && (TRUE == isAllRecursiveResolved(lastRule - 1, lastStack)) && (TRUE == isAllFieldsResolved(lastRule, lastStack))) {
                    if (TRUE == isComplex) {
                        puts("CALL 8b");
                        return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                    } else {
                        puts("Errrrr");
                        return LE_OK;
                    }

                }*/

                /* if ((PARSED_OBJECT == (currStack - 1)->type) || (JOINED_OBJECT == (currStack - 1)->type) || (HEAD_OF_JOINED_OBJECT == (currStack - 1)->type)) { */

                switch (currStack->type) {
                    case PARSED_OBJECT:
                        if (currRule == lastRule) {
                            currStack->type = HEAD_OF_JOINED_OBJECT;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule, currStack, isComplex, FALSE);
                            currStack->type = PARSED_OBJECT;
                            printf("RETURN LE_OK 25 %.*s\r\n", s->length, s->data);
                            return LE_OK; /* by test 1  ??????????????????????????????????????????????? Can recursive be empty? */
                        } else {
                            currStack->type = JOINED_OBJECT;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule, currStack, isComplex, TRUE);
                            currStack->type = HEAD_OF_JOINED_OBJECT;
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule, currStack, isComplex, FALSE);
                            currStack->type = PARSED_OBJECT;
                            printf("RETURN LE_OK 25 %.*s\r\n", s->length, s->data);
                            return LE_OK; /* by test 1  ??????????????????????????????????????????????? Can recursive be empty? */
                        }
                        return LE_OK;
                    case JOINED_OBJECT:
                        if (lastStack == currStack) {
                            printf("ERROR STATE 1 %.*s\r\n", s->length, s->data);
                            printStack(lastStack, "ERROR STATE 1");
                            return LE_OK;
                        } else {
                            ++(currStack);
                        }
                        break;
                    case HEAD_OF_JOINED_OBJECT: /* currStack */
                        if (lastRule == currRule) {
                            /* DO NOT KNOW */
                            /*if (((currStack + 1) == lastStack) && (((currStack + 1)->type == PARSED_FIELD_WITH_OBJECT) || ((currStack + 1)->type == PARSED_FIELD)) && (TRUE == isAllRecursiveResolved(lastRule, lastStack)) && (TRUE == isAllFieldsResolved(lastRule, lastStack))) {
                                puts("CALL 6");
                                return LE_OK;
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            } else {
                                printf("ERROR STATE 2 %.*s\r\n", s->length, s->data);
                                printStack(lastStack, "ERROR STATE 2");
                                return LE_OK;
                            }*/
                            if ((TRUE == isComplex) /*&& (TRUE == isAllRecursiveResolved(lastRule, lastStack)) && (TRUE == isAllFieldsResolved(lastRule, lastStack))*/) {
                                puts("CALL 6");
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            } else {
                                printf("ERROR STATE 2 %.*s\r\n", s->length, s->data);
                                printStack(lastStack, "ERROR STATE 2");
                                return LE_OK;
                            }
                        } else {
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack + 1, isComplex, FALSE);
                            currStack->type = PARSED_OBJECT;
                            printf("RETURN LE_OK 28 %.*s\r\n", s->length, s->data);
                            return LE_OK; /* May be break ??? */
                        }
                        break;
                    case PARSED_FIELD:
                        return LE_OK;
                        break;
                    case PARSED_INDEX:
                        if ((currStack == lastStack) && (lastRule == currRule) /*&& (TRUE == isAllRecursiveResolved(lastRule - 1, lastStack)) && (TRUE == isAllFieldsResolved(lastRule, lastStack))*/) {
                            if (TRUE == isComplex) {
                                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
                            } else {
                                return LE_OK;
                            }

                            /* return LE_OK; */
                        } else {
                            /* ++(currStack); */
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule, currStack + 1, isComplex, FALSE);
                            isJsonPathResolved(currRoot, lastRule, lastStack, s, currRule + 1, currStack, isComplex, FALSE);
                            return LE_OK;
                        }
                        /*return LE_OK;*/
                        break;
                    case PARSED_FIELD_WITH_OBJECT:
                        if (currStack == lastStack) {
                            return LE_OK;
                        } else {
                            ++(currStack);
                        }
                        break;
                    case ROOT:
                    case NONE:
                        /*(currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);*/
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

    /*if ((RECURSIVE == currRule->type) && (lastRule == currRule)) {
        if ((PARSED_FIELD_WITH_OBJECT == (currStack - 1)->type) || ((TRUE == isComplex) && (PARSED_ROOT == (currStack - 1)->type))) {
            printf("OOOUUUTTTT TRY5\n");
            return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
        } else {
            printf("OOOUUUTTTT TRY3\n");
            printStack(lastStack);
            if ((FALSE == isComplex) && (PARSED_FIELD == (currStack - 1)->type) && ((currStack - 1) == lastStack)) {
                return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
            } else {
                return LE_OK;
            }
        }
    }*/

    /* TODO: Fix ugly hack! */
    /*if ((currRule - 1 == lastRule) && (RECURSIVE != (currRule - 1)->type) && (currStack - 1 == lastStack)) {
        printf("OOOUUUTTTT TRY4\n");
        printStack(lastStack, "TRY4");
        return (currRoot->data.root.callback.handler)(s, currRoot->data.root.callback.data);
    }*/

    /* if (currStack->type == JOINED_OBJECT) { currStack->type = PARSED_OBJECT; } */
    return LE_OK;
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
    /*jsonPathElement *currElement = jsonRequest->elements;
    while (ROOT == currElement->type) {
        const rootRule *currRoot = &(currElement->data.root);
        const jsonPathElement *currStack = &((jsonRequest->elements)[jsonRequest->elementsCount]);
        const jsonPathElement *lastStack = &((jsonRequest->elements)[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);

        string s;
        s.data = lastStack->data.containerStartPosition;
        s.length = endObjectPosition - s.data + 1;
        if (RECURSIVE == currElement[currRoot->ruleSize - 1].type) {
            isJsonPathResolved(currRoot, &(currElement[currRoot->ruleSize - 1]), lastStack, &s, currRoot, currStack);
        }

        currElement = &(currElement[currRoot->ruleSize]);
    }*/
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

const lemonError executeJsonPathCallbackWithValue(jsonPathRequest *jsonRequest, const string *s, const boolean isComplex) {
    jsonPathElement *currElement = jsonRequest->elements;

    const jsonPathElement *currStack = &((jsonRequest->elements)[jsonRequest->elementsCount]);
    jsonPathElement *lastStack = &((jsonRequest->elements)[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);

    if ((TRUE == isComplex) && (PARSED_FIELD == lastStack->type)) {
        lastStack->type = PARSED_FIELD_WITH_OBJECT;
    }

    /*if (FALSE == isComplex) {*/
        while (lastStack != currStack) {
            if (((PARSED_OBJECT == lastStack->type) || (PARSED_INDEX == lastStack->type)) && (PARSED_FIELD == (lastStack - 1)->type)) {
                (lastStack - 1)->type = PARSED_FIELD_WITH_OBJECT;
            }
            --(lastStack);
        }
    /*}*/


    lastStack = &((jsonRequest->elements)[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);

    while (ROOT == currElement->type) { /* Be carefull */
        const rootRule *currRoot = &(currElement->data.root);


        isJsonPathResolved(currRoot, &(currElement[currRoot->ruleSize - 1]), lastStack, s, currRoot, currStack, isComplex, FALSE);

        currElement = &(currElement[currRoot->ruleSize]);;
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