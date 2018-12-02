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


#include "jsonPathParser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../httpRequest.h"
#include "../../../boolean.h"

#include "jsonPath.h"
#include "jsonPath.c"

const static lemonError appendHttpToParser(jsonPathParserState* ps, httpRequest *http) {
    if ((NULL == ps) || (NULL == http)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    ps->request = http;
    return LE_OK;
}

const boolean isJSONPathParsed(const jsonPathParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return FALSE;
    }
    return ps->isParsed;
}

const static boolean isParseFailed(const jsonPathParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return FALSE;
    }
    return ps->isParseFailed;
}

const static boolean isSyntaxIncorrect(const jsonPathParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return FALSE;
    }
    return ps->isSyntaxIncorrect;
}

const lemonError markJSONPathAsParsed(jsonPathParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    ps->isParsed = TRUE;
    return LE_OK;
}

const lemonError markJSONPathAsParseFailed(jsonPathParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (0 >= ps->request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    ps->isParseFailed = TRUE;
    return LE_OK;
}

const lemonError markJSONPathAsSyntaxIncorrect(jsonPathParserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (0 >= ps->request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    ps->isSyntaxIncorrect = TRUE;
    return LE_OK;
}

const lemonError parseJSONPath(httpRequest *request, char *jsonPath) {
    if (NULL == request) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (0 >= request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    {
        char pParser[sizeof(yyParser)];
        const unsigned char ascii[256] = {
            JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL,
            JSONPATH_BACKSPACE, JSONPATH_CHARTAB, JSONPATH_LINEFEED, JSONPATH_CONTROL, JSONPATH_FORMFEED, JSONPATH_CARRETURN, JSONPATH_CONTROL, JSONPATH_CONTROL,
            JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL,
            JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL,
            JSONPATH_SYM, JSONPATH_SYM, JSONPATH_QUOTATION, JSONPATH_SYM, JSONPATH_DOLLAR, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_APOSTROPHE,
            JSONPATH_SYM, JSONPATH_SYM, JSONPATH_ASTERISK, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_DOT, JSONPATH_SLASH,
            JSONPATH_ZERO, JSONPATH_ONE, JSONPATH_TWO, JSONPATH_THREE, JSONPATH_FOUR, JSONPATH_FIVE, JSONPATH_SIX, JSONPATH_SEVEN,
            JSONPATH_EIGHT, JSONPATH_NINE, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM,
            JSONPATH_SYM, JSONPATH_A, JSONPATH_B, JSONPATH_C, JSONPATH_D, JSONPATH_E, JSONPATH_F, JSONPATH_SYM,
            JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM,
            JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM,
            JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_LBRACKET, JSONPATH_BACKSLASH, JSONPATH_RBRACKET, JSONPATH_SYM, JSONPATH_SYM,
            JSONPATH_SYM, JSONPATH_LA, JSONPATH_LB, JSONPATH_LC, JSONPATH_LD, JSONPATH_LE, JSONPATH_LF, JSONPATH_SYM,
            JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM,
            JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_LU, JSONPATH_SYM, JSONPATH_SYM,
            JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_CONTROL,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT,
            JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT, JSONPATH_EXT
        };
        size_t pos = 0;

        jsonPathParserState ps;

        {
            const lemonError ret = appendHttpToParser(&ps, request);
            if (LE_OK != ret) {
                return ret;
            }
        }
        
        ParseJSONPathInit(&pParser);
        ParseJSONPathTrace(stdout, "parser >>");

        ps.isParsed = ps.isParseFailed = ps.isSyntaxIncorrect = FALSE;
        while (
                (FALSE == isParsed(&ps)) &&
                (FALSE == isParseFailed(&ps)) &&
                (FALSE == isSyntaxIncorrect(&ps)) &&
                (NULL != (jsonPath)[pos])
                ) {
            ParseJSONPath(&pParser, ascii[(jsonPath)[pos]], &((jsonPath)[pos]), &ps);
            ++pos;
        }

        ParseJSONPath(&pParser, 0, NULL, &ps);

        return (FALSE == isParseFailed(&ps)) ? ((FALSE == isSyntaxIncorrect(&ps)) ? LE_OK : LE_INCORRECT_SYNTAX) : LE_PARSING_IS_FAILED;
    }
}
