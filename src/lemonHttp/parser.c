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


#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "httpRequest.h"
#include "../boolean.h"

#include "http11Lemon.h"
#include "http11Lemon.c"

static lemonError appendHttpToParser(parserState* ps, httpRequest *http) {
    if ((NULL == ps) || (NULL == http)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    ps->container.httpRequest = http;
    return LE_OK;
}

static lemonError appendJsonPathToParser(parserState* ps, httpRequest *http) {
    if ((NULL == ps) || (NULL == http)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    ps->container.httpRequest = http;
    ps->container.jsonPath = NULL; /* Not required for parser */
    return LE_OK;
}

boolean isParsed(const parserState* ps) {
    if ((NULL == ps)) {
        return FALSE;
    }
    return ps->isParsed;
}

static boolean isParseFailed(const parserState* ps) {
    if ((NULL == ps)) {
        return FALSE;
    }
    return ps->isParseFailed;
}

static boolean isSyntaxIncorrect(const parserState* ps) {
    if ((NULL == ps)) {
        return FALSE;
    }
    return ps->isSyntaxIncorrect;
}

lemonError markAsParsed(parserState* ps) {
    if ((NULL == ps)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    ps->isParsed = TRUE;
    return LE_OK;
}

lemonError markAsParseFailed(parserState* ps) {
    if ((NULL == ps)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /*if (0 >= ps->request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }*/
    ps->isParseFailed = TRUE;
    return LE_OK;
}

lemonError markAsSyntaxIncorrect(parserState* ps) {
    if ((NULL == ps)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    /*if (0 >= ps->request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }*/
    ps->isSyntaxIncorrect = TRUE;
    return LE_OK;
}

static lemonError parse(dataContainer container, const parsingMode mode) {
    /* if (NULL == request) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (0 >= request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }*/
    {
        yyParser pParser;
        const unsigned char ascii[256] = {
                /* OK */
                /* JSONPATH_NULL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, */
                /* JSON_NULL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL */
                TOK_NULL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL,

                /* OK */
                /* JSONPATH_BACKSPACE, JSONPATH_CHARTAB, JSONPATH_LINEFEED, JSONPATH_CONTROL, JSONPATH_FORMFEED, JSONPATH_CARRETURN, JSONPATH_CONTROL, JSONPATH_CONTROL, */
                /*-JSON_BACKSPACE, -JSON_CHARTAB, -JSON_LINEFEED, JSON_CONTROL, -JSON_FORMFEED, -JSON_CARRETURN, JSON_CONTROL, JSON_CONTROL,*/
                TOK_BACKSPACE, TOK_HTAB, TOK_CLF, TOK_CONTROL, TOK_FORMFEED, TOK_CR, TOK_CONTROL, TOK_CONTROL,

                /* OK */
                /* JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, */
                /* JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, */
                TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL,

                /* OK */
                /* JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, JSONPATH_CONTROL, */
                /* JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, JSON_CONTROL, */
                TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL,

                /* JSONPATH_SYM, JSONPATH_SYM, JSONPATH_QUOTATION, JSONPATH_SYM, JSONPATH_DOLLAR, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_APOSTROPHE, */
                /* JSON_SP, JSON_SYM, JSON_QUOTATION, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, */
                TOK_SP, TOK_EXCLAMATION, TOK_QUOTATION, TOK_OCTOTHORPE, TOK_DOLLAR, TOK_PERCENT, TOK_AMPERSAND, TOK_APOSTROPHE,

                /* JSONPATH_SYM, JSONPATH_SYM, JSONPATH_ASTERISK, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_DOT, JSONPATH_SLASH, */
                /* JSON_SYM, JSON_SYM, JSON_SYM, JSON_PLUS, JSON_COMMA, JSON_MINUS, JSON_DOT, JSON_SLASH, */
                TOK_LPARENTHESIS, TOK_RPARENTHESIS, TOK_ASTERISK, TOK_PLUS, TOK_COMMA, TOK_MINUS, TOK_DOT, TOK_SLASH,

                /* OK */
                /* JSONPATH_ZERO, JSONPATH_ONE, JSONPATH_TWO, JSONPATH_THREE, JSONPATH_FOUR, JSONPATH_FIVE, JSONPATH_SIX, JSONPATH_SEVEN, */
                /* JSON_ZERO, JSON_ONE, JSON_TWO, JSON_THREE, JSON_FOUR, JSON_FIVE, JSON_SIX, JSON_SEVEN, */
                TOK_ZERO, TOK_ONE, TOK_TWO, TOK_THREE, TOK_FOUR, TOK_FIVE, TOK_SIX, TOK_SEVEN,

                /* JSONPATH_EIGHT, JSONPATH_NINE, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, */
                /* JSON_EIGHT, JSON_NINE, JSON_COLON, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, */
                TOK_EIGHT, TOK_NINE, TOK_COLON, TOK_SEMICOLON, TOK_LESSTHAN, TOK_EQUALS, TOK_GREATERTHAN, TOK_QUESTION,

                /* JSONPATH_SYM, JSONPATH_A, JSONPATH_B, JSONPATH_C, JSONPATH_D, JSONPATH_E, JSONPATH_F, JSONPATH_SYM, */
                /* JSON_SYM, JSON_A, JSON_B, JSON_C, JSON_D, JSON_E, JSON_F, JSON_SYM, */
                TOK_AT, TOK_A, TOK_B, TOK_C, TOK_D, TOK_E, TOK_F, TOK_SYM,

                /* JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, */
                /* JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, */
                TOK_H, TOK_SYM, TOK_SYM, TOK_SYM, TOK_SYM, TOK_SYM, TOK_SYM, TOK_SYM,

                /* JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, */
                /* JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, */
                TOK_P, TOK_SYM, TOK_SYM, TOK_SYM, TOK_T, TOK_SYM, TOK_SYM, TOK_SYM,

                /* JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_LBRACKET, JSONPATH_BACKSLASH, JSONPATH_RBRACKET, JSONPATH_SYM, JSONPATH_SYM, */
                /* JSON_SYM, JSON_SYM, JSON_SYM, JSON_LBRACKET, JSON_BACKSLASH, JSON_RBRACKET, JSON_SYM, JSON_SYM, */
                TOK_SYM, TOK_SYM, TOK_SYM, TOK_LBRACKET, TOK_BACKSLASH, TOK_RBRACKET, TOK_CARET, TOK_UNDERSCORE,

                /* JSONPATH_SYM, JSONPATH_LA, JSONPATH_LB, JSONPATH_LC, JSONPATH_LD, JSONPATH_LE, JSONPATH_LF, JSONPATH_SYM, */
                /* JSON_SYM, JSON_LA, JSON_LB, JSON_LC, JSON_LD, JSON_LE, JSON_LF, JSON_SYM, */
                TOK_BACKQUOTE, TOK_LA, TOK_LB, TOK_LC, TOK_LD, TOK_LE, TOK_LF, TOK_SYM,

                /* JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, */
                /* JSON_SYM, JSON_SYM, JSON_SYM, JSON_SYM, JSON_LL, JSON_SYM, JSON_LN, JSON_SYM, */
                TOK_SYM, TOK_SYM, TOK_SYM, TOK_SYM, TOK_LL, TOK_SYM, TOK_LN, TOK_SYM,

                /* JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_LU, JSONPATH_SYM, JSONPATH_SYM, */
                /* JSON_SYM, JSON_SYM, JSON_LR, JSON_LS, JSON_LT, JSON_LU, JSON_SYM, JSON_SYM, */
                TOK_SYM, TOK_SYM, TOK_LR, TOK_LS, TOK_LT, TOK_LU, TOK_SYM, TOK_SYM,

                /* JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_SYM, JSONPATH_CONTROL, */
                /* JSON_SYM, JSON_SYM, JSON_SYM, JSON_LBRACE, JSON_SYM, JSON_RBRACE, JSON_SYM, JSON_CONTROL, */
                TOK_SYM, TOK_SYM, TOK_SYM, TOK_LBRACE, TOK_VBAR, TOK_RBRACE, TOK_TILDE, TOK_CONTROL,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT,
                TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT, TOK_OBSTEXT
        };
        size_t pos = 0;

        parserState ps;
        char *buffer;

        ParseHTTP11Init(&pParser);
        ParseHTTP11Trace(stdout, "parser >>");

        switch (mode) {
            case GENERAL_HTTP:
                {
                    const lemonError ret = appendHttpToParser(&ps, container.httpRequest);
                    if (LE_OK != ret) {
                        return ret;
                    }
                }
                ParseHTTP11(&pParser, TOK_ONE, 0, &ps);
                buffer = container.httpRequest->privateBuffer;
                break;
            case JSON_BODY:
                {
                    const lemonError ret = appendHttpToParser(&ps, container.httpRequest);
                    if (LE_OK != ret) {
                        return ret;
                    }
                }
                ParseHTTP11(&pParser, TOK_TWO, 0, &ps);
                buffer = container.httpRequest->privateBuffer;
                break;
            case JSON_PATH:
                {
                    const lemonError ret = appendJsonPathToParser(&ps, container.httpRequest);
                    if (LE_OK != ret) {
                        return ret;
                    }
                }
                ParseHTTP11(&pParser, TOK_THREE, 0, &ps);
                buffer = container.jsonPath;
                break;
            default:
                assert(0);
                break;
        }

        ps.isParsed = ps.isParseFailed = ps.isSyntaxIncorrect = FALSE;
        ps.container = container;
        while (
                (FALSE == isParsed(&ps)) &&
                (FALSE == isParseFailed(&ps)) &&
                (FALSE == isSyntaxIncorrect(&ps))
                ) {
            ParseHTTP11(&pParser, ascii[(buffer)[pos]], &((buffer)[pos]), &ps);
            ++pos;
        }

        ParseHTTP11(&pParser, 0, NULL, &ps);

        /* --pos;  Because of last JSONPATH_REQUEST_ANY
        container.httpRequest->body.data = &((container.httpRequest->privateBuffer)[pos]);
        container.httpRequest->body.length -= pos;*/

        return (FALSE == isParseFailed(&ps)) ? ((FALSE == isSyntaxIncorrect(&ps)) ? LE_OK : LE_INCORRECT_SYNTAX) : LE_PARSING_IS_FAILED;
    }
}

lemonError parseHTTP(httpRequest *request) {
    dataContainer container;
    container.httpRequest = request;
    return parse(container, GENERAL_HTTP);
}

lemonError parseJSONPath(httpRequest *jsonPathRequest, char *jsonPath) {
    dataContainer container;
    container.httpRequest = jsonPathRequest;
    container.jsonPath = jsonPath;
    return parse(container, JSON_PATH);
}

lemonError parseJSON(httpRequest *request) {
    dataContainer container;
    container.httpRequest = request;
    return parse(container, JSON_BODY);
}
