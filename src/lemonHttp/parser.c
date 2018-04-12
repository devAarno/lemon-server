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


#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "httpRequest.h"
#include "../boolean.h"

#include "http11.h"
#include "http11.c"

static const unsigned char ascii[256] = {
    TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL,
    TOK_CONTROL, TOK_HTAB, TOK_CLF, TOK_CONTROL, TOK_CONTROL, TOK_CR, TOK_CONTROL, TOK_CONTROL,
    TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL,
    TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL, TOK_CONTROL,
    TOK_SP, TOK_EXCLAMATION, TOK_QUOTATION, TOK_OCTOTHORPE, TOK_DOLLAR, TOK_PERCENT, TOK_AMPERSAND, TOK_APOSTROPHE,
    TOK_LPARENTHESIS, TOK_RPARENTHESIS, TOK_ASTERISK, TOK_PLUS, TOK_COMMA, TOK_MINUS, TOK_DOT, TOK_SLASH,
    TOK_ZERO, TOK_ONE, TOK_TWO, TOK_THREE, TOK_FOUR, TOK_FIVE, TOK_SIX, TOK_SEVEN,
    TOK_EIGHT, TOK_NINE, TOK_COLON, TOK_SEMICOLON, TOK_LESSTHAN, TOK_EQUALS, TOK_GREATERTHAN, TOK_QUESTION,
    TOK_AT, TOK_A, TOK_B, TOK_C, TOK_D, TOK_E, TOK_F, TOK_G,
    TOK_H, TOK_I, TOK_J, TOK_K, TOK_L, TOK_M, TOK_N, TOK_O,
    TOK_P, TOK_Q, TOK_R, TOK_S, TOK_T, TOK_U, TOK_V, TOK_W,
    TOK_X, TOK_Y, TOK_Z, TOK_LBRACKET, TOK_BACKSLASH, TOK_RBRACKET, TOK_CARET, TOK_UNDERSCORE,
    TOK_BACKQUOTE, TOK_LA, TOK_LB, TOK_LC, TOK_LD, TOK_LE, TOK_LF, TOK_LG,
    TOK_LH, TOK_LI, TOK_LJ, TOK_LK, TOK_LL, TOK_LM, TOK_LN, TOK_LO,
    TOK_LP, TOK_LQ, TOK_LR, TOK_LS, TOK_LT, TOK_LU, TOK_LV, TOK_LW,
    TOK_LX, TOK_LY, TOK_LZ, TOK_LBRACE, TOK_VBAR, TOK_RBRACE, TOK_TILDE, TOK_CONTROL,
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

const static lemonError appendHttpToParser(parserState* ps, httpRequest *http) {
    if ((NULL == ps) || (NULL == http)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    ps->request = http;
    return LE_OK;
}

const boolean isParsed(const parserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return FALSE;
    }
    return ps->isParsed;
}

const static boolean isParseFailed(const parserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return FALSE;
    }
    return ps->isParseFailed;
}

const static boolean isSyntaxIncorrect(const parserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return FALSE;
    }
    return ps->isSyntaxIncorrect;
}

const lemonError markAsParsed(parserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    ps->isParsed = TRUE;
    return LE_OK;
}

const lemonError markAsParseFailed(parserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (0 >= ps->request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    ps->isParseFailed = TRUE;
    return LE_OK;
}

const lemonError markAsSyntaxIncorrect(parserState* ps) {
    if ((NULL == ps) || (NULL == ps->request)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (0 >= ps->request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    ps->isSyntaxIncorrect = TRUE;
    return LE_OK;
}

const lemonError parse(httpRequest *request) {
    if (NULL == request) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    if (0 >= request->elementsCount) {
        return LE_INCORRECT_INPUT_VALUES;
    }
    {
        char pParser[sizeof(yyParser)];
        size_t pos = 0;

        parserState ps;

        {
            const lemonError ret = appendHttpToParser(&ps, request);
            if (LE_OK != ret) {
                return ret;
            }
        }

        ParseHTTP11Init(&pParser);
        ParseHTTP11Trace(stdout, "parser >>");

        ps.isParsed = ps.isParseFailed = ps.isSyntaxIncorrect = FALSE;
        while (
                (FALSE == isParsed(&ps)) &&
                (FALSE == isParseFailed(&ps)) &&
                (FALSE == isSyntaxIncorrect(&ps))
                ) {
            ParseHTTP11(&pParser, ascii[(request->privateBuffer)[pos]], &((request->privateBuffer)[pos]), &ps);
            ++pos;
        }

        ParseHTTP11(&pParser, 0, NULL, &ps);

        --pos; /* Because of last ANY */
        request->body.data = &((request->privateBuffer)[pos]);
        request->body.length -= pos;

        return (FALSE == isParseFailed(&ps)) ? ((FALSE == isSyntaxIncorrect(&ps)) ? LE_OK : LE_INCORRECT_SYNTAX) : LE_PARSING_IS_FAILED;
    }
}
