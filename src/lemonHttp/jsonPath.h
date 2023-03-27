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

#ifndef LEMONSERVER_JSONPATH_H
#define LEMONSERVER_JSONPATH_H

#include "../../string.h"
#include "../../httpRequest.h"
#include "../../lemonError.h"
#include "./jsonPathQueryBuffer.h"

typedef enum {
    _JSONPATH_REQUEST_ROOT, /* 0 */
    _JSONPATH_REQUEST_ANY, /* 1 */
    _JSONPATH_REQUEST_ANYINDEX, /* 2 */
    _JSONPATH_REQUEST_NAME, /* 3 */
    _JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY, /* 4 */
    _JSONPATH_REQUEST_INDEX, /* 5 */
    _JSONPATH_REQUEST_RECURSIVE, /* 6 */
    _PARSED_JSON_ROOT, /* 7 */
    _PARSED_JSON_OBJECT, /* 8 */
    _PARSED_JSON_JOINED_OBJECT, /* 9 */
    _PARSED_JSON_HEAD_OF_JOINED_OBJECT, /* 10 */
    _PARSED_JSON_INDEX, /* 11 */
    _PARSED_JSON_FIELD, /* 12 */
    _PARSED_JSON_FIELD_WITH_OBJECT, /* 13 */
    _PARSED_JSON_RESOLVED_FIELD /* 14 */,
    _NONE
} _ruleType;

typedef void changingData;
typedef const lemonError (*jsonPathExecutionHandler)(const string *value, changingData *data);

typedef struct {
    jsonPathExecutionHandler handler;
    changingData *data;
} _jsonPathCallback;

typedef struct {
    jsonPathCallback callback;
    size_t ruleSize;
} _rootRule;

typedef struct {
    char *containerStartPosition;
    size_t index;
} _indexRule;

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
