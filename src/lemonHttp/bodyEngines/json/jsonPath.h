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

#ifndef LEMONSERVER_JSONPATH_H
#define LEMONSERVER_JSONPATH_H

#include "../../string.h"
#include "../../httpRequest.h"
#include "../../lemonError.h"
#include "./jsonPathQueryBuffer.h"

typedef enum {
    ROOT, /* 0 */
    ANY, /* 1 */
    ANYINDEX, /* 2 */
    NAME, /* 3 */
    NAME_WITH_OBJECT_OR_ARRAY, /* 4 */
    INDEX, /* 5 */
    RECURSIVE, /* 6 */
    PARSED_ROOT, /* 7 */
    PARSED_OBJECT, /* 8 */
    JOINED_OBJECT, /* 9 */
    HEAD_OF_JOINED_OBJECT, /* 10 */
    PARSED_INDEX, /* 11 */
    PARSED_FIELD, /* 12 */
    PARSED_FIELD_WITH_OBJECT, /* 13 */
    RESOLVED_FIELD /* 14 */,
    /* PARSED_VALUE,
    PARSED_ANY,
    PARSED_ANYINDEX,
    PARSED_NAME,
    PARSED_INDEX,
    PARSED_RECURSIVE, */
    NONE
} ruleType;

typedef void changingData;
typedef const lemonError (*jsonPathExecutionHandler)(const string *value, changingData *data);

typedef struct {
    jsonPathExecutionHandler handler;
    changingData *data;
} jsonPathCallback;

/*typedef struct jsonPathElement {
    jsonPathCallback callback;
    string value;
    size_t level;
    size_t index;
    size_t realRootSize;
    struct jsonPathElement *next;
    struct jsonPathElement *recursiveRoot;
    struct jsonPathElement *recursivePrevious;
    char *containerStartPosition;
    ruleType type;
} jsonPathElement;*/

typedef struct {
    jsonPathCallback callback;
    size_t ruleSize;
} rootRule;

typedef struct {
    char *containerStartPosition;
    size_t index;
} indexRule;

typedef struct {
    union {
        rootRule root;
        string name;
        indexRule index;
        char *containerStartPosition;
    } data;
    ruleType type;
} jsonPathElement;

typedef struct {
    jsonPathElement elements[MAX_ELEMENTS];
    size_t elementsCount;
    size_t parsedStackSize;
} jsonPathRequest;

const lemonError initJsonPathRequest(jsonPathRequest *r);

const lemonError appendJsonPathRequest(jsonPathRequest *p, jsonPathQueryBuffer *b, jsonPathExecutionHandler handler, changingData *data);

#endif /* LEMONSERVER_JSONPATH_H */
