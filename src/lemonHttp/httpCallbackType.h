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

#ifndef LEMONSERVER_HTTPCALLBACKTYPE_H
#define LEMONSERVER_HTTPCALLBACKTYPE_H

#include "lemonError.h"
#include "string.h"
#include "changingData.h"

/* HTTP */
typedef const lemonError (*httpMethodExecutionHandler)(const string *value, changingData *data);
typedef const lemonError (*httpUriExecutionHandler)(const string *value, changingData *data);
typedef const lemonError (*httpVersionExecutionHandler)(const string *value, changingData *data);
/*typedef char jsonPathQueryBuffer; */
typedef char httpGetParameterQueryBuffer;
typedef const lemonError (*httpGetParameterQueryExecutionHandler)(const string *value, changingData *data);
typedef char httpHeaderQueryBuffer;
typedef const lemonError (*httpHeaderQueryExecutionHandler)(const string *value, changingData *data);

typedef struct {
    httpMethodExecutionHandler handler;
    changingData *data;
} httpMethodCallback;

typedef struct {
    httpUriExecutionHandler handler;
    changingData *data;
} httpUriCallback;

typedef struct {
    httpVersionExecutionHandler handler;
    changingData *data;
} httpVersionCallback;

typedef struct {
    httpGetParameterQueryExecutionHandler handler;
    changingData *data;
    string getParameter;
} httpGetParameterCallback;

typedef struct {
    httpHeaderQueryExecutionHandler handler;
    changingData *data;
    string headerName;
} httpHeaderQueryCallback;

#endif /* LEMONSERVER_HTTPCALLBACKTYPE_H */
