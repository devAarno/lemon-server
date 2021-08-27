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


%include {
#include <stddef.h>
#include <assert.h>
#include "jsonPathParser.h"
#include "../../../boolean.h"
#include "./jsonPathInternal.h"
#include "../../string.h"
}

%name ParseJSONPath
%token_type {char*}
%token_prefix JSONPATH_
%extra_argument {jsonPathParserState *ps}
%syntax_error  { if (FALSE == isJSONPathParsed(ps) ) { puts("Syntax error"); puts("---"); markJSONPathAsSyntaxIncorrect(ps);} }
%parse_failure { markJSONPathAsParseFailed(ps); }

/* Declare unusable token */
%token_class control CONTROL.
%token_class ext EXT.

%token_class digit ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE.

%token_class hexdig ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|A|B|C|D|E|F|LA|LB|LC|LD|LE|LF.

mainn ::= main NULL.

main ::= jsonpath. { markJSONPathAsParsed(ps); puts("DONE"); }

jsonpath ::= DOLLAR. {
    const string emptyString = getEmptyString();
    if (NULL == appendJsonPathElementOfHttpRequest(ps->jsonPathRequest, &(emptyString), ROOT)) {
        markJSONPathAsParseFailed(ps);
    }
}

%type dotobjectname {string}
%type objectname {string}
%type char {string}
%type arrayindex {string}
jsonpath ::= jsonpath DOT dotobjectname(var_s). { appendJsonPathElementOfHttpRequest(ps->jsonPathRequest, &var_s, NAME); }
jsonpath ::= jsonpath DOT ASTERISK. {
    const string emptyString = getEmptyString();
    appendJsonPathElementOfHttpRequest(ps->jsonPathRequest, &(emptyString), ANY);
}
jsonpath ::= jsonpath LBRACKET APOSTROPHE objectname(var_s) APOSTROPHE RBRACKET. { appendJsonPathElementOfHttpRequest(ps->jsonPathRequest, &var_s, NAME); }
jsonpath ::= jsonpath LBRACKET arrayindex(var_s) RBRACKET. { appendJsonPathElementOfHttpRequest(ps->jsonPathRequest, &var_s, INDEX); }
jsonpath ::= jsonpath LBRACKET ASTERISK RBRACKET. {
    const string emptyString = getEmptyString();
    appendJsonPathElementOfHttpRequest(ps->jsonPathRequest, &(emptyString), ANYINDEX);
}
jsonpath ::= jsonpath DOT DOT. {
    const string emptyString = getEmptyString();
    appendJsonPathElementOfHttpRequest(ps->jsonPathRequest, &(emptyString), RECURSIVE);
}
jsonpath ::= jsonpath DOT DOT dotobjectname(var_s). {
    const string emptyString = getEmptyString();
    appendJsonPathElementOfHttpRequest(ps->jsonPathRequest, &(emptyString), RECURSIVE);
    appendJsonPathElementOfHttpRequest(ps->jsonPathRequest, &var_s, NAME);
}
jsonpath ::= jsonpath DOT DOT ASTERISK. {
    const string emptyString = getEmptyString();
    appendJsonPathElementOfHttpRequest(ps->jsonPathRequest, &(emptyString), RECURSIVE);
    appendJsonPathElementOfHttpRequest(ps->jsonPathRequest, &(emptyString), ANY);
}

dotobjectname(var_s) ::= char(var_ch). { var_s.length = var_ch.length; var_s.data = var_ch.data; }
dotobjectname(var_s) ::= BACKSLASH ASTERISK(var_c). { var_s.length = 1; var_s.data = var_c; }
dotobjectname(var_s) ::= dotobjectname char(var_ch). {
    while (var_ch.length > 0) {
        (var_s.data)[var_s.length++] = *(var_ch.data++);
        --(var_ch.length);
    }
}
dotobjectname(var_s) ::= dotobjectname BACKSLASH ASTERISK(var_c). { (var_s.data)[var_s.length] = *var_c; ++(var_s.length); }

/*dotobjectname(var_s) ::= dotobjectname char(var_ch). { (var_s.data)[var_s.length] = *var_ch; ++(var_s.length); }
dotobjectname(var_s) ::= dotobjectname BACKSLASH ASTERISK(var_c). { (var_s.data)[var_s.length] = *var_c; ++(var_s.length); }*/

objectname(var_s) ::= char(var_ch). { var_s.length = var_ch.length; var_s.data = var_ch.data; }
objectname(var_s) ::= BACKSLASH APOSTROPHE(var_c). { var_s.length = 1; var_s.data = var_c; }
objectname(var_s) ::= BACKSLASH LBRACKET(var_c). { var_s.length = 1; var_s.data = var_c; }
objectname(var_s) ::= BACKSLASH RBRACKET(var_c). { var_s.length = 1; var_s.data = var_c; }
objectname(var_s) ::= objectname char(var_ch). {
    while (var_ch.length > 0) {
        (var_s.data)[var_s.length++] = *(var_ch.data++);
        --(var_ch.length);
    }
}
objectname(var_s) ::= objectname BACKSLASH APOSTROPHE(var_c). { (var_s.data)[var_s.length] = *var_c; ++(var_s.length); }
objectname(var_s) ::= objectname BACKSLASH LBRACKET(var_c). { (var_s.data)[var_s.length] = *var_c; ++(var_s.length); }
objectname(var_s) ::= objectname BACKSLASH RBRACKET(var_c). { (var_s.data)[var_s.length] = *var_c; ++(var_s.length); }

arrayindex(var_s) ::= digit(var_ch). { var_s.length = 1; var_s.data = var_ch; }
arrayindex(var_s) ::= arrayindex digit(var_ch). { (var_s.data)[var_s.length] = *var_ch; ++(var_s.length); }

char(var_ch) ::= SYM(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
char(var_ch) ::= hexdig(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
char(var_ch) ::= LU(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
char(var_ch) ::= BACKSLASH QUOTATION(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
char(var_ch) ::= BACKSLASH BACKSLASH(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
char(var_ch) ::= BACKSLASH SLASH(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
char(var_ch) ::= BACKSLASH BACKSPACE(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
char(var_ch) ::= BACKSLASH FORMFEED(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
char(var_ch) ::= BACKSLASH LINEFEED(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
char(var_ch) ::= BACKSLASH CARRETURN(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
char(var_ch) ::= BACKSLASH CHARTAB(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
char(var_ch) ::= BACKSLASH LU hexdig(h1) hexdig(h2) hexdig(h3) hexdig(h4). { var_ch = convertUtf16ToString(h1, h2, *h3, *h4); }