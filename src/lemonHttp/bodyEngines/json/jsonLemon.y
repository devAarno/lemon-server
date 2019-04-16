/*
 * Copyright (C) 2017, 2018, 2019 Parkhomenko Stanislav
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
#include "jsonParser.h"
#include "../../../boolean.h"
}

%name ParseJSON
%token_type {char*}
%token_prefix JSON_
%extra_argument {jsonParserState *ps}
%syntax_error  { if (FALSE == isJSONParsed(ps) ) { puts("Systax error"); puts("---"); markJSONAsIncorrect(ps);} }
%parse_failure { markJSONAsParseFailed(ps); }

/* Declare unusable token
%token_class control CONTROL.
%token_class ext EXT.*/

%token_class digit ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE.

%token_class digit_without_zero ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE.

%token_class hexdig ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|A|B|C|D|E|F|LA|LB|LC|LD|LE|LF.

/* !#$%&'()*+,-./01234567890:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~ */
/* %token_class symbols_without_quotation_and_backslash EXCLAMATION|OCTOTHORPE|DOLLAR|PERCENT|AMPERSAND|APOSTROPHE|LPARENTHESIS|RPARENTHESIS|ASTERISK|PLUS|COMMA|MINUS|DOT|SLASH|ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|COLON|SEMICOLON|LESSTHAN|EQUALS|GREATERTHAN|QUESTION|AT|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|LBRACKET|RBRACKET|CARET|UNDERSCORE|BACKQUOTE|LA|LB|LC|LD|LE|LF|LG|LH|LI|LJ|LK|LL|LM|LN|LO|LP|LQ|LR|LS|LT|LU|LV|LW|LX|LY|LZ|LBRACE|VBAR|RBRACE|TILDE.*/

json ::= ows value ows. {markJSONAsParsed(ps); puts("DONE");}

value ::= object.
value ::= array.
value ::= number.
value ::= string.
value ::= true.
value ::= false.
value ::= null.

object ::= l_crl_brckt ows r_crl_brckt.
object ::= l_crl_brckt ows object_content ows r_crl_brckt.

object_content ::= string ows COLON ows value.
object_content ::= object_content ows COMMA ows string ows COLON ows value.

array ::= l_sqr_brckt ows r_sqr_brckt.
array ::= l_sqr_brckt ows array_content ows r_sqr_brckt.

array_content ::= value.
array_content ::= array_content ows COMMA ows value.

number ::= MINUS mantissa.
number ::= mantissa.
number ::= MINUS mantissa exponent.
number ::= mantissa exponent.

mantissa ::= ZERO.
mantissa ::= ZERO DOT digits.
mantissa ::= digit_without_zero.
mantissa ::= digit_without_zero DOT digits.
mantissa ::= digit_without_zero digits.
mantissa ::= digit_without_zero digits DOT digits.

exponent ::= exp sign digits.

exp ::= LE.
exp ::= E.

sign ::= .
sign ::= PLUS.
sign ::= MINUS.

digits ::= digit.
digits ::= digits digit.

string ::= QUOTATION chars QUOTATION.

chars ::= .
chars ::= chars char.
        
char ::= SYM.
char ::= BACKSLASH QUOTATION.
char ::= BACKSLASH BACKSLASH.
char ::= BACKSLASH SLASH.
char ::= BACKSLASH BACKSPACE.
char ::= BACKSLASH FORMFEED.
char ::= BACKSLASH LINEFEED.
char ::= BACKSLASH CARRETURN.
char ::= BACKSLASH CHARTAB.
char ::= BACKSLASH LU hexdig hexdig hexdig hexdig.
char ::= LT.
char ::= LR.
char ::= LU.
char ::= LL.
char ::= LS.
char ::= LN.
char ::= hexdig.
char ::= SP.
char ::= CHARTAB.
char ::= COLON.
char ::= COMMA.
char ::= PLUS.
char ::= MINUS.
char ::= DOT.
char ::= CONTROL.
char ::= EXT.


true ::= LT LR LU LE.
false ::= LF LA LL LS LE.
null ::= LN LU LL LL.

l_crl_brckt ::= LBRACE.

r_crl_brckt ::= RBRACE.

l_sqr_brckt ::= LBRACKET.

r_sqr_brckt ::= RBRACKET.

ows ::= .
ows ::= ows SP.
ows ::= ows CHARTAB.
ows ::= ows LINEFEED.
ows ::= ows CARRETURN.