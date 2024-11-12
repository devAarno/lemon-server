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

#ifndef LEMONSERVER_JSONPATHINTERNAL_H
#define LEMONSERVER_JSONPATHINTERNAL_H

#include "./jsonPath.h"
#include "lemonError.h"
#include "rules.h"

lemonError appendJsonPathElementOfHttpRequest(httpRequest *r, const string *s, const ruleType type);

lemonError executeJsonPathCallbackWithValue(httpRequest *jsonRequest, const string *s, const boolean isComplex);

lemonError updateJsonPathRequestStatusByArrayElement(httpRequest *jsonRequest, const char *position);

lemonError updateJsonPathRequestStatusByZeroElement(httpRequest *jsonRequest, const char *position);

lemonError openFrame(httpRequest *jsonRequest, const char *position, const ruleType type);

lemonError setFrameLength(httpRequest *jsonRequest, const size_t length);

lemonError setFrameString(httpRequest *jsonRequest, const size_t length);

lemonError setNull(httpRequest *jsonRequest);

lemonError setFalse(httpRequest *jsonRequest);

lemonError setTrue(httpRequest *jsonRequest);

lemonError closeFrame(httpRequest *jsonRequest, const char *position);

lemonError fixFieldName(httpRequest *jsonRequest, const size_t length);

lemonError openKey(httpRequest *jsonRequest, const char *key);

lemonError openValue(httpRequest *jsonRequest, const char *value);

#endif /* LEMONSERVER_JSONPATHINTERNAL_H */
