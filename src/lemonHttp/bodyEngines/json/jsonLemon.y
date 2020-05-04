/*
 * Copyright (C) 2017, 2018, 2019, 2020 Parkhomenko Stanislav
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
#include "jsonPath.h"
#include "../../../boolean.h"
#include "./jsonPathInternal.h"
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

jsonn ::= json NULL.

json ::= ows start_root value end_root ows. {markJSONAsParsed(ps); puts("DONE");}

start_root ::= . { puts("ROOT_START"); updateJsonPathRequestStatusByRoot(ps->jsonRequest); }

end_root ::= . { puts("ROOT_END"); rollbackJsonPathRequestStatusByRoot(ps->jsonRequest); }

%type string {string}
%type chars {string}
%type char {string}
%type key {string}

value ::= object.
value ::= array.
value ::= number.
value ::= string(s1). { printf("\nVALUE -- %.*s\n", s1.length, s1.data); executeJsonPathCallbackWithValue(ps->jsonRequest, &s1); }
value ::= true.
value ::= false.
value ::= null.

object ::= l_crl_brckt ows r_crl_brckt.
object ::= l_crl_brckt ows object_content ows r_crl_brckt.

key(s1) ::= string(s1). {
  printf("\nKEY IN -- %.*s\n", s1.length, s1.data);
  updateJsonPathRequestStatusByFieldName(ps->jsonRequest, &s1);
}

object_content ::= key(s1) ows COLON ows value. { rollbackJsonPathRequestStatusByFieldName(ps->jsonRequest, &s1); }
object_content ::= object_content ows COMMA ows key(s1) ows COLON ows value. { rollbackJsonPathRequestStatusByFieldName(ps->jsonRequest, &s1); }

array ::= l_sqr_brckt ows r_sqr_brckt.
array ::= l_sqr_brckt ows array_content ows r_sqr_brckt.

array_content ::= array_inc value.
array_content ::= array_content ows COMMA ows array_inc value.

array_inc ::= . { puts("ARRAY ELEMENT"); updateJsonPathRequestStatusByArrayElement(ps->jsonRequest); }

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

string(s) ::= QUOTATION QUOTATION. { s = getEmptyString(); }
string(s) ::= QUOTATION chars(cs) QUOTATION. { s.data = cs.data; s.length = cs.length; }

chars(cs) ::= char(c). { cs.data = c.data; cs.length = c.length; }
chars(cs) ::= chars char(c). {
    while (c.length > 0) {
        (cs.data)[cs.length++] = *(c.data++);
        --(c.length);
    }
}
        
char(c) ::= SYM(s). { c.data = s; c.length = 1; }
char(c) ::= BACKSLASH QUOTATION(s). { c.data = s; c.length = 1; }
char(c) ::= BACKSLASH BACKSLASH(s). { c.data = s; c.length = 1; }
char(c) ::= BACKSLASH SLASH(s). { c.data = s; c.length = 1; }
char(c) ::= BACKSLASH BACKSPACE(s). { c.data = s; c.length = 1; }
char(c) ::= BACKSLASH FORMFEED(s). { c.data = s; c.length = 1; }
char(c) ::= BACKSLASH LINEFEED(s). { c.data = s; c.length = 1; }
char(c) ::= BACKSLASH CARRETURN(s). { c.data = s; c.length = 1; }
char(c) ::= BACKSLASH CHARTAB(s). { c.data = s; c.length = 1; }
char(c) ::= BACKSLASH LU hexdig(h1) hexdig(h2) hexdig(h3) hexdig(h4). { c = convertUtf16ToString(h1, h2, *h3, *h4); }
char(c) ::= LT(s). { c.data = s; c.length = 1; }
char(c) ::= LR(s). { c.data = s; c.length = 1; }
char(c) ::= LU(s). { c.data = s; c.length = 1; }
char(c) ::= LL(s). { c.data = s; c.length = 1; }
char(c) ::= LS(s). { c.data = s; c.length = 1; }
char(c) ::= LN(s). { c.data = s; c.length = 1; }
char(c) ::= hexdig(s). { c.data = s; c.length = 1; }
char(c) ::= SP(s). { c.data = s; c.length = 1; }
char(c) ::= CHARTAB(s). { c.data = s; c.length = 1; }
char(c) ::= COLON(s). { c.data = s; c.length = 1; }
char(c) ::= COMMA(s). { c.data = s; c.length = 1; }
char(c) ::= PLUS(s). { c.data = s; c.length = 1; }
char(c) ::= MINUS(s). { c.data = s; c.length = 1; }
char(c) ::= DOT(s). { c.data = s; c.length = 1; }
char(c) ::= CONTROL(s). { c.data = s; c.length = 1; }
char(c) ::= EXT(s). { c.data = s; c.length = 1; }


true ::= LT LR LU LE.
false ::= LF LA LL LS LE.
null ::= LN LU LL LL.

l_crl_brckt ::= LBRACE(c). { puts("VAL_START"); puts(c); updateJsonPathRequestStatusByObject(ps->jsonRequest, c); }

r_crl_brckt ::= RBRACE(c). { puts("VAL_END"); puts(c); rollbackJsonPathRequestStatusByObject(ps->jsonRequest, c); }

l_sqr_brckt ::= LBRACKET(c). { puts("ARRAY_START"); puts(c); updateJsonPathRequestStatusByArray(ps->jsonRequest, c); }

r_sqr_brckt ::= RBRACKET(c). { puts("ARRAY_END"); puts(c); rollbackJsonPathRequestStatusByArray(ps->jsonRequest, c); }

ows ::= .
ows ::= ows SP.
ows ::= ows CHARTAB.
ows ::= ows LINEFEED.
ows ::= ows CARRETURN.