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


#include "jsonParser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../httpRequest.h"
#include "../../../boolean.h"

#include "jsonLemon.h"
#include "jsonLemon.c"

const static lemonError appendAdditionalInfoToParser(jsonParserState* ps, httpRequest *http, jsonPathRequest *jsonRequest) {
    if ((NULL == ps) || (NULL == http) || (NULL == jsonRequest)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if ((0 >= http->elementsCount) || (0 >= jsonRequest->elementsCount)) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    ps->request = http;
    ps->jsonRequest = jsonRequest;
    return LE_OK;
}

const boolean isJSONParsed(const jsonParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return FALSE;
    }
    return ps->isParsed;
}

const static boolean isJSONParseFailed(const jsonParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return FALSE;
    }
    return ps->isParseFailed;
}

const static boolean isJSONSyntaxIncorrect(const jsonParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return FALSE;
    }
    return ps->isSyntaxIncorrect;
}

const lemonError markJSONAsParsed(jsonParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    ps->isParsed = TRUE;
    return LE_OK;
}

const lemonError markJSONAsParseFailed(jsonParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (0 >= ps->request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    ps->isParseFailed = TRUE;
    return LE_OK;
}

const lemonError markJSONAsIncorrect(jsonParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (0 >= ps->request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    ps->isSyntaxIncorrect = TRUE;
    return LE_OK;
}

const lemonError parseJSON(httpRequest *request, jsonPathRequest *jsonRequest) {
    if (NULL == request) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (0 >= request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    {
        yyParser pParser;
        const unsigned char ascii[256] = {
            JSON_NULL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL,
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
            const lemonError ret = appendAdditionalInfoToParser(&ps, request, jsonRequest);
            if (LE_OK != ret) {
                return ret;
            }
        }
        
        ParseJSONInit(&pParser);
        ParseJSONTrace(stdout, "parser >>");

        ps.isParsed = ps.isParseFailed = ps.isSyntaxIncorrect = FALSE;
        while (
                (FALSE == isJSONParsed(&ps)) &&
                (FALSE == isJSONParseFailed(&ps)) &&
                (FALSE == isJSONSyntaxIncorrect(&ps)) /*&&
                ('\0' != (request->privateBuffer)[pos])*/
                ) {
            ParseJSON(&pParser, ascii[(request->privateBuffer)[pos]], &((request->privateBuffer)[pos]), &ps);
            ++pos;
        }

        /*ParseJSON(&pParser, 0, NULL, &ps);*/

        /* Because of last ANY */
        /*--pos;
        request->body.data = &((request->privateBuffer)[pos]);
        request->body.length -= pos;*/

        return (TRUE == isJSONParsed(&ps)) ? ((FALSE == isJSONSyntaxIncorrect(&ps)) ? LE_OK : LE_INCORRECT_SYNTAX) : LE_PARSING_IS_FAILED;
    }
}
