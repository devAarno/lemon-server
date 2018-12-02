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


#include "jsonParser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../httpRequest.h"
#include "../../../boolean.h"

#include "json.h"
#include "json.c"

const static lemonError appendHttpToParser(jsonParserState* ps, httpRequest *http) {
    if ((NULL == ps) || (NULL == http)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    ps->request = http;
    return LE_OK;
}

const boolean isJSONParsed(const jsonParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return FALSE;
    }
    return ps->isParsed;
}

const static boolean isParseFailed(const jsonParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return FALSE;
    }
    return ps->isParseFailed;
}

const static boolean isSyntaxIncorrect(const jsonParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return FALSE;
    }
    return ps->isSyntaxIncorrect;
}

const lemonError markAsJSONParsed(jsonParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    ps->isParsed = TRUE;
    return LE_OK;
}

const lemonError markAsJSONParseFailed(jsonParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (0 >= ps->request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    ps->isParseFailed = TRUE;
    return LE_OK;
}

const lemonError markAsJSONIncorrect(jsonParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (0 >= ps->request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    ps->isSyntaxIncorrect = TRUE;
    return LE_OK;
}

const lemonError parseJSON(httpRequest *request) {
    if (NULL == request) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (0 >= request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    {
        char pParser[sizeof(yyParser)];
        const unsigned char ascii[256] = {
            JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL,
            JSON_BACKSPACE, JSON_CHARTAB, JSON_LINEFEED, JSON_CONTROL, JSON_FORMFEED, JSON_CARRETURN, JSON_CONTROL, JSON_CONTROL,
            JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL,
            JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL,
            JSON_SP, JSON_SYM, JSON_QUOTATION, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM,
            JSON_SYM, JSON_SYM, JSON_SYM, JSON_PLUS, JSON_COMMA, JSON_MINUS, JSON_DOT, JSON_SLASH,
            JSON_ZERO, JSON_ONE, JSON_TWO, JSON_THREE, JSON_FOUR, JSON_FIVE, JSON_SIX, JSON_SEVEN,
            JSON_EIGHT, JSON_NINE, JSON_COLON, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM,
            JSON_SYM, JSON_A, JSON_B, JSON_C, JSON_D, JSON_E, JSON_F, JSON_SYM,
            JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM,
            JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM,
            JSON_SYM, JSON_SYM, JSON_SYM, JSON_LBRACKET, JSON_BACKSLASH, JSON_RBRACKET, JSON_SYM, JSON_SYM,
            JSON_SYM, JSON_LA, JSON_LB, JSON_LC, JSON_LD, JSON_LE, JSON_LF, JSON_SYM,
            JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_LL, JSON_SYM, JSON_LN, JSON_SYM,
            JSON_SYM, JSON_SYM, JSON_LR, JSON_LS, JSON_LT, JSON_LU, JSON_SYM, JSON_SYM,
            JSON_SYM, JSON_SYM, JSON_SYM, JSON_LBRACE, JSON_SYM, JSON_RBRACE, JSON_SYM, JSON_CONTROL,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT,
            JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT, JSON_EXT
        };
        size_t pos = 0;

        jsonParserState ps;

        {
            const lemonError ret = appendHttpToParser(&ps, request);
            if (LE_OK != ret) {
                return ret;
            }
        }
        
        ParseJSONInit(&pParser);
        ParseJSONTrace(stdout, "parser >>");

        ps.isParsed = ps.isParseFailed = ps.isSyntaxIncorrect = FALSE;
        while (
                (FALSE == isParsed(&ps)) &&
                (FALSE == isParseFailed(&ps)) &&
                (FALSE == isSyntaxIncorrect(&ps))
                ) {
            ParseJSON(&pParser, ascii[(request->privateBuffer)[pos]], &((request->privateBuffer)[pos]), &ps);
            ++pos;
        }

        ParseJSON(&pParser, 0, NULL, &ps);

        --pos; /* Because of last ANY */
        request->body.data = &((request->privateBuffer)[pos]);
        request->body.length -= pos;

        return (FALSE == isParseFailed(&ps)) ? ((FALSE == isSyntaxIncorrect(&ps)) ? LE_OK : LE_INCORRECT_SYNTAX) : LE_PARSING_IS_FAILED;
    }
}
