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

#ifndef LEMONSERVER_RULES_H
#define LEMONSERVER_RULES_H

#include "jsonCallbackType.h"

typedef enum {
    NONE,
    HTTP_REQUEST_METHOD,
    HTTP_REQUEST_URI,
    HTTP_REQUEST_GET_QUERY_ELEMENT,
    HTTP_REQUEST_VALUE,
    HTTP_REQUEST_HTTP_VERSION,
    HTTP_REQUEST_HEADER,
    JSONPATH_REQUEST_ROOT,
    JSONPATH_REQUEST_ANY,
    JSONPATH_REQUEST_ANYINDEX,
    JSONPATH_REQUEST_NAME,
    JSON_PATH_REQUEST_NAME_WITH_OBJECT_OR_ARRAY,
    JSONPATH_REQUEST_INDEX,
    JSONPATH_REQUEST_RECURSIVE,
    PARSED_JSON_ROOT,
    PARSED_JSON_OBJECT,
    PARSED_JSON_JOINED_OBJECT,
    PARSED_JSON_HEAD_OF_JOINED_OBJECT,
    PARSED_JSON_INDEX,
    PARSED_JSON_FIELD,
    PARSED_JSON_FIELD_WITH_OBJECT,
    PARSED_JSON_RESOLVED_FIELD,
    FINAL_ON_SUCCESS_CALLBACK,
    ON_START_CALLBACK
} ruleType;

typedef struct {
    jsonPathCallback callback;
    size_t ruleSize;
} rootRule;

typedef struct {
    char *containerStartPosition;
    size_t index;
} indexRule;

#endif /* LEMONSERVER_RULES_H */
