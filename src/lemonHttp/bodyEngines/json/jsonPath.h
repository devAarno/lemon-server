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
    ROOT,
    ANY,
    ANYINDEX,
    NAME,
    INDEX,
    RECURSIVE
} ruleType;

typedef void changingData;
typedef const lemonError (*jsonPathExecutionHandler)(const string *value, changingData *data);

typedef struct {
    jsonPathExecutionHandler handler;
    changingData *data;
} jsonPathCallback;

typedef struct jsonPathElement {
    jsonPathCallback callback;
    string value;
    size_t level;
    size_t index;
    struct jsonPathElement *next;
    char *containerStartPosition;
    ruleType type;
} jsonPathElement;

typedef struct {
    jsonPathElement elements[MAX_ELEMENTS];
    size_t elementsCount;
} jsonPathRequest;

const lemonError initJsonPathRequest(jsonPathRequest *r);

const lemonError appendJsonPathRequest(jsonPathRequest *p, jsonPathQueryBuffer *b, jsonPathExecutionHandler handler, changingData *data);

#endif /* LEMONSERVER_JSONPATH_H */
