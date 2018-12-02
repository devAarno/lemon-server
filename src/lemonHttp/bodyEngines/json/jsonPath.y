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


%include {
#include <stddef.h>
#include <assert.h>
#include "jsonPathParser.h"
#include "../../../boolean.h"
}

%name ParseJSONPath
%token_type {char*}
%token_prefix JSONPATH_
%extra_argument {jsonPathParserState *ps}
%syntax_error  { if (FALSE == isJSONPathParsed(ps) ) { puts("Systax error"); puts("---"); markJSONPathAsSyntaxIncorrect(ps);} }
%parse_failure { markJSONPathAsParseFailed(ps); }

/* Declare unusable token */
%token_class control CONTROL.
%token_class ext EXT.

%token_class digit ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE.

%token_class hexdig ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|A|B|C|D|E|F|LA|LB|LC|LD|LE|LF.

main ::= jsonpath.

jsonpath ::= DOLLAR.
jsonpath ::= jsonpath DOT dotobjectname.
jsonpath ::= jsonpath DOT ASTERISK.
jsonpath ::= jsonpath LBRACKET APOSTROPHE objectname APOSTROPHE RBRACKET.
jsonpath ::= jsonpath LBRACKET arrayindex RBRACKET.

dotobjectname ::= char.
dotobjectname ::= BACKSLASH ASTERISK.
dotobjectname ::= dotobjectname char.
dotobjectname ::= dotobjectname BACKSLASH ASTERISK.

objectname ::= char.
objectname ::= BACKSLASH APOSTROPHE.
objectname ::= BACKSLASH LBRACKET.
objectname ::= BACKSLASH RBRACKET.
objectname ::= objectname char.
objectname ::= objectname BACKSLASH APOSTROPHE.
objectname ::= objectname BACKSLASH LBRACKET.
objectname ::= objectname BACKSLASH RBRACKET.

arrayindex ::= .
arrayindex ::= arrayindex digit.
arrayindex ::= ASTERISK.
        
char ::= SYM.
char ::= hexdig.
char ::= LU.
char ::= BACKSLASH QUOTATION.
char ::= BACKSLASH BACKSLASH.
char ::= BACKSLASH SLASH.
char ::= BACKSLASH BACKSPACE.
char ::= BACKSLASH FORMFEED.
char ::= BACKSLASH LINEFEED.
char ::= BACKSLASH CARRETURN.
char ::= BACKSLASH CHARTAB.
char ::= BACKSLASH LU hexdig hexdig hexdig hexdig.