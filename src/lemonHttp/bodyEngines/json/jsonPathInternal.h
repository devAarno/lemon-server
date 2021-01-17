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

#ifndef LEMONSERVER_JSONPATHINTERNAL_H
#define LEMONSERVER_JSONPATHINTERNAL_H

#include "./jsonPath.h"
#include "../../lemonError.h"

jsonPathElement *appendJsonPathElementOfHttpRequest(jsonPathRequest *r, const string *s, const ruleType type);

const string convertUtf16ToString(char *c1, char *c2, const char c3, const char c4);

const lemonError updateJsonPathRequestStatusByFieldName(jsonPathRequest *jsonRequest, const string *key);

const lemonError rollbackJsonPathRequestStatusByFieldName(jsonPathRequest *jsonRequest, const string *key);

const lemonError updateJsonPathRequestStatusByObject(jsonPathRequest *jsonRequest, const char *startObjectPosition);

const lemonError rollbackJsonPathRequestStatusByObject(jsonPathRequest *jsonRequest, const char *endObjectPosition);

const lemonError updateJsonPathRequestStatusByArray(jsonPathRequest *jsonRequest, const char *startArrayPosition);

const lemonError rollbackJsonPathRequestStatusByArray(jsonPathRequest *jsonRequest, const char *endArrayPosition);

const lemonError executeJsonPathCallbackWithValue(jsonPathRequest *jsonRequest, const string *s);

const lemonError executeJsonPathCallbackByObject(jsonPathRequest *jsonRequest);

const lemonError updateJsonPathRequestStatusByArrayElement(jsonPathRequest *jsonRequest);

const lemonError updateJsonPathRequestStatusByRoot(jsonPathRequest *jsonRequest);

const lemonError rollbackJsonPathRequestStatusByRoot(jsonPathRequest *jsonRequest);

const lemonError executeJsonPathCallbackByRoot(jsonPathRequest *jsonRequest);

#endif /* LEMONSERVER_JSONPATHINTERNAL_H */
