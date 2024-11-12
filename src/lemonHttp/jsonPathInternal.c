/*
 * Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022, 2023, 2024 Parkhomenko Stanislav
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

const char * getRuleName(const ruleType rule) {
    switch (rule) {
        case NONE:
            return "NONE";
        case HTTP_REQUEST_METHOD:
            return "HTTP_REQUEST_METHOD";
        case HTTP_REQUEST_URI:
            return "HTTP_REQUEST_URI";
        case HTTP_REQUEST_GET_QUERY_ELEMENT:
            return "HTTP_REQUEST_GET_QUERY_ELEMENT";
        case HTTP_REQUEST_VALUE:
            return "HTTP_REQUEST_VALUE";
        case HTTP_REQUEST_HTTP_VERSION:
            return "HTTP_REQUEST_HTTP_VERSION";
        case HTTP_REQUEST_HEADER:
            return "HTTP_REQUEST_HEADER";
        case JSONPATH_REQUEST_ROOT:
            return "JSONPATH_REQUEST_ROOT";
        case JSONPATH_REQUEST_ANY:
            return "JSONPATH_REQUEST_ANY";
        case JSONPATH_REQUEST_ANYINDEX:
            return "JSONPATH_REQUEST_ANYINDEX";
        case JSONPATH_REQUEST_NAME:
            return "JSONPATH_REQUEST_NAME";
        case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
            return "JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY";
        case JSONPATH_REQUEST_INDEX:
            return "JSONPATH_REQUEST_INDEX";
        case JSONPATH_REQUEST_RECURSIVE:
            return "JSONPATH_REQUEST_RECURSIVE";
        case JSONPATH_REQUEST_OBJECT:
            return "JSONPATH_REQUEST_OBJECT";
        case JSONPATH_REQUEST_ARRAY:
            return "JSONPATH_REQUEST_ARRAY";
        case JSONPATH_REQUEST_TERMINATOR:
            return "JSONPATH_REQUEST_TERMINATOR";
        case PARSED_JSON_ROOT:
            return "PARSED_JSON_ROOT";
        case PARSED_JSON_OBJECT:
            return "PARSED_JSON_OBJECT";
        case PARSED_JSON_JOINED_OBJECT:
            return "PARSED_JSON_JOINED_OBJECT";
        case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
            return "PARSED_JSON_HEAD_OF_JOINED_OBJECT";
        case PARSED_JSON_ARRAY:
            return "PARSED_JSON_ARRAY";
        case PARSED_JSON_ARRAY_VALUE:
            return "PARSED_JSON_ARRAY_VALUE";
        case PARSED_JSON_FIELD:
            return "PARSED_JSON_FIELD";
        case PARSED_JSON_FIELD_WITH_OBJECT:
            return "PARSED_JSON_FIELD_WITH_OBJECT";
        case PARSED_JSON_RESOLVED_FIELD:
            return "PARSED_JSON_RESOLVED_FIELD";
        case PARSED_JSON_VALUE:
            return "PARSED_JSON_VALUE";
        case FINAL_ON_SUCCESS_CALLBACK:
            return "FINAL_ON_SUCCESS_CALLBACK";
        case ON_START_CALLBACK:
            return "ON_START_CALLBACK";
    }
}

lemonError appendJsonPathElementOfHttpRequest(httpRequest *r, const string *s, const ruleType type) {
    const string emptyString = getEmptyString();
    /* if ((NULL == r) || (NULL == s) || (NULL == s->data) ||
        (0 > r->elementsCount) ||
        (((emptyString.length == s->length) && (emptyString.data != s->data)) || ((emptyString.length != s->length) && (emptyString.data == s->data))) ||
        ((emptyString.length == s->length) && (emptyString.data == s->data) && (JSONPATH_REQUEST_ROOT != type) && (JSONPATH_REQUEST_ANY != type) && (JSONPATH_REQUEST_ANYINDEX != type) && (JSONPATH_REQUEST_RECURSIVE != type)) ||
        (emptyString.length > s->length)) {
        return LE_NULL_IN_INPUT_VALUES;
    }*/
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
            case JSONPATH_REQUEST_ARRAY:
            case JSONPATH_REQUEST_OBJECT:
            case JSONPATH_REQUEST_TERMINATOR:
                break;
            default:
                return LE_INCORRECT_INPUT_VALUES;
        }
        return LE_OK;
    }
}

