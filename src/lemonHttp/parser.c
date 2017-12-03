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

static const unsigned char stop_symbols[256] = {
    [0] = 0, [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0,
    [8] = 0, [9] = TOK_HTAB, [10] = TOK_LF, [11] = 0, [12] = 0, [13] = TOK_CR, [14] = 0, [15] = 0,
    [16] = 0, [17] = 0, [18] = 0, [19] = 0, [20] = 0, [21] = 0, [22] = 0, [23] = 0,
    [24] = 0, [25] = 0, [26] = 0, [27] = 0, [28] = 0, [29] = 0, [30] = 0, [31] = 0,
    [32] = TOK_SP, [33] = 0, [34] = 0, [35] = 0, [36] = 0, [37] = 0, [38] = TOK_AMP, [39] = 0,
    [40] = 0, [41] = 0, [42] = 0, [43] = 0, [44] = 0, [45] = 0, [46] = 0, [47] = 0,
    [48] = 0, [49] = 0, [50] = 0, [51] = 0, [52] = 0, [53] = 0, [54] = 0, [55] = 0,
    [56] = 0, [57] = 0, [58] = TOK_CLN, [59] = 0, [60] = 0, [61] = TOK_EQ, [62] = 0, [63] = TOK_QST,
    [64] = 0, [65] = 0, [66] = 0, [67] = 0, [68] = 0, [69] = 0, [70] = 0, [71] = 0,
    [72] = 0, [73] = 0, [74] = 0, [75] = 0, [76] = 0, [77] = 0, [78] = 0, [79] = 0,
    [80] = 0, [81] = 0, [82] = 0, [83] = 0, [84] = 0, [85] = 0, [86] = 0, [87] = 0,
    [88] = 0, [89] = 0, [90] = 0, [91] = 0, [92] = 0, [93] = 0, [94] = 0, [95] = 0,
    [96] = 0, [97] = 0, [98] = 0, [99] = 0, [100] = 0, [101] = 0, [102] = 0, [103] = 0,
    [104] = 0, [105] = 0, [106] = 0, [107] = 0, [108] = 0, [109] = 0, [110] = 0, [111] = 0,
    [112] = 0, [113] = 0, [114] = 0, [115] = 0, [116] = 0, [117] = 0, [118] = 0, [119] = 0,
    [120] = 0, [121] = 0, [122] = 0, [123] = 0, [124] = 0, [125] = 0, [126] = 0, [127] = 0,
    [128] = 0, [129] = 0, [130] = 0, [131] = 0, [132] = 0, [133] = 0, [134] = 0, [135] = 0,
    [136] = 0, [137] = 0, [138] = 0, [139] = 0, [140] = 0, [141] = 0, [142] = 0, [143] = 0,
    [144] = 0, [145] = 0, [146] = 0, [147] = 0, [148] = 0, [149] = 0, [150] = 0, [151] = 0,
    [152] = 0, [153] = 0, [154] = 0, [155] = 0, [156] = 0, [157] = 0, [158] = 0, [159] = 0,
    [160] = 0, [161] = 0, [162] = 0, [163] = 0, [164] = 0, [165] = 0, [166] = 0, [167] = 0,
    [168] = 0, [169] = 0, [170] = 0, [171] = 0, [172] = 0, [173] = 0, [174] = 0, [175] = 0,
    [176] = 0, [177] = 0, [178] = 0, [179] = 0, [180] = 0, [181] = 0, [182] = 0, [183] = 0,
    [184] = 0, [185] = 0, [186] = 0, [187] = 0, [188] = 0, [189] = 0, [190] = 0, [191] = 0,
    [192] = 0, [193] = 0, [194] = 0, [195] = 0, [196] = 0, [197] = 0, [198] = 0, [199] = 0,
    [200] = 0, [201] = 0, [202] = 0, [203] = 0, [204] = 0, [205] = 0, [206] = 0, [207] = 0,
    [208] = 0, [209] = 0, [210] = 0, [211] = 0, [212] = 0, [213] = 0, [214] = 0, [215] = 0,
    [216] = 0, [217] = 0, [218] = 0, [219] = 0, [220] = 0, [221] = 0, [222] = 0, [223] = 0,
    [224] = 0, [225] = 0, [226] = 0, [227] = 0, [228] = 0, [229] = 0, [230] = 0, [231] = 0,
    [232] = 0, [233] = 0, [234] = 0, [235] = 0, [236] = 0, [237] = 0, [238] = 0, [239] = 0,
    [240] = 0, [241] = 0, [242] = 0, [243] = 0, [244] = 0, [245] = 0, [246] = 0, [247] = 0,
    [248] = 0, [249] = 0, [250] = 0, [251] = 0, [252] = 0, [253] = 0, [254] = 0, [255] = 0
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
    size_t start_pos = 0;
    size_t len = 0;

    parserState ps;

    {
        const lemonHttpError ret = appendHttpToParser(&ps, request);
        if (OK != ret) {
            return ret;
        }
    }

    ParseHTTP11Init(&pParser);
    /*ParseHTTP11Trace(stdout, "parser >>");*/

    ps.isParsed = ps.isParseFailed = ps.isSyntaxIncorrect = FALSE;
    memcpy(&(ps.stopSymbols), &stop_symbols, sizeof (stop_symbols));
    while (
            (FALSE == isParsed(&ps)) && 
            (FALSE == isParseFailed(&ps)) && 
            (FALSE == isSyntaxIncorrect(&ps))
            ) {
        len = 0;
        while (!(ps.stopSymbols[(request->privateBuffer)[start_pos + len]])) {
            ++len;
        }

        if (0 == len) {
            ParseHTTP11(&pParser, ps.stopSymbols[(request->privateBuffer)[start_pos]], &((request->privateBuffer)[start_pos]), &ps);
            if ((4 <= start_pos) && ((request->privateBuffer)[start_pos] == '\n')  && ((request->privateBuffer)[start_pos - 1] == '\r') && ((request->privateBuffer)[start_pos - 2] == '\n') && ((request->privateBuffer)[start_pos - 3] == '\r')) {
                ++start_pos;
                break;
            }
            ++start_pos;
        } else {
            ps.length = len;
            ParseHTTP11(&pParser, TOK_STRING, &((request->privateBuffer)[start_pos]), &ps);
        }
        start_pos += len;
    }

    ParseHTTP11(&pParser, 0, NULL, &ps);

    {
        const lemonHttpError ret = finalizeHttpRequest(request);
        if (OK != ret) {
            return ret;
        }
    }

    request->body.data = &((request->privateBuffer)[start_pos]);
    request->body.length -= start_pos;

    return (FALSE == isParseFailed(&ps)) ? ((FALSE == isSyntaxIncorrect(&ps)) ? OK : INCORRECT_SYNTAX) : PARSING_IS_FAILED;
}
