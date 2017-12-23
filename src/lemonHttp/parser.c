/*
 * Copyright (C) 2017 Parkhomenko Stanislav
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
#include "http_request.h"
#include "../boolean.h"
#include "http11.h"

static const unsigned char ascii[256] = {
    [0] = TOK_CONTROL, [1] = TOK_CONTROL, [2] = TOK_CONTROL, [3] = TOK_CONTROL, [4] = TOK_CONTROL, [5] = TOK_CONTROL, [6] = TOK_CONTROL, [7] = TOK_CONTROL,
    [8] = TOK_CONTROL, [9] = TOK_HTAB, [10] = TOK_CLF, [11] = TOK_CONTROL, [12] = TOK_CONTROL, [13] = TOK_CR, [14] = TOK_CONTROL, [15] = TOK_CONTROL,
    [16] = TOK_CONTROL, [17] = TOK_CONTROL, [18] = TOK_CONTROL, [19] = TOK_CONTROL, [20] = TOK_CONTROL, [21] = TOK_CONTROL, [22] = TOK_CONTROL, [23] = TOK_CONTROL,
    [24] = TOK_CONTROL, [25] = TOK_CONTROL, [26] = TOK_CONTROL, [27] = TOK_CONTROL, [28] = TOK_CONTROL, [29] = TOK_CONTROL, [30] = TOK_CONTROL, [31] = TOK_CONTROL,
    [32] = TOK_SP, [33] = TOK_EXCLAMATION, [34] = TOK_QUOTATION, [35] = TOK_OCTOTHORPE, [36] = TOK_DOLLAR, [37] = TOK_PERCENT, [38] = TOK_AMPERSAND, [39] = TOK_APOSTROPHE,
    [40] = TOK_LPARENTHESIS, [41] = TOK_RPARENTHESIS, [42] = TOK_ASTERISK, [43] = TOK_PLUS, [44] = TOK_COMMA, [45] = TOK_MINUS, [46] = TOK_DOT, [47] = TOK_SLASH,
    [48] = TOK_ZERO, [49] = TOK_ONE, [50] = TOK_TWO, [51] = TOK_THREE, [52] = TOK_FOUR, [53] = TOK_FIVE, [54] = TOK_SIX, [55] = TOK_SEVEN,
    [56] = TOK_EIGHT, [57] = TOK_NINE, [58] = TOK_COLON, [59] = TOK_SEMICOLON, [60] = TOK_LESSTHAN, [61] = TOK_EQUALS, [62] = TOK_GREATERTHAN, [63] = TOK_QUESTION,
    [64] = TOK_AT, [65] = TOK_A, [66] = TOK_B, [67] = TOK_C, [68] = TOK_D, [69] = TOK_E, [70] = TOK_F, [71] = TOK_G,
    [72] = TOK_H, [73] = TOK_I, [74] = TOK_J, [75] = TOK_K, [76] = TOK_L, [77] = TOK_M, [78] = TOK_N, [79] = TOK_O,
    [80] = TOK_P, [81] = TOK_Q, [82] = TOK_R, [83] = TOK_S, [84] = TOK_T, [85] = TOK_U, [86] = TOK_V, [87] = TOK_W,
    [88] = TOK_X, [89] = TOK_Y, [90] = TOK_Z, [91] = TOK_LBRACKET, [92] = TOK_BACKSLASH, [93] = TOK_RBRACKET, [94] = TOK_CARET, [95] = TOK_UNDERSCORE,
    [96] = TOK_BACKQUOTE, [97] = TOK_LA, [98] = TOK_LB, [99] = TOK_LC, [100] = TOK_LD, [101] = TOK_LE, [102] = TOK_LF, [103] = TOK_LG,
    [104] = TOK_LH, [105] = TOK_LI, [106] = TOK_LJ, [107] = TOK_LK, [108] = TOK_LL, [109] = TOK_LM, [110] = TOK_LN, [111] = TOK_LO,
    [112] = TOK_LP, [113] = TOK_LQ, [114] = TOK_LR, [115] = TOK_LS, [116] = TOK_LT, [117] = TOK_LU, [118] = TOK_LV, [119] = TOK_LW,
    [120] = TOK_LX, [121] = TOK_LY, [122] = TOK_LZ, [123] = TOK_LBRACE, [124] = TOK_VBAR, [125] = TOK_RBRACE, [126] = TOK_TILDE, [127] = TOK_CONTROL,
    [128] = TOK_OBSTEXT, [129] = TOK_OBSTEXT, [130] = TOK_OBSTEXT, [131] = TOK_OBSTEXT, [132] = TOK_OBSTEXT, [133] = TOK_OBSTEXT, [134] = TOK_OBSTEXT, [135] = TOK_OBSTEXT,
    [136] = TOK_OBSTEXT, [137] = TOK_OBSTEXT, [138] = TOK_OBSTEXT, [139] = TOK_OBSTEXT, [140] = TOK_OBSTEXT, [141] = TOK_OBSTEXT, [142] = TOK_OBSTEXT, [143] = TOK_OBSTEXT,
    [144] = TOK_OBSTEXT, [145] = TOK_OBSTEXT, [146] = TOK_OBSTEXT, [147] = TOK_OBSTEXT, [148] = TOK_OBSTEXT, [149] = TOK_OBSTEXT, [150] = TOK_OBSTEXT, [151] = TOK_OBSTEXT,
    [152] = TOK_OBSTEXT, [153] = TOK_OBSTEXT, [154] = TOK_OBSTEXT, [155] = TOK_OBSTEXT, [156] = TOK_OBSTEXT, [157] = TOK_OBSTEXT, [158] = TOK_OBSTEXT, [159] = TOK_OBSTEXT,
    [160] = TOK_OBSTEXT, [161] = TOK_OBSTEXT, [162] = TOK_OBSTEXT, [163] = TOK_OBSTEXT, [164] = TOK_OBSTEXT, [165] = TOK_OBSTEXT, [166] = TOK_OBSTEXT, [167] = TOK_OBSTEXT,
    [168] = TOK_OBSTEXT, [169] = TOK_OBSTEXT, [170] = TOK_OBSTEXT, [171] = TOK_OBSTEXT, [172] = TOK_OBSTEXT, [173] = TOK_OBSTEXT, [174] = TOK_OBSTEXT, [175] = TOK_OBSTEXT,
    [176] = TOK_OBSTEXT, [177] = TOK_OBSTEXT, [178] = TOK_OBSTEXT, [179] = TOK_OBSTEXT, [180] = TOK_OBSTEXT, [181] = TOK_OBSTEXT, [182] = TOK_OBSTEXT, [183] = TOK_OBSTEXT,
    [184] = TOK_OBSTEXT, [185] = TOK_OBSTEXT, [186] = TOK_OBSTEXT, [187] = TOK_OBSTEXT, [188] = TOK_OBSTEXT, [189] = TOK_OBSTEXT, [190] = TOK_OBSTEXT, [191] = TOK_OBSTEXT,
    [192] = TOK_OBSTEXT, [193] = TOK_OBSTEXT, [194] = TOK_OBSTEXT, [195] = TOK_OBSTEXT, [196] = TOK_OBSTEXT, [197] = TOK_OBSTEXT, [198] = TOK_OBSTEXT, [199] = TOK_OBSTEXT,
    [200] = TOK_OBSTEXT, [201] = TOK_OBSTEXT, [202] = TOK_OBSTEXT, [203] = TOK_OBSTEXT, [204] = TOK_OBSTEXT, [205] = TOK_OBSTEXT, [206] = TOK_OBSTEXT, [207] = TOK_OBSTEXT,
    [208] = TOK_OBSTEXT, [209] = TOK_OBSTEXT, [210] = TOK_OBSTEXT, [211] = TOK_OBSTEXT, [212] = TOK_OBSTEXT, [213] = TOK_OBSTEXT, [214] = TOK_OBSTEXT, [215] = TOK_OBSTEXT,
    [216] = TOK_OBSTEXT, [217] = TOK_OBSTEXT, [218] = TOK_OBSTEXT, [219] = TOK_OBSTEXT, [220] = TOK_OBSTEXT, [221] = TOK_OBSTEXT, [222] = TOK_OBSTEXT, [223] = TOK_OBSTEXT,
    [224] = TOK_OBSTEXT, [225] = TOK_OBSTEXT, [226] = TOK_OBSTEXT, [227] = TOK_OBSTEXT, [228] = TOK_OBSTEXT, [229] = TOK_OBSTEXT, [230] = TOK_OBSTEXT, [231] = TOK_OBSTEXT,
    [232] = TOK_OBSTEXT, [233] = TOK_OBSTEXT, [234] = TOK_OBSTEXT, [235] = TOK_OBSTEXT, [236] = TOK_OBSTEXT, [237] = TOK_OBSTEXT, [238] = TOK_OBSTEXT, [239] = TOK_OBSTEXT,
    [240] = TOK_OBSTEXT, [241] = TOK_OBSTEXT, [242] = TOK_OBSTEXT, [243] = TOK_OBSTEXT, [244] = TOK_OBSTEXT, [245] = TOK_OBSTEXT, [246] = TOK_OBSTEXT, [247] = TOK_OBSTEXT,
    [248] = TOK_OBSTEXT, [249] = TOK_OBSTEXT, [250] = TOK_OBSTEXT, [251] = TOK_OBSTEXT, [252] = TOK_OBSTEXT, [253] = TOK_OBSTEXT, [254] = TOK_OBSTEXT, [255] = TOK_OBSTEXT
};

const static lemonHttpError appendHttpToParser(parserState* ps, httpRequest *http) {
    ps->request = http;
    return OK;
}

const boolean isParsed(const parserState* ps) {
    return ps->isParsed;
}

const static boolean isParseFailed(const parserState* ps) {
    return ps->isParseFailed;
}

const static boolean isSyntaxIncorrect(const parserState* ps) {
    return ps->isSyntaxIncorrect;
}

const lemonHttpError markAsParsed(parserState* ps) {
    ps->isParsed = TRUE;
    return OK;
}

const lemonHttpError markAsParseFailed(parserState* ps) {
    ps->isParseFailed = TRUE;
    return OK;
}

const lemonHttpError markAsSyntaxIncorrect(parserState* ps) {
    ps->isSyntaxIncorrect = TRUE;
    return OK;
}

const lemonHttpError parse(httpRequest *request) {
    char pParser[ParseHTTP11Size()];
    size_t pos = 0;

    parserState ps;

    {
        const lemonHttpError ret = appendHttpToParser(&ps, request);
        if (OK != ret) {
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

    request->body.data = &((request->privateBuffer)[pos]);
    request->body.length -= pos;

    return (FALSE == isParseFailed(&ps)) ? ((FALSE == isSyntaxIncorrect(&ps)) ? OK : INCORRECT_SYNTAX) : PARSING_IS_FAILED;
}
