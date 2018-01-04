/*
 * Copyright (C) 2017, 2018 Parkhomenko Stanislav
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

#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

#include "http_request.h"
#include "../boolean.h"
#include "lemonHttpError.h"

typedef struct {
    requestElement tmpElement;
    size_t length;
    httpRequest *request;
    linkedDataString *link;
    boolean isParsed;
    boolean isParseFailed;
    boolean isSyntaxIncorrect;
} parserState;

const lemonHttpError parse(httpRequest *request);

const boolean isParsed(const parserState* ps);

const lemonHttpError markAsParsed(parserState* ps);

const lemonHttpError markAsParseFailed(parserState* ps);

const lemonHttpError markAsSyntaxIncorrect(parserState* ps);

#endif /* PARSER_H */