lemonError openFrame(httpRequest *jsonRequest, const char *position, const ruleType type) {
    requestElement *newStackElement = &(jsonRequest->elements[jsonRequest->elementsCount + jsonRequest->parsedStackSize]);
    requestElement *stackElements = jsonRequest->elements;
    newStackElement->type = type;
    newStackElement->fType = USELESS;
    newStackElement->frameStartPosition = position;

    ++(jsonRequest->parsedStackSize);

    while (JSONPATH_REQUEST_ROOT == stackElements->type) { /* Be carefull */
        const size_t ruleSize = stackElements->data.root.ruleSize;
        requestElement *currRule = &(stackElements[stackElements->data.root.resolvedRulesCount]);

        /* isJsonPathResolved(currElement, &(currElement[ruleSize - 1]), lastStack, s, currElement, currStack, isComplex); */

        /* if ((NULL != stackElements->data.root.alreadyFailed) && (PARSED_JSON_FIELD == ((requestElement *)(stackElements->data.root.alreadyFailed))->type)) {
            stackElements->data.root.alreadyFailed = newStackElement;
        }*/

        if ((NULL == stackElements->data.root.alreadyFailed) && (ruleSize - 1  == stackElements->data.root.resolvedRulesCount)) {
            printf("  LOCK FRAME: ruleLine = %p, frame = %p, position = %.20s\n", stackElements, newStackElement, position);
            stackElements->data.root.alreadyFailed = newStackElement;
            newStackElement->fType = REQUIRED;
        }

        if (NULL == stackElements->data.root.alreadyFailed) {

            printf("OPEN FRAME: ruleLine = %p, currRule = %s meets arg = %s, position = %.20s\n", stackElements, getRuleName(currRule->type), getRuleName(type), position);

            switch (currRule->type) {
                case JSONPATH_REQUEST_ROOT:
                    switch (type) {
                        case PARSED_JSON_ROOT:
                            ++(stackElements->data.root.resolvedRulesCount);
                            printf("Next rule type 2: %d (root resolved)\n", stackElements[stackElements->data.root.resolvedRulesCount].type);
                            break;
                        case PARSED_JSON_OBJECT:
                        case PARSED_JSON_JOINED_OBJECT:
                        case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        case PARSED_JSON_ARRAY:
                        case PARSED_JSON_ARRAY_VALUE:
                        case PARSED_JSON_FIELD:
                        case PARSED_JSON_FIELD_WITH_OBJECT:
                        case PARSED_JSON_RESOLVED_FIELD:
                        case PARSED_JSON_VALUE:
                            stackElements->data.root.alreadyFailed = newStackElement;
                            printf("  LOCK FRAME: ruleLine = %p, frame = %p, position = %.20s\n", stackElements, newStackElement, position);
                            puts("(rule type) RRROOOTTT???");
                            break;
                        case NONE:
                        case HTTP_REQUEST_METHOD:
                        case HTTP_REQUEST_URI:
                        case HTTP_REQUEST_GET_QUERY_ELEMENT:
                        case HTTP_REQUEST_VALUE:
                        case HTTP_REQUEST_HTTP_VERSION:
                        case HTTP_REQUEST_HEADER:
                        case JSONPATH_REQUEST_ROOT:
                        case JSONPATH_REQUEST_ANY:
                        case JSONPATH_REQUEST_ANYINDEX:
                        case JSONPATH_REQUEST_NAME:
                        case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                        case JSONPATH_REQUEST_INDEX:
                        case JSONPATH_REQUEST_RECURSIVE:
                        case JSONPATH_REQUEST_OBJECT:
                        case JSONPATH_REQUEST_ARRAY:
                        case FINAL_ON_SUCCESS_CALLBACK:
                        case ON_START_CALLBACK:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case JSONPATH_REQUEST_OBJECT:
                    switch (type) {
                        case PARSED_JSON_OBJECT:
                            ++(stackElements->data.root.resolvedRulesCount);
                            printf("Next rule type 2: %d (object resolved)\n", stackElements[stackElements->data.root.resolvedRulesCount].type);
                            break;
                        case PARSED_JSON_ROOT:
                        case PARSED_JSON_JOINED_OBJECT:
                        case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        case PARSED_JSON_ARRAY:
                        case PARSED_JSON_ARRAY_VALUE:
                        case PARSED_JSON_FIELD:
                        case PARSED_JSON_FIELD_WITH_OBJECT:
                        case PARSED_JSON_RESOLVED_FIELD:
                        case PARSED_JSON_VALUE:
                            stackElements->data.root.alreadyFailed = newStackElement;
                            printf("  LOCK FRAME: ruleLine = %p, frame = %p, position = %.20s\n", stackElements, newStackElement, position);
                            puts("(rule type) OBJECT???");
                            break;
                        case NONE:
                        case HTTP_REQUEST_METHOD:
                        case HTTP_REQUEST_URI:
                        case HTTP_REQUEST_GET_QUERY_ELEMENT:
                        case HTTP_REQUEST_VALUE:
                        case HTTP_REQUEST_HTTP_VERSION:
                        case HTTP_REQUEST_HEADER:
                        case JSONPATH_REQUEST_ROOT:
                        case JSONPATH_REQUEST_ANY:
                        case JSONPATH_REQUEST_ANYINDEX:
                        case JSONPATH_REQUEST_NAME:
                        case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                        case JSONPATH_REQUEST_INDEX:
                        case JSONPATH_REQUEST_RECURSIVE:
                        case JSONPATH_REQUEST_OBJECT:
                        case JSONPATH_REQUEST_ARRAY:
                        case FINAL_ON_SUCCESS_CALLBACK:
                        case ON_START_CALLBACK:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case JSONPATH_REQUEST_ARRAY:
                    switch (type) {
                        case PARSED_JSON_ARRAY:
                            /* It contains the current index value */
                            newStackElement->data.index.index = 0;
                            newStackElement->data.index.containerStartPosition = position;
                            ++(stackElements->data.root.resolvedRulesCount);
                            break;
                        case PARSED_JSON_ARRAY_VALUE:
                        case PARSED_JSON_ROOT:
                        case PARSED_JSON_OBJECT:
                        case PARSED_JSON_JOINED_OBJECT:
                        case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        case PARSED_JSON_FIELD:
                        case PARSED_JSON_FIELD_WITH_OBJECT:
                        case PARSED_JSON_RESOLVED_FIELD:
                        case PARSED_JSON_VALUE:
                            stackElements->data.root.alreadyFailed = newStackElement;
                            printf("  LOCK FRAME: ruleLine = %p, frame = %p, position = %.20s\n", stackElements, newStackElement, position);
                            puts("(rule type) ARRAY???");
                            break;
                        case NONE:
                        case HTTP_REQUEST_METHOD:
                        case HTTP_REQUEST_URI:
                        case HTTP_REQUEST_GET_QUERY_ELEMENT:
                        case HTTP_REQUEST_VALUE:
                        case HTTP_REQUEST_HTTP_VERSION:
                        case HTTP_REQUEST_HEADER:
                        case JSONPATH_REQUEST_ROOT:
                        case JSONPATH_REQUEST_ANY:
                        case JSONPATH_REQUEST_ANYINDEX:
                        case JSONPATH_REQUEST_NAME:
                        case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                        case JSONPATH_REQUEST_INDEX:
                        case JSONPATH_REQUEST_RECURSIVE:
                        case JSONPATH_REQUEST_OBJECT:
                        case JSONPATH_REQUEST_ARRAY:
                        case FINAL_ON_SUCCESS_CALLBACK:
                        case ON_START_CALLBACK:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case JSONPATH_REQUEST_ANY:
                case JSONPATH_REQUEST_NAME:
                    switch (type) {
                        case PARSED_JSON_FIELD:
                            /* We force REQUIRED because we need a field name */
                            newStackElement->fType = REQUIRED;
                            break;
                        case PARSED_JSON_ROOT:
                        case PARSED_JSON_OBJECT:
                        case PARSED_JSON_JOINED_OBJECT:
                        case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        case PARSED_JSON_ARRAY:
                        case PARSED_JSON_ARRAY_VALUE:
                        case PARSED_JSON_FIELD_WITH_OBJECT:
                        case PARSED_JSON_RESOLVED_FIELD:
                        case PARSED_JSON_VALUE:
                            stackElements->data.root.alreadyFailed = newStackElement;
                            printf("  LOCK FRAME: ruleLine = %p, frame = %p, position = %.20s\n", stackElements, newStackElement, position);
                            puts("(rule type) FIELD???");
                            break;
                        case NONE:
                        case HTTP_REQUEST_METHOD:
                        case HTTP_REQUEST_URI:
                        case HTTP_REQUEST_GET_QUERY_ELEMENT:
                        case HTTP_REQUEST_VALUE:
                        case HTTP_REQUEST_HTTP_VERSION:
                        case HTTP_REQUEST_HEADER:
                        case JSONPATH_REQUEST_ROOT:
                        case JSONPATH_REQUEST_ANY:
                        case JSONPATH_REQUEST_ANYINDEX:
                        case JSONPATH_REQUEST_NAME:
                        case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                        case JSONPATH_REQUEST_INDEX:
                        case JSONPATH_REQUEST_RECURSIVE:
                        case JSONPATH_REQUEST_OBJECT:
                        case JSONPATH_REQUEST_ARRAY:
                        case FINAL_ON_SUCCESS_CALLBACK:
                        case ON_START_CALLBACK:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case JSONPATH_REQUEST_ANYINDEX:
                    switch (type) {
                        case PARSED_JSON_ARRAY_VALUE:
                            newStackElement->fType = REQUIRED;
                            newStackElement->frameStartPosition = position;
                            ++(stackElements->data.root.resolvedRulesCount);
                            break;
                        case NONE:
                        case HTTP_REQUEST_METHOD:
                        case HTTP_REQUEST_URI:
                        case HTTP_REQUEST_GET_QUERY_ELEMENT:
                        case HTTP_REQUEST_VALUE:
                        case HTTP_REQUEST_HTTP_VERSION:
                        case HTTP_REQUEST_HEADER:
                        case JSONPATH_REQUEST_ROOT:
                        case JSONPATH_REQUEST_ANY:
                        case JSONPATH_REQUEST_ANYINDEX:
                        case JSONPATH_REQUEST_NAME:
                        case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                        case JSONPATH_REQUEST_INDEX:
                        case JSONPATH_REQUEST_RECURSIVE:
                        case JSONPATH_REQUEST_OBJECT:
                        case JSONPATH_REQUEST_ARRAY:
                        case JSONPATH_REQUEST_TERMINATOR:
                        case PARSED_JSON_ROOT:
                        case PARSED_JSON_OBJECT:
                        case PARSED_JSON_JOINED_OBJECT:
                        case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        case PARSED_JSON_ARRAY:
                        case PARSED_JSON_FIELD:
                        case PARSED_JSON_FIELD_WITH_OBJECT:
                        case PARSED_JSON_RESOLVED_FIELD:
                        case PARSED_JSON_VALUE:
                        case FINAL_ON_SUCCESS_CALLBACK:
                        case ON_START_CALLBACK:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case JSONPATH_REQUEST_INDEX:
                    switch (type) {
                        case PARSED_JSON_ARRAY_VALUE:
                            /* We store current index value at JSONPATH_REQUEST_ARRAY */
                            if (currRule->data.index.index != (currRule - 1)->data.index.index) {
                                stackElements->data.root.alreadyFailed = newStackElement;
                                printf("  LOCK FRAME: ruleLine = %p, frame = %p, position = %.20s\n", stackElements, newStackElement, position);
                            } else {
                                newStackElement->fType = REQUIRED;
                                newStackElement->frameStartPosition = position;
                                ++(stackElements->data.root.resolvedRulesCount);
                            }
                            break;
                        case NONE:
                        case HTTP_REQUEST_METHOD:
                        case HTTP_REQUEST_URI:
                        case HTTP_REQUEST_GET_QUERY_ELEMENT:
                        case HTTP_REQUEST_VALUE:
                        case HTTP_REQUEST_HTTP_VERSION:
                        case HTTP_REQUEST_HEADER:
                        case JSONPATH_REQUEST_ROOT:
                        case JSONPATH_REQUEST_ANY:
                        case JSONPATH_REQUEST_ANYINDEX:
                        case JSONPATH_REQUEST_NAME:
                        case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                        case JSONPATH_REQUEST_INDEX:
                        case JSONPATH_REQUEST_RECURSIVE:
                        case JSONPATH_REQUEST_OBJECT:
                        case JSONPATH_REQUEST_ARRAY:
                        case JSONPATH_REQUEST_TERMINATOR:
                        case PARSED_JSON_ROOT:
                        case PARSED_JSON_OBJECT:
                        case PARSED_JSON_JOINED_OBJECT:
                        case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        case PARSED_JSON_ARRAY:
                        case PARSED_JSON_FIELD:
                        case PARSED_JSON_FIELD_WITH_OBJECT:
                        case PARSED_JSON_RESOLVED_FIELD:
                        case PARSED_JSON_VALUE:
                        case FINAL_ON_SUCCESS_CALLBACK:
                        case ON_START_CALLBACK:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    /* if (currRule->data.index.index != (newStackElement - 1)->data.index.index) {
                        if (REQUIRED == (newStackElement - 1)->fType) {
                            --(stackElements->data.root.resolvedRulesCount);
                        }
                        stackElements->data.root.alreadyFailed = newStackElement;
                        (newStackElement - 1)->fType = USELESS;
                        (newStackElement - 1)->frameStartPosition = NULL;
                    } else {
                        (newStackElement - 1)->fType = REQUIRED;
                        (newStackElement - 1)->frameStartPosition = position;
                        ++(stackElements->data.root.resolvedRulesCount);
                    }*/
                    puts("(rule type) INDEX???");
                    break;
                case NONE:
                case HTTP_REQUEST_METHOD:
                case HTTP_REQUEST_URI:
                case HTTP_REQUEST_GET_QUERY_ELEMENT:
                case HTTP_REQUEST_VALUE:
                case HTTP_REQUEST_HTTP_VERSION:
                case HTTP_REQUEST_HEADER:
                case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                case JSONPATH_REQUEST_RECURSIVE:
                case PARSED_JSON_ROOT:
                case PARSED_JSON_OBJECT:
                case PARSED_JSON_JOINED_OBJECT:
                case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                case PARSED_JSON_ARRAY:
                case PARSED_JSON_ARRAY_VALUE:
                case PARSED_JSON_FIELD:
                case PARSED_JSON_FIELD_WITH_OBJECT:
                case PARSED_JSON_RESOLVED_FIELD:
                case PARSED_JSON_VALUE:
                case FINAL_ON_SUCCESS_CALLBACK:
                case ON_START_CALLBACK:
                default:
                    puts("wtf");
                    return LE_INCORRECT_INPUT_VALUES;
            }
        }



        stackElements = &(stackElements[ruleSize]);
    }

    return LE_OK;
}

lemonError setFrameLength(httpRequest *jsonRequest, const size_t length) {
    (&(jsonRequest->elements[jsonRequest->elementsCount + (jsonRequest->parsedStackSize - 1)]))->frameLength = length;
    printf("SET LENGTH %lu\n", length);
    return LE_OK;
}

lemonError setFrameString(httpRequest *jsonRequest, const size_t length) {
    requestElement *lastStackElement = &((jsonRequest->elements)[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);
    lastStackElement->frameLength = length;
    lastStackElement->vType = VSTRING;
    ++(lastStackElement->frameStartPosition);
    printf("SET STRING %lu\n", length);
    return LE_OK;
}

lemonError setNull(httpRequest *jsonRequest) {
    requestElement *lastStackElement = &((jsonRequest->elements)[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);
    lastStackElement->frameLength = 4;
    lastStackElement->vType = VSTRING;
    lastStackElement->frameStartPosition = getNullString().data;
    printf("SET STRING %lu\n", 4);
    return LE_OK;
}

lemonError setFalse(httpRequest *jsonRequest) {
    requestElement *lastStackElement = &((jsonRequest->elements)[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);
    lastStackElement->frameLength = 5;
    lastStackElement->vType = VSTRING;
    lastStackElement->frameStartPosition = getFalseString().data;
    printf("SET STRING %lu\n", 5);
    return LE_OK;
}

lemonError setTrue(httpRequest *jsonRequest) {
    requestElement *lastStackElement = &((jsonRequest->elements)[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);
    lastStackElement->frameLength = 4;
    lastStackElement->vType = VSTRING;
    lastStackElement->frameStartPosition = getTrueString().data;
    printf("SET STRING %lu\n", 4);
    return LE_OK;
}

lemonError closeFrame(httpRequest * jsonRequest, const char *position) {

    requestElement *stackElements = jsonRequest->elements;
    requestElement *lastStackElement = &((jsonRequest->elements)[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);
    const ruleType type = lastStackElement->type;

    while (JSONPATH_REQUEST_ROOT == stackElements->type) { /* Be carefull */
        const size_t ruleSize = stackElements->data.root.ruleSize;
        requestElement *currRule = &(stackElements[stackElements->data.root.resolvedRulesCount]);

        if (lastStackElement == stackElements->data.root.alreadyFailed) {
            printf("UNLOCK FRAME: ruleLine = %p, frame = %p\n", stackElements, stackElements->data.root.alreadyFailed);
            stackElements->data.root.alreadyFailed = NULL;
        }

        /* if ((NULL == stackElements->data.root.alreadyFailed) && (ruleSize - 1 == stackElements->data.root.resolvedRulesCount) && (REQUIRED == lastStackElement->fType)) {
            string s;
            s.data = lastStackElement->frameStartPosition;
            s.length = lastStackElement->frameLength;
            (stackElements->data.root.callback.handler)(&s, stackElements->data.root.callback.data);
        }*/

        /* This is more correct */
        if ((JSONPATH_REQUEST_TERMINATOR == currRule->type) && (NULL == stackElements->data.root.alreadyFailed)) {

            string s;
            s.data = lastStackElement->frameStartPosition;
            s.length = lastStackElement->frameLength;
            (stackElements->data.root.callback.handler)(&s, stackElements->data.root.callback.data);

            --(stackElements->data.root.resolvedRulesCount);
            currRule = &(stackElements[stackElements->data.root.resolvedRulesCount]);

            printf("CLOSE TERM FRAME: ruleLine = %p, currRule = %s meets arg = %s\n", stackElements, getRuleName(currRule->type), getRuleName(type));
        }

        if (NULL == stackElements->data.root.alreadyFailed) {

            printf("CLOSE FRAME: ruleLine = %p, currRule = %s meets arg = %s\n", stackElements, getRuleName(currRule->type), getRuleName(type));

            switch (currRule->type) {
                /*case JSONPATH_REQUEST_TERMINATOR:
                    --(stackElements->data.root.resolvedRulesCount);
                    currRule = &(stackElements[stackElements->data.root.resolvedRulesCount]);
                    break; */
                case JSONPATH_REQUEST_ROOT:
                    switch (type) {
                        case PARSED_JSON_ROOT:
                            if (0 < stackElements->data.root.resolvedRulesCount) {
                                --(stackElements->data.root.resolvedRulesCount);
                            } else {
                                puts("ERROR 10");
                                return LE_INCORRECT_INPUT_VALUES;
                            }

                            puts("(rule type) Out of root abs\n");
                            break;
                        case PARSED_JSON_OBJECT:
                        case PARSED_JSON_JOINED_OBJECT:
                        case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        case PARSED_JSON_ARRAY:
                        case PARSED_JSON_ARRAY_VALUE:
                        case PARSED_JSON_FIELD:
                        case PARSED_JSON_FIELD_WITH_OBJECT:
                        case PARSED_JSON_RESOLVED_FIELD:
                        case PARSED_JSON_VALUE:
                            puts("(rule type) out RRROOOTTT???");
                            break;
                        case NONE:
                        case HTTP_REQUEST_METHOD:
                        case HTTP_REQUEST_URI:
                        case HTTP_REQUEST_GET_QUERY_ELEMENT:
                        case HTTP_REQUEST_VALUE:
                        case HTTP_REQUEST_HTTP_VERSION:
                        case HTTP_REQUEST_HEADER:
                        case JSONPATH_REQUEST_ROOT:
                        case JSONPATH_REQUEST_ANY:
                        case JSONPATH_REQUEST_ANYINDEX:
                        case JSONPATH_REQUEST_NAME:
                        case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                        case JSONPATH_REQUEST_INDEX:
                        case JSONPATH_REQUEST_RECURSIVE:
                        case JSONPATH_REQUEST_OBJECT:
                        case JSONPATH_REQUEST_ARRAY:
                        case FINAL_ON_SUCCESS_CALLBACK:
                        case ON_START_CALLBACK:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case JSONPATH_REQUEST_OBJECT:
                    switch (type) {
                        case PARSED_JSON_OBJECT:
                            if (0 < stackElements->data.root.resolvedRulesCount) {
                                --(stackElements->data.root.resolvedRulesCount);
                            } else {
                                puts("ERROR 11");
                                return LE_INCORRECT_INPUT_VALUES;
                            }
                            printf("Rollback rule type 2: %d (object out)\n", stackElements[stackElements->data.root.resolvedRulesCount].type);
                            break;
                        case PARSED_JSON_FIELD:
                            if (0 < stackElements->data.root.resolvedRulesCount) {
                                --(stackElements->data.root.resolvedRulesCount);
                            } else {
                                puts("ERROR 12");
                                return LE_INCORRECT_INPUT_VALUES;
                            }
                            printf("Rollback rule type 2: %d (object out)\n", stackElements[stackElements->data.root.resolvedRulesCount].type);
                            /*if (PARSED_JSON_OBJECT == stackElements[stackElements->data.root.resolvedRulesCount].type) {
                                --(stackElements->data.root.resolvedRulesCount);
                                printf("Rollback rule type 2: %d (object out 2)\n", stackElements[stackElements->data.root.resolvedRulesCount].type);
                            }*/
                            break;
                        case PARSED_JSON_ROOT:
                        case PARSED_JSON_JOINED_OBJECT:
                        case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        case PARSED_JSON_ARRAY:
                        case PARSED_JSON_ARRAY_VALUE:
                        case PARSED_JSON_FIELD_WITH_OBJECT:
                        case PARSED_JSON_RESOLVED_FIELD:
                        case PARSED_JSON_VALUE:
                            puts("(rule type) out OBJECT???");
                            break;
                        case NONE:
                        case HTTP_REQUEST_METHOD:
                        case HTTP_REQUEST_URI:
                        case HTTP_REQUEST_GET_QUERY_ELEMENT:
                        case HTTP_REQUEST_VALUE:
                        case HTTP_REQUEST_HTTP_VERSION:
                        case HTTP_REQUEST_HEADER:
                        case JSONPATH_REQUEST_ROOT:
                        case JSONPATH_REQUEST_ANY:
                        case JSONPATH_REQUEST_ANYINDEX:
                        case JSONPATH_REQUEST_NAME:
                        case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                        case JSONPATH_REQUEST_INDEX:
                        case JSONPATH_REQUEST_RECURSIVE:
                        case JSONPATH_REQUEST_OBJECT:
                        case JSONPATH_REQUEST_ARRAY:
                        case FINAL_ON_SUCCESS_CALLBACK:
                        case ON_START_CALLBACK:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case JSONPATH_REQUEST_ARRAY:
                    switch (type) {
                        case PARSED_JSON_ARRAY:
                            if (0 < stackElements->data.root.resolvedRulesCount) {
                                --(stackElements->data.root.resolvedRulesCount);
                            } else {
                                puts("ERROR 13");
                                return LE_INCORRECT_INPUT_VALUES;
                            }
                            printf("Rollback rule type 2: %d (array out)\n", stackElements[stackElements->data.root.resolvedRulesCount].type);
                            break;
                        case PARSED_JSON_ARRAY_VALUE:
                        case PARSED_JSON_ROOT:
                        case PARSED_JSON_OBJECT:
                        case PARSED_JSON_JOINED_OBJECT:
                        case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        case PARSED_JSON_FIELD:
                        case PARSED_JSON_FIELD_WITH_OBJECT:
                        case PARSED_JSON_RESOLVED_FIELD:
                        case PARSED_JSON_VALUE:
                            puts("(rule type) out ARRAY???");
                            break;
                        case NONE:
                        case HTTP_REQUEST_METHOD:
                        case HTTP_REQUEST_URI:
                        case HTTP_REQUEST_GET_QUERY_ELEMENT:
                        case HTTP_REQUEST_VALUE:
                        case HTTP_REQUEST_HTTP_VERSION:
                        case HTTP_REQUEST_HEADER:
                        case JSONPATH_REQUEST_ROOT:
                        case JSONPATH_REQUEST_ANY:
                        case JSONPATH_REQUEST_ANYINDEX:
                        case JSONPATH_REQUEST_NAME:
                        case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                        case JSONPATH_REQUEST_INDEX:
                        case JSONPATH_REQUEST_RECURSIVE:
                        case JSONPATH_REQUEST_OBJECT:
                        case JSONPATH_REQUEST_ARRAY:
                        case FINAL_ON_SUCCESS_CALLBACK:
                        case ON_START_CALLBACK:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case JSONPATH_REQUEST_INDEX:
                    switch (type) {
                        case PARSED_JSON_ARRAY_VALUE:
                            /* Here is bug. >Each< rule decreases lastStackElement */
                            if (currRule->data.index.index == (currRule - 1)->data.index.index) {
                                --(stackElements->data.root.resolvedRulesCount);
                            }
                            ++((currRule - 1)->data.index.index);
                            break;
                        case NONE:
                        case HTTP_REQUEST_METHOD:
                        case HTTP_REQUEST_URI:
                        case HTTP_REQUEST_GET_QUERY_ELEMENT:
                        case HTTP_REQUEST_VALUE:
                        case HTTP_REQUEST_HTTP_VERSION:
                        case HTTP_REQUEST_HEADER:
                        case JSONPATH_REQUEST_ROOT:
                        case JSONPATH_REQUEST_ANY:
                        case JSONPATH_REQUEST_ANYINDEX:
                        case JSONPATH_REQUEST_NAME:
                        case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                        case JSONPATH_REQUEST_INDEX:
                        case JSONPATH_REQUEST_RECURSIVE:
                        case JSONPATH_REQUEST_OBJECT:
                        case JSONPATH_REQUEST_ARRAY:
                        case JSONPATH_REQUEST_TERMINATOR:
                        case PARSED_JSON_ROOT:
                        case PARSED_JSON_OBJECT:
                        case PARSED_JSON_JOINED_OBJECT:
                        case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        case PARSED_JSON_ARRAY:
                        case PARSED_JSON_FIELD:
                        case PARSED_JSON_FIELD_WITH_OBJECT:
                        case PARSED_JSON_RESOLVED_FIELD:
                        case PARSED_JSON_VALUE:
                        case FINAL_ON_SUCCESS_CALLBACK:
                        case ON_START_CALLBACK:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case JSONPATH_REQUEST_ANYINDEX:
                    switch (type) {
                        case PARSED_JSON_ARRAY_VALUE:

                            /* Do NOTHING */

                            /* The next possible field is also good. */

                            /* --(stackElements->data.root.resolvedRulesCount); */
                            ++((currRule - 1)->data.index.index);
                            break;
                        case PARSED_JSON_ARRAY:
                            if (0 < stackElements->data.root.resolvedRulesCount) {
                                --(stackElements->data.root.resolvedRulesCount);
                            } else {
                                puts("ERROR 14");
                                return LE_INCORRECT_INPUT_VALUES;
                            }
                            printf("Rollback rule type 2: %d (any index out)\n", stackElements[stackElements->data.root.resolvedRulesCount].type);
                            if ((JSONPATH_REQUEST_ARRAY == stackElements[stackElements->data.root.resolvedRulesCount].type)) {
                                if (0 < stackElements->data.root.resolvedRulesCount) {
                                    --(stackElements->data.root.resolvedRulesCount);
                                } else {
                                    puts("ERROR 15");
                                    return LE_INCORRECT_INPUT_VALUES;
                                }
                                printf("Rollback rule type 3: %d (array out)\n", stackElements[stackElements->data.root.resolvedRulesCount].type);
                            }
                            break;
                        case PARSED_JSON_ROOT:
                        case PARSED_JSON_OBJECT:
                        case PARSED_JSON_JOINED_OBJECT:
                        case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        case PARSED_JSON_FIELD:
                        case PARSED_JSON_FIELD_WITH_OBJECT:
                        case PARSED_JSON_RESOLVED_FIELD:
                        case PARSED_JSON_VALUE:
                            puts("(rule type) ANYINDEX out ???");
                            break;
                        case NONE:
                        case HTTP_REQUEST_METHOD:
                        case HTTP_REQUEST_URI:
                        case HTTP_REQUEST_GET_QUERY_ELEMENT:
                        case HTTP_REQUEST_VALUE:
                        case HTTP_REQUEST_HTTP_VERSION:
                        case HTTP_REQUEST_HEADER:
                        case JSONPATH_REQUEST_ROOT:
                        case JSONPATH_REQUEST_ANY:
                        case JSONPATH_REQUEST_NAME:
                        case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                        case JSONPATH_REQUEST_INDEX:
                        case JSONPATH_REQUEST_RECURSIVE:
                        case JSONPATH_REQUEST_OBJECT:
                        case JSONPATH_REQUEST_ARRAY:
                        case FINAL_ON_SUCCESS_CALLBACK:
                        case ON_START_CALLBACK:
                        case JSONPATH_REQUEST_TERMINATOR:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case JSONPATH_REQUEST_NAME:
                    switch (type) {
                        case PARSED_JSON_FIELD:
                            if (
                                    (currRule->data.name.length == lastStackElement->data.name.length) &&
                                    (0 == STRNCASECMP(lastStackElement->data.name.data, currRule->data.name.data,
                                                      lastStackElement->data.name.length))
                                    ) {
                                /* if ((ruleSize == stackElements->data.root.resolvedRulesCount)) {
                                    lastStackElement->fType = USELESS;
                                };*/
                                lastStackElement->fType = USELESS; /* ??????????????? */
                                if (0 < stackElements->data.root.resolvedRulesCount) {
                                    /* --(stackElements->data.root.resolvedRulesCount); */
                                } else {
                                    puts("ERROR 16");
                                    return LE_INCORRECT_INPUT_VALUES;
                                }
                                printf("Rollback rule type 2: %d (resolved field out)\n", stackElements[stackElements->data.root.resolvedRulesCount].type);
                            }
                            break;
                        case PARSED_JSON_OBJECT:
                            if (0 < stackElements->data.root.resolvedRulesCount) {
                                /* Exit from JSONPATH_REQUEST_NAME */
                                --(stackElements->data.root.resolvedRulesCount);
                                /* Exit from JSONPATH_REQUEST_OBJECT */
                                --(stackElements->data.root.resolvedRulesCount);
                            } else {
                                puts("ERROR 17");
                                return LE_INCORRECT_INPUT_VALUES;
                            }
                            printf("Rollback rule type 2: %d (object out at field rule)\n", stackElements[stackElements->data.root.resolvedRulesCount].type);
                            break;
                        case PARSED_JSON_ARRAY:
                            if (0 < stackElements->data.root.resolvedRulesCount) {
                                --(stackElements->data.root.resolvedRulesCount);
                            } else {
                                puts("ERROR 17b");
                                return LE_INCORRECT_INPUT_VALUES;
                            }
                            printf("Rollback rule type 2: %d (array out at field rule)\n", stackElements[stackElements->data.root.resolvedRulesCount].type);
                            break;
                        case PARSED_JSON_ARRAY_VALUE:
                        case PARSED_JSON_ROOT:
                        case PARSED_JSON_JOINED_OBJECT:
                        case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        case PARSED_JSON_FIELD_WITH_OBJECT:
                        case PARSED_JSON_RESOLVED_FIELD:
                        case PARSED_JSON_VALUE:
                            puts("(rule type) out FIELD???");
                            break;
                        case NONE:
                        case HTTP_REQUEST_METHOD:
                        case HTTP_REQUEST_URI:
                        case HTTP_REQUEST_GET_QUERY_ELEMENT:
                        case HTTP_REQUEST_VALUE:
                        case HTTP_REQUEST_HTTP_VERSION:
                        case HTTP_REQUEST_HEADER:
                        case JSONPATH_REQUEST_ROOT:
                        case JSONPATH_REQUEST_ANY:
                        case JSONPATH_REQUEST_ANYINDEX:
                        case JSONPATH_REQUEST_NAME:
                        case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                        case JSONPATH_REQUEST_INDEX:
                        case JSONPATH_REQUEST_RECURSIVE:
                        case JSONPATH_REQUEST_OBJECT:
                        case JSONPATH_REQUEST_ARRAY:
                        case FINAL_ON_SUCCESS_CALLBACK:
                        case ON_START_CALLBACK:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case JSONPATH_REQUEST_ANY:
                    switch (type) {
                        case PARSED_JSON_FIELD:

                            /* Do NOTHING */

                            /* The next possible field is also good. */

                            /*if (0 < stackElements->data.root.resolvedRulesCount) {
                                --(stackElements->data.root.resolvedRulesCount);
                            } else {
                                puts("ERROR 18");
                                return LE_INCORRECT_INPUT_VALUES;
                            }
                            printf("Rollback rule type 2: %d (resolved any field out)\n", stackElements[stackElements->data.root.resolvedRulesCount].type);*/
                            break;
                        case PARSED_JSON_OBJECT:
                            if (0 < stackElements->data.root.resolvedRulesCount) {
                                /* Exit from JSONPATH_REQUEST_ANY */
                                --(stackElements->data.root.resolvedRulesCount);
                                /* Exit from JSONPATH_REQUEST_OBJECT */
                                --(stackElements->data.root.resolvedRulesCount);
                            } else {
                                puts("ERROR 17");
                                return LE_INCORRECT_INPUT_VALUES;
                            }
                            printf("Rollback rule type 2: %d (object out at field rule)\n", stackElements[stackElements->data.root.resolvedRulesCount].type);
                            break;
                        case PARSED_JSON_ROOT:
                        case PARSED_JSON_JOINED_OBJECT:
                        case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                        case PARSED_JSON_ARRAY:
                        case PARSED_JSON_ARRAY_VALUE:
                        case PARSED_JSON_FIELD_WITH_OBJECT:
                        case PARSED_JSON_RESOLVED_FIELD:
                        case PARSED_JSON_VALUE:
                            puts("(rule type) out ANY FIELD???");
                            break;
                        case NONE:
                        case HTTP_REQUEST_METHOD:
                        case HTTP_REQUEST_URI:
                        case HTTP_REQUEST_GET_QUERY_ELEMENT:
                        case HTTP_REQUEST_VALUE:
                        case HTTP_REQUEST_HTTP_VERSION:
                        case HTTP_REQUEST_HEADER:
                        case JSONPATH_REQUEST_ROOT:
                        case JSONPATH_REQUEST_ANY:
                        case JSONPATH_REQUEST_ANYINDEX:
                        case JSONPATH_REQUEST_NAME:
                        case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                        case JSONPATH_REQUEST_INDEX:
                        case JSONPATH_REQUEST_RECURSIVE:
                        case JSONPATH_REQUEST_OBJECT:
                        case JSONPATH_REQUEST_ARRAY:
                        case FINAL_ON_SUCCESS_CALLBACK:
                        case ON_START_CALLBACK:
                        default:
                            return LE_INCORRECT_INPUT_VALUES;
                    }
                    break;
                case NONE:
                case HTTP_REQUEST_METHOD:
                case HTTP_REQUEST_URI:
                case HTTP_REQUEST_GET_QUERY_ELEMENT:
                case HTTP_REQUEST_VALUE:
                case HTTP_REQUEST_HTTP_VERSION:
                case HTTP_REQUEST_HEADER:
                case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                case JSONPATH_REQUEST_RECURSIVE:
                case PARSED_JSON_ROOT:
                case PARSED_JSON_OBJECT:
                case PARSED_JSON_JOINED_OBJECT:
                case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                case PARSED_JSON_ARRAY:
                case PARSED_JSON_ARRAY_VALUE:
                case PARSED_JSON_FIELD:
                case PARSED_JSON_FIELD_WITH_OBJECT:
                case PARSED_JSON_RESOLVED_FIELD:
                case PARSED_JSON_VALUE:
                case FINAL_ON_SUCCESS_CALLBACK:
                case ON_START_CALLBACK:
                default:
                    printf("wtf2 %d\n", currRule->type);
                    return LE_INCORRECT_INPUT_VALUES;
            }
        }

        if (lastStackElement == stackElements->data.root.alreadyFailed) {
            /* stackElements->data.root.alreadyFailed = NULL; */
            /* Return complex object */
            /*
            if ((ruleSize  == stackElements->data.root.resolvedRulesCount)) {
                string s;
                s.data = lastStackElement->frameStartPosition;
                s.length = lastStackElement->frameLength;
                (stackElements->data.root.callback.handler)(&s, stackElements->data.root.callback.data);
            }*/
        }

        /*if ((NULL == stackElements->data.root.alreadyFailed) && (ruleSize  == stackElements->data.root.resolvedRulesCount)) {
            string s;
            s.data = lastStackElement->frameStartPosition;
            s.length = lastStackElement->frameLength;
            (stackElements->data.root.callback.handler)(&s, stackElements->data.root.callback.data);
        }*/

        stackElements = &(stackElements[ruleSize]);
    }

    /*if (PARSED_JSON_ARRAY_VALUE == (&(jsonRequest->elements[jsonRequest->elementsCount + (--(jsonRequest->parsedStackSize))]))->type) {
        (&(jsonRequest->elements[jsonRequest->elementsCount + ((jsonRequest->parsedStackSize))]))->vType = UNKNOWN;
        (&(jsonRequest->elements[jsonRequest->elementsCount + (--(jsonRequest->parsedStackSize))]))->type = NONE;
    }*/

    (&(jsonRequest->elements[jsonRequest->elementsCount + ((jsonRequest->parsedStackSize))]))->vType = UNKNOWN;
    (&(jsonRequest->elements[jsonRequest->elementsCount + (--(jsonRequest->parsedStackSize))]))->type = NONE;
    puts("CLOSE_FRAME");
    return LE_OK;
}

lemonError fixFieldName(httpRequest *jsonRequest, const size_t length) {
    requestElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);
    requestElement *stackElements = jsonRequest->elements;

    if (PARSED_JSON_FIELD == currentElement->type) {

        currentElement->data.name.length = length;
        currentElement->fType = USELESS;

        while (JSONPATH_REQUEST_ROOT == stackElements->type) { /* Be carefull */
            const size_t ruleSize = stackElements->data.root.ruleSize;
            requestElement *currRule = stackElements->data.root.resolvedRulesCount < ruleSize ? &(stackElements[stackElements->data.root.resolvedRulesCount]) : &(stackElements[ruleSize - 1]);

            if (NULL == stackElements->data.root.alreadyFailed) {
                switch (currRule->type) {
                    case JSONPATH_REQUEST_NAME:
                        if (
                                (PARSED_JSON_FIELD == currentElement->type) &&
                                (currRule->data.name.length == currentElement->data.name.length) &&
                                (0 == STRNCASECMP(currentElement->data.name.data, currRule->data.name.data,
                                                  currentElement->data.name.length))
                                ) {
                            ++(stackElements->data.root.resolvedRulesCount);
                            if ((ruleSize - 1 == stackElements->data.root.resolvedRulesCount)) {
                                currentElement->fType = REQUIRED;
                            };
                            printf("Next rule type 9: %d (field matched)\n", stackElements[stackElements->data.root.resolvedRulesCount].type);
                        } else {
                            stackElements->data.root.alreadyFailed = currentElement;
                            printf("  LOCK FRAME: ruleLine = %p, frame = %p, FIX_FIELD_NAME\n", stackElements, currentElement);
                        }
                        break;
                    case JSONPATH_REQUEST_ANY:
                        ++(stackElements->data.root.resolvedRulesCount);
                        if ((ruleSize - 1 == stackElements->data.root.resolvedRulesCount)) {
                            currentElement->fType = REQUIRED;
                        };
                        printf("Next rule type: %d\n", stackElements[stackElements->data.root.resolvedRulesCount].type);
                        break;

                    case NONE:
                    case HTTP_REQUEST_METHOD:
                    case HTTP_REQUEST_URI:
                    case HTTP_REQUEST_GET_QUERY_ELEMENT:
                    case HTTP_REQUEST_VALUE:
                    case HTTP_REQUEST_HTTP_VERSION:
                    case HTTP_REQUEST_HEADER:
                    case JSONPATH_REQUEST_ROOT:
                    case JSONPATH_REQUEST_ANYINDEX:
                    case JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY:
                    case JSONPATH_REQUEST_INDEX:
                    case JSONPATH_REQUEST_RECURSIVE:
                    case JSONPATH_REQUEST_OBJECT:
                    case JSONPATH_REQUEST_ARRAY:
                    case PARSED_JSON_ROOT:
                    case PARSED_JSON_OBJECT:
                    case PARSED_JSON_JOINED_OBJECT:
                    case PARSED_JSON_HEAD_OF_JOINED_OBJECT:
                    case PARSED_JSON_ARRAY:
                    case PARSED_JSON_ARRAY_VALUE:
                    case PARSED_JSON_FIELD:
                    case PARSED_JSON_FIELD_WITH_OBJECT:
                    case PARSED_JSON_RESOLVED_FIELD:
                    case PARSED_JSON_VALUE:
                    case FINAL_ON_SUCCESS_CALLBACK:
                    case ON_START_CALLBACK:
                    default:
                        printf("wtf222 %d\n", currRule->type);
                        return LE_INCORRECT_INPUT_VALUES;
                }
            }
            stackElements = &(stackElements[ruleSize]);
        }
    } else {
        return LE_PARSING_IS_FAILED;
    };

    /* requestElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);

    if (PARSED_JSON_FIELD == currentElement->type) {
        currentElement->data.name.length = length;
        currentElement->fType = USELESS;
    } else {
        return LE_PARSING_IS_FAILED;
    }; */

    return LE_OK;
}

lemonError openKey(httpRequest *jsonRequest, const char *key) {
    requestElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);

    if (PARSED_JSON_FIELD == currentElement->type) {
        currentElement->data.name.data = key;
        currentElement->fType = USELESS;
        currentElement->vType = UNKNOWN;
    } else {
        return LE_PARSING_IS_FAILED;
    };

    return LE_OK;
}

lemonError openValue(httpRequest *jsonRequest, const char *value) {
    requestElement *currentElement = &(jsonRequest->elements[jsonRequest->elementsCount + jsonRequest->parsedStackSize - 1]);

    if (PARSED_JSON_FIELD == currentElement->type) {
        currentElement->frameStartPosition = value;
    } else {
        return LE_PARSING_IS_FAILED;
    };

    return LE_OK;
}
