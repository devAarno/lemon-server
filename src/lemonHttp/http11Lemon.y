/*
 * Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022, 2023, 2024 Parkhomenko Stanislav
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
#include "parser.h"
#include "httpRequest.h"
#include "httpRequestInternal.h"
#include "jsonPathInternal.h"
#include "string.h"
#include "rules.h"
}

%name ParseHTTP11
%token_type {char*}
%token_prefix TOK_
%extra_argument {parserState *ps}
%syntax_error  {if (FALSE == isParsed(ps) ) { puts("Syntax error"); puts("---"); markAsSyntaxIncorrect(ps);} }
%parse_failure { markAsParseFailed(ps); }

/* https://habrahabr.ru/post/232385/ 
 * https://www.mnot.net/blog/2014/06/07/rfc2616_is_dead
 * https://www.rfc-editor.org/rfc/rfc7230
 */

/* Declare unusable token */
%token_class control CONTROL BACKSPACE FORMFEED.

/* !"#$%&'()*+,-./01234567890:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~ */
%token_class vchar EXCLAMATION|QUOTATION|OCTOTHORPE|DOLLAR|PERCENT|AMPERSAND|APOSTROPHE|LPARENTHESIS|RPARENTHESIS|ASTERISK|PLUS|COMMA|MINUS|DOT|SLASH|ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|COLON|SEMICOLON|LESSTHAN|EQUALS|GREATERTHAN|QUESTION|AT|A|B|C|D|E|F|H|P|T|LBRACKET|BACKSLASH|RBRACKET|CARET|UNDERSCORE|BACKQUOTE|LA|LB|LC|LD|LE|LF|LL|LN|LR|LS|LT|LU|SYM|LBRACE|VBAR|RBRACE|TILDE.

%token_class digit ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE.

%token_class digit_without_zero ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE.

/* tchar = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." /
    "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA */
%token_class tchar EXCLAMATION|OCTOTHORPE|DOLLAR|PERCENT|AMPERSAND|APOSTROPHE|ASTERISK|PLUS|MINUS|DOT|CARET|UNDERSCORE|BACKQUOTE|VBAR|TILDE|ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|A|B|C|D|E|F|H|P|T|LA|LB|LC|LD|LE|LF|LL|LN|LR|LS|LT|LU|SYM.

/* unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~" */
%token_class unreserved A|B|C|D|E|F|H|P|T|LA|LB|LC|LD|LE|LF|LL|LN|LR|LS|LT|LU|SYM|ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|MINUS|DOT|UNDERSCORE|TILDE.

/* unreserved_dot_ex = ALPHA / DIGIT / "-" / "_" / "~" */
%token_class unreserved_dot_ex A|B|C|D|E|F|H|P|T|LA|LB|LC|LD|LE|LF|LL|LN|LR|LS|LT|LU|SYM|ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|MINUS|UNDERSCORE|TILDE.

/* sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "=" */
%token_class subdelims EXCLAMATION|DOLLAR|AMPERSAND|APOSTROPHE|LPARENTHESIS|RPARENTHESIS|ASTERISK|PLUS|COMMA|SEMICOLON|EQUALS.

/* sub-delims-kv = "!" / "$" / "'" / "(" / ")" / "*" / "+" / "," / ";" */
%token_class subdelims_kv EXCLAMATION|DOLLAR|APOSTROPHE|LPARENTHESIS|RPARENTHESIS|ASTERISK|PLUS|COMMA|SEMICOLON.

%token_class hexdig ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|A|B|C|D|E|F|LA|LB|LC|LD|LE|LF.

/* !#$%&'()*+,-./01234567890:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~ */
/* %token_class symbols_without_quotation_and_backslash EXCLAMATION|OCTOTHORPE|DOLLAR|PERCENT|AMPERSAND|APOSTROPHE|LPARENTHESIS|RPARENTHESIS|ASTERISK|PLUS|COMMA|MINUS|DOT|SLASH|ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|COLON|SEMICOLON|LESSTHAN|EQUALS|GREATERTHAN|QUESTION|AT|A|B|C|D|E|F|H|P|T|LBRACKET|RBRACKET|CARET|UNDERSCORE|BACKQUOTE|LA|LB|LC|LD|LE|LF|LL|LN|LR|LS|LT|LU|SYM|LBRACE|VBAR|RBRACE|TILDE.*/

%token_class json_sym SYM|EXCLAMATION|OCTOTHORPE|DOLLAR|PERCENT|AMPERSAND|APOSTROPHE|LPARENTHESIS|RPARENTHESIS|ASTERISK|SEMICOLON|LESSTHAN|EQUALS|GREATERTHAN|QUESTION|AT|H|P|T|CARET|UNDERSCORE|BACKQUOTE|VBAR|TILDE.
%token_class jsonpath_sym SYM|SP|EXCLAMATION|OCTOTHORPE|PERCENT|AMPERSAND|LPARENTHESIS|RPARENTHESIS|PLUS|COMMA|MINUS|COLON|SEMICOLON|LESSTHAN|EQUALS|GREATERTHAN|QUESTION|AT|H|P|T|CARET|UNDERSCORE|BACKQUOTE|LL|LN|LR|LS|LT|LBRACE|VBAR|RBRACE|TILDE.

main ::= ONE http_message. {markAsParsed(ps); puts("DONE");}
main ::= TWO jsonn. {markAsParsed(ps); puts("DONE");}
main ::= THREE mmainn.  {markAsParsed(ps); puts("DONE");}

/* HTTP-message   = start-line
                    *( header-field CRLF )
                    CRLF
                    [ message-body ]
 * (Here message body ignored as a parsing entity.)
 */

/* start-line     = request-line / status-line
 * (Here request only.)
 */
http_message ::= request_line http_headers crlf. {markAsParsed(ps); puts("DONE");}


/* request-line   = method SP request-target SP HTTP-version CRLF */
request_line ::= method SP request_target SP http_version crlf.

/* method = token  */
%type token {string}
method ::= token(var_s). { executeHttpMethodCallback(ps->container.httpRequest, &var_s); }

/* token = 1*tchar */
token(var_s) ::= tchar(var_c). { var_s.length = 1; var_s.data = var_c; }
token(var_s) ::= token tchar. { ++(var_s.length); }

/* request-target = origin-form
                    / absolute-form
                    / authority-form
                    / asterisk-form
 * (Here only origin-form.)
 */
/* origin-form    = absolute-path [ "?" query ] */

/* pct-encoded    = "%" HEXDIG HEXDIG */
/* sub-delims     */
%type absolute_path {string}
request_target ::= absolute_path(var_s). {
    if (LE_OK != decodeValue( &(var_s) , FALSE)) {
        markAsParseFailed(ps);
    } else {
        executeHttpUriCallback(ps->container.httpRequest, &var_s);
    }
}
request_target ::= absolute_path(var_s) QUESTION query. {
    if (LE_OK != decodeValue( &(var_s) , FALSE)) {
        markAsParseFailed(ps);
    } else {
        executeHttpUriCallback(ps->container.httpRequest, &var_s);
    }
}

/* absolute-path  = 1*( "/" segment ) */
%type segment {string}
absolute_path(var_s) ::= SLASH(var_c). { var_s.length = 1; var_s.data = var_c; }
absolute_path(var_s) ::= SLASH(var_c) segment(var_l). {
    var_s.data = var_c;
    var_s.length = var_l.length + 1;
}
absolute_path(var_s) ::= SLASH(var_c) DOT. { var_s.length = 1; var_s.data = var_c; }
absolute_path(var_s) ::= SLASH(var_c) DOT segment(var_l). {
    var_s.data = var_c;
    var_s.length = var_l.length + 2;
}
absolute_path(var_s) ::= SLASH(var_c) DOT DOT. { var_s.length = 1; var_s.data = var_c; }
absolute_path        ::= absolute_path SLASH.
absolute_path        ::= absolute_path SLASH DOT.
absolute_path(var_s) ::= absolute_path SLASH DOT DOT. {
    while ('/' != ((var_s.data)[var_s.length - 1]) && (var_s.length > 0)) {
        --(var_s.length);
    }
    if (var_s.length > 0) {
        /* Delete SLASH too if possible */
        --(var_s.length);
    }
}
absolute_path(var_s) ::= absolute_path SLASH segment(var_l). {
    size_t i;
    for (i = 0; i <= var_l.length; ++i) {
        (var_s.data)[var_s.length + i] = (var_l.data)[i - 1];
    }
    var_s.length += 1 + var_l.length;
}
absolute_path(var_s) ::= absolute_path SLASH DOT segment(var_l). {
    size_t i;
    for (i = 0; i <= var_l.length + 1; ++i) {
        (var_s.data)[var_s.length + i] = (var_l.data)[i - 2];
    }
    var_s.length += 2 + var_l.length;
}

/* segment        = *pchar */
%type pchar {string}
/*segment(var_l) ::= . { var_l = 0; }*/
segment(var_l) ::= pchar(var_p). { var_l = var_p; }
segment(var_l) ::= segment pchar(var_p). { var_l.length += var_p.length; }
segment(var_l) ::= segment DOT. { var_l.length += 1; }

/* pchar          = unreserved / pct-encoded / sub-delims / ":" / "@" */
pchar(var_p) ::= unreserved_dot_ex(var_c). { var_p.data = var_c; var_p.length = 1; }
pchar(var_p) ::= pctencoded(var_z). { var_p.data = var_z; var_p.length = 3; }
pchar(var_p) ::= subdelims(var_c). { var_p.data = var_c; var_p.length = 1; }
pchar(var_p) ::= COLON(var_c). { var_p.data = var_c; var_p.length = 1; }
pchar(var_p) ::= AT(var_c). { var_p.data = var_c; var_p.length = 1; }

/* pct-encoded    = "%" HEXDIG HEXDIG */
pctencoded(var_z) ::= PERCENT(var_c) hexdig hexdig. { var_z = var_c; }

/* query          = *( pchar / "/" / "?" )
 * (In general case, but we go deeper. Let's exclude '=' and '&'
 * from subdelims and include and get a key-value pairs. Assume '/' and '?' as
 * a part of custom pchar.)
 */
query ::= .
query ::= key_val.
query ::= query AMPERSAND key_val.

%type key {string}
%type val {string}
key_val ::= key(var_k) EQUALS. {
    if (LE_OK != decodeValue( &(var_k) , TRUE)) {
        markAsParseFailed(ps);
    } else {
        const string e = getEmptyString();
        executeGetParameterCallback(ps->container.httpRequest, &var_k, &e);
    }
}
key_val ::= key(var_k) EQUALS val(var_l). {
    if ((LE_OK != decodeValue( &(var_k) , TRUE)) ||
            (LE_OK != decodeValue( &(var_l) , TRUE))) {
        markAsParseFailed(ps);
    } else {
        executeGetParameterCallback(ps->container.httpRequest, &var_k, &var_l);
    }
}

%type pchar_kv {string}
key(var_k) ::= pchar_kv(var_c). { var_k.length = var_c.length; var_k.data = var_c.data; }
key(var_k) ::= key pchar_kv(var_c). { var_k.length += var_c.length; }


val(var_l) ::= pchar_kv(var_c). { var_l.length = var_c.length; var_l.data = var_c.data; }
val(var_l) ::= val pchar_kv(var_c). { var_l.length += var_c.length; }

pchar_kv(var_c) ::= unreserved(var_cc). { var_c.length = 1; var_c.data = var_cc; }
pchar_kv(var_c) ::= pctencoded(var_cc). { var_c.length = 3; var_c.data = var_cc; }
pchar_kv(var_c) ::= subdelims_kv(var_cc). { var_c.length = 1; var_c.data = var_cc; }
pchar_kv(var_c) ::= COLON(var_cc). { var_c.length = 1; var_c.data = var_cc; }
pchar_kv(var_c) ::= AT(var_cc). { var_c.length = 1; var_c.data = var_cc; }
pchar_kv(var_c) ::= SLASH(var_cc). { var_c.length = 1; var_c.data = var_cc; }
pchar_kv(var_c) ::= QUESTION(var_cc). { var_c.length = 1; var_c.data = var_cc; }

/* HTTP-version  = HTTP-name "/" DIGIT "." DIGIT */
/* HTTP-name     = %x48.54.54.50 ; "HTTP", case-sensitive */
%type http_version {string}
http_version(var_s) ::= H(var_c) T T P SLASH digit DOT digit. {
    var_s.length = 8;
    var_s.data = var_c;
    executeHttpVersionCallback(ps->container.httpRequest, &var_s);
}

/* HTTP-message   = start-line
                    *( header-field CRLF )
                    ... */
http_headers ::= .
http_headers ::= http_headers header_field crlf.

/* header-field   = field-name ":" OWS field-value OWS */
/* field-name     = token */
/* field-value    = *( field-content / obs-fold ) */
/* field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ] */
/* field-vchar    = VCHAR / obs-text */
/* There too many rules for SP and HTAB. I suppose that it is out of LALR(1) grammar support. */
/* So, we drop obs-fold, and reduce rules to: */
/* header-field   = field-name ":" trim(field-value) */
/* field-value    = *( vchar / SP / HTAB / OBSTEXT ) */
%type field_value {string}
header_field ::= token(var_k) COLON. {
    const string e = getEmptyString();
    executeHeaderCallback(ps->container.httpRequest, &var_k, &e);
}
header_field ::= token(var_k) COLON field_value(var_v). {
    trim(&var_v);
    executeHeaderCallback(ps->container.httpRequest, &var_k, &var_v);
}

field_value(var_f) ::= vchar(var_c). { var_f.length = 1; var_f.data = var_c; }
field_value(var_f) ::= SP(var_c). { var_f.length = 1; var_f.data = var_c; }
field_value(var_f) ::= HTAB(var_c). { var_f.length = 1; var_f.data = var_c; }
field_value(var_f) ::= OBSTEXT(var_c). { var_f.length = 1; var_f.data = var_c; }
field_value(var_f) ::= field_value vchar. { ++var_f.length; }
field_value(var_f) ::= field_value SP. { ++var_f.length; }
field_value(var_f) ::= field_value HTAB. { ++var_f.length; }
field_value(var_f) ::= field_value OBSTEXT. { ++var_f.length; }

crlf ::= CR CLF.

/* ows ::= .
ows ::= ows SP.
ows ::= ows HTAB. */



/* ---------------- JSON ---------------- */

/* There are length counters */
%type string {size_t}
%type json_chars {size_t}
%type json_char {size_t}
%type json_key {size_t}
%type array {size_t}
%type object {size_t}
%type json_ows {size_t}
%type number {size_t}


jsonn ::= json NULL.

json ::= json_ows start_root root_value end_root json_ows. {markAsParsed(ps); puts("DONE");}

start_root ::= . { puts("ROOT_START"); openFrame(ps->container.httpRequest, ps->currentPosition, PARSED_JSON_ROOT); }
root_value ::= value(value_length). { /* setFrameLength(ps->container.httpRequest, value_length); */ }
end_root ::= . { puts("ROOT_END"); closeFrame(ps->container.httpRequest, ps->currentPosition); }

%type value {size_t}

value(value_length) ::= object(object_length). { value_length = object_length; setFrameLength(ps->container.httpRequest, value_length); }
value(value_length) ::= array(array_length). { value_length = array_length; setFrameLength(ps->container.httpRequest, value_length); }

value(value_length) ::= number(number_length). { value_length = number_length; setFrameLength(ps->container.httpRequest, value_length); }
value(value_length) ::= string(string_length). { value_length = string_length; /* The String set frameLength itself. */ }
value(value_length) ::= true. { value_length = 4; setTrue(ps->container.httpRequest); }
value(value_length) ::= false. { value_length = 5; setFalse(ps->container.httpRequest); }
value(value_length) ::= null. { value_length = 4; setNull(ps->container.httpRequest); }


/* value(value_length) ::= true. { value_length = 4; setFrameLength(ps->container.httpRequest, 4); }
value(value_length) ::= false. { value_length = 5; setFrameLength(ps->container.httpRequest, 5); }
value(value_length) ::= null. { value_length = 4; setFrameLength(ps->container.httpRequest, 4); } */

object(object_length) ::= l_crl_brckt inner_object(inner_object_length) r_crl_brckt. {
  object_length = inner_object_length; /* inner_object takes brackets into account */
}

inner_object(inner_object_length) ::= json_ows(json_ows_length). { inner_object_length = 2 + json_ows_length; setFrameLength(ps->container.httpRequest, inner_object_length); }
inner_object(inner_object_length) ::= json_ows(json_ows_length_1) object_content(object_content_length) json_ows(json_ows_length_2). { inner_object_length = 2 + json_ows_length_1 + object_content_length + json_ows_length_2; setFrameLength(ps->container.httpRequest, inner_object_length); }

json_key(json_key_length) ::= json_chars(json_chars_length). {
  json_key_length = json_chars_length;
  fixFieldName(ps->container.httpRequest, json_chars_length);
}

%type object_content {size_t}
object_content(object_content_length) ::= open_key_value QUOTATION open_key json_key(json_key_length) close_key QUOTATION json_ows(json_ows_length_1) COLON json_ows(json_ows_length_2) open_value value(value_length) close_value close_key_value. {
  object_content_length = 3 + json_key_length + json_ows_length_1 + json_ows_length_2 + value_length;
}
object_content(object_content_length) ::= object_content json_ows(json_ows_length_1) COMMA json_ows(json_ows_length_2) open_key_value QUOTATION open_key json_key(json_key_length) close_key QUOTATION json_ows(json_ows_length_3) COLON json_ows(json_ows_length_4) open_value value(value_length) close_value close_key_value. {
  object_content_length += 4 + json_ows_length_1 + json_ows_length_2 + json_key_length + json_ows_length_3 + json_ows_length_4 + value_length;
}

open_key_value ::= . { openFrame(ps->container.httpRequest, ps->currentPosition, PARSED_JSON_FIELD); }
close_key_value ::= . { closeFrame(ps->container.httpRequest, ps->currentPosition); }

open_key ::= . { openKey(ps->container.httpRequest, ps->currentPosition); }
close_key ::= .

open_value ::= . { openValue(ps->container.httpRequest, ps->currentPosition); }
close_value ::= .

array(array_length) ::= l_sqr_brckt json_ows(json_ows_length) r_sqr_brckt. {
  array_length = 2 + json_ows_length;
}
array(array_length) ::= l_sqr_brckt json_ows(json_ows_length_1) array_content(array_content_length) json_ows(json_ows_length_2) r_sqr_brckt. {
  array_length = 2 + json_ows_length_1 + array_content_length + json_ows_length_2;
}

%type array_content {size_t}
array_content(array_content_length) ::= open_array_value value(value_length) close_array_value. { array_content_length = value_length; }
array_content(array_content_length) ::= array_content json_ows(json_ows_length_1) COMMA json_ows(json_ows_length_2) open_array_value value(value_length) close_array_value. {
  array_content_length += 1 + json_ows_length_1 + json_ows_length_2 + value_length;
}

open_array_value ::= . { openFrame(ps->container.httpRequest, ps->currentPosition, PARSED_JSON_ARRAY_VALUE); }
close_array_value ::= . { closeFrame(ps->container.httpRequest, ps->currentPosition); }

/* index_zero ::= . { puts("ARRAY ZERO ELEMENT"); updateJsonPathRequestStatusByZeroElement(ps->container.httpRequest, ps->currentPosition); }
index_inc ::= . { puts("ARRAY ELEMENT"); updateJsonPathRequestStatusByArrayElement(ps->container.httpRequest, ps->currentPosition); } */

%type mantissa {size_t}
%type exponent {size_t}

number(number_length) ::= MINUS mantissa(mantissa_length). { number_length = 1 + mantissa_length; }
number(number_length) ::= mantissa(mantissa_length). { number_length = mantissa_length; }
number(number_length) ::= MINUS mantissa(mantissa_length) exponent(exponent_length). {
  number_length = 1 + mantissa_length + exponent_length;
}
number(number_length) ::= mantissa(mantissa_length) exponent(exponent_length). {
  number_length = mantissa_length + exponent_length;
}

mantissa(mantissa_length) ::= ZERO. { mantissa_length = 1; }
mantissa(mantissa_length) ::= ZERO DOT digits(digits_length). { mantissa_length = 2 + digits_length; }
mantissa(mantissa_length) ::= digit_without_zero. { mantissa_length = 1; }
mantissa(mantissa_length) ::= digit_without_zero DOT digits(digits_length). { mantissa_length = 2 + digits_length; }
mantissa(mantissa_length) ::= digit_without_zero digits(digits_length). { mantissa_length = 1 + digits_length; }
mantissa(mantissa_length) ::= digit_without_zero digits(digits_length_1) DOT digits(digits_length_2). { mantissa_length = 2 + digits_length_1 + digits_length_2; }

exponent(exponent_length) ::= exp digits(digits_length). { exponent_length = 1 + digits_length; }
exponent(exponent_length) ::= exp PLUS digits(digits_length). { exponent_length = 2 + digits_length; }
exponent(exponent_length) ::= exp MINUS digits(digits_length). { exponent_length = 2 + digits_length; }

exp ::= LE.
exp ::= E.

%type digits {size_t}

digits(digits_length) ::= digit. { digits_length = 1; }
digits(digits_length) ::= digits digit. { ++(digits_length); }

string(string_length) ::= QUOTATION QUOTATION. { string_length = 2; }
string(string_length) ::= QUOTATION json_chars(json_chars_length) QUOTATION. {
  string_length = 2 + json_chars_length;
  setFrameString(ps->container.httpRequest, json_chars_length);
}

json_chars(json_chars_length) ::= json_char(json_char_length). { json_chars_length = json_char_length; }
json_chars(json_chars_length) ::= json_chars json_char(json_char_length). {
    json_chars_length += json_char_length;
}

json_char(length) ::= json_sym. { length = 1; }
json_char(length) ::= BACKSLASH QUOTATION. { length = 2; }
json_char(length) ::= BACKSLASH BACKSLASH. { length = 2; }
json_char(length) ::= BACKSLASH SLASH. { length = 2; }
json_char(length) ::= BACKSLASH BACKSPACE. { length = 2; }
json_char(length) ::= BACKSLASH FORMFEED. { length = 2; }
json_char(length) ::= BACKSLASH CLF. { length = 2; }
json_char(length) ::= BACKSLASH CR. { length = 2; }
json_char(length) ::= BACKSLASH HTAB. { length = 2; }
json_char(length) ::= BACKSLASH LU hexdig hexdig hexdig hexdig. { length = 6; }
json_char(length) ::= LT. { length = 1; }
json_char(length) ::= LR. { length = 1; }
json_char(length) ::= LU. { length = 1; }
json_char(length) ::= LL. { length = 1; }
json_char(length) ::= LS. { length = 1; }
json_char(length) ::= LN. { length = 1; }
json_char(length) ::= hexdig. { length = 1; }
json_char(length) ::= SP. { length = 1; }
json_char(length) ::= HTAB. { length = 1; }
json_char(length) ::= COLON. { length = 1; }
json_char(length) ::= COMMA. { length = 1; }
json_char(length) ::= PLUS. { length = 1; }
json_char(length) ::= MINUS. { length = 1; }
json_char(length) ::= DOT. { length = 1; }
json_char(length) ::= control. { length = 1; }
json_char(length) ::= OBSTEXT. { length = 1; }


true ::= LT LR LU LE.
false ::= LF LA LL LS LE.
null ::= LN LU LL LL.

l_crl_brckt ::= LBRACE(c). { puts("VAL_START"); puts(c); openFrame(ps->container.httpRequest, c, PARSED_JSON_OBJECT); }

r_crl_brckt ::= RBRACE(c). { puts("VAL_END"); puts(c); closeFrame(ps->container.httpRequest, c); }

l_sqr_brckt ::= LBRACKET(c). { puts("ARRAY_START"); puts(c); openFrame(ps->container.httpRequest, c, PARSED_JSON_ARRAY); }

r_sqr_brckt ::= RBRACKET(c). { puts("ARRAY_END"); puts(c); closeFrame(ps->container.httpRequest, c); }

json_ows(length) ::= . { length = 0; }
json_ows(length) ::= json_ows SP. { ++length; }
json_ows(length) ::= json_ows HTAB. { ++length; }
json_ows(length) ::= json_ows CLF. { ++length; }
json_ows(length) ::= json_ows CR. { ++length; }


/* ---------------- JSONPATH ---------------- */
mmainn ::= mmain NULL.

mmain ::= jsonpath. {
    const string emptyString = getEmptyString();
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_TERMINATOR)) {
        /* TODO: Fix markJSONPathAsParseFailed !!!! */
        markAsParseFailed(ps);
    };
    markAsParsed(ps); puts("DONE1");
}
mmain ::= jsonpath DOT. {
    const string emptyString = getEmptyString();
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_TERMINATOR)) {
        /* TODO: Fix markJSONPathAsParseFailed !!!! */
        markAsParseFailed(ps);
    };
    markAsParsed(ps); puts("DONE2");
}
mmain ::= jsonpath DOT DOT. {
    const string emptyString = getEmptyString();
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_RECURSIVE)) {
        /* TODO: Fix markJSONPathAsParseFailed !!!! */
        markAsParseFailed(ps);
    };
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_TERMINATOR)) {
            /* TODO: Fix markJSONPathAsParseFailed !!!! */
            markAsParseFailed(ps);
    };
    markAsParsed(ps); puts("DONE3");
}

jsonpath ::= DOLLAR. {
    const string emptyString = getEmptyString();
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_ROOT)) {
        markAsParseFailed(ps);
    }
}

%type dotobjectname {string}
%type objectname {string}
%type jsonpath_char {string}
%type arrayindex {string}
jsonpath ::= jsonpath DOT dotobjectname(var_s). {
    const string emptyString = getEmptyString();
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_OBJECT)) {
        markAsParseFailed(ps);
    }
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &var_s, JSONPATH_REQUEST_NAME)) {
        markAsParseFailed(ps);
    }
}
jsonpath ::= jsonpath DOT ASTERISK. {
    const string emptyString = getEmptyString();
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_OBJECT)) {
        markAsParseFailed(ps);
    }
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_ANY)) {
        markAsParseFailed(ps);
    }
}
jsonpath ::= jsonpath LBRACKET APOSTROPHE objectname(var_s) APOSTROPHE RBRACKET. {
    const string emptyString = getEmptyString();
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_OBJECT)) {
        markAsParseFailed(ps);
    }
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &var_s, JSONPATH_REQUEST_NAME)) {
        markAsParseFailed(ps);
    }
}
jsonpath ::= jsonpath LBRACKET arrayindex(var_s) RBRACKET. {
    const string emptyString = getEmptyString();
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_ARRAY)) {
        markAsParseFailed(ps);
    }
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &var_s, JSONPATH_REQUEST_INDEX)) {
        markAsParseFailed(ps);
    }
}
jsonpath ::= jsonpath LBRACKET ASTERISK RBRACKET. {
    const string emptyString = getEmptyString();
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_ARRAY)) {
        markAsParseFailed(ps);
    }
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_ANYINDEX)) {
        markAsParseFailed(ps);
    }
}
jsonpath ::= jsonpath DOT DOT dotobjectname(var_s). {
    const string emptyString = getEmptyString();
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_RECURSIVE)) {
        markAsParseFailed(ps);
    }
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &var_s, JSONPATH_REQUEST_NAME)) {
        markAsParseFailed(ps);
    }
}
jsonpath ::= jsonpath DOT DOT ASTERISK. {
    const string emptyString = getEmptyString();
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_RECURSIVE)) {
        markAsParseFailed(ps);
    }
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_ANY)) {
        markAsParseFailed(ps);
    }
}
jsonpath ::= jsonpath DOT DOT LBRACKET APOSTROPHE objectname(var_s) APOSTROPHE RBRACKET. {
    const string emptyString = getEmptyString();
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_RECURSIVE)) {
        markAsParseFailed(ps);
    }
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &var_s, JSONPATH_REQUEST_NAME)) {
        markAsParseFailed(ps);
    }
}
jsonpath ::= jsonpath DOT DOT LBRACKET arrayindex(var_s) RBRACKET. {
    const string emptyString = getEmptyString();
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_RECURSIVE)) {
        markAsParseFailed(ps);
    }
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &var_s, JSONPATH_REQUEST_INDEX)) {
        markAsParseFailed(ps);
    }
}
jsonpath ::= jsonpath DOT DOT LBRACKET ASTERISK RBRACKET. {
    const string emptyString = getEmptyString();
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_RECURSIVE)) {
        markAsParseFailed(ps);
    }
    if (LE_OK != appendJsonPathElementOfHttpRequest(ps->container.httpRequest, &(emptyString), JSONPATH_REQUEST_ANYINDEX)) {
        markAsParseFailed(ps);
    }
}

dotobjectname(var_s) ::= jsonpath_char(var_ch). { var_s.length = var_ch.length; var_s.data = var_ch.data; }
dotobjectname(var_s) ::= BACKSLASH ASTERISK(var_c). { var_s.length = 1; var_s.data = var_c; }
dotobjectname(var_s) ::= dotobjectname jsonpath_char(var_ch). {
    while (var_ch.length > 0) {
        (var_s.data)[var_s.length++] = *(var_ch.data++);
        --(var_ch.length);
    }
}
dotobjectname(var_s) ::= dotobjectname BACKSLASH ASTERISK(var_c). { (var_s.data)[var_s.length] = *var_c; ++(var_s.length); }

/*dotobjectname(var_s) ::= dotobjectname jsonpath_char(var_ch). { (var_s.data)[var_s.length] = *var_ch; ++(var_s.length); }
dotobjectname(var_s) ::= dotobjectname BACKSLASH ASTERISK(var_c). { (var_s.data)[var_s.length] = *var_c; ++(var_s.length); }*/

objectname(var_s) ::= jsonpath_char(var_ch). { var_s.length = var_ch.length; var_s.data = var_ch.data; }
objectname(var_s) ::= BACKSLASH APOSTROPHE(var_c). { var_s.length = 1; var_s.data = var_c; }
objectname(var_s) ::= BACKSLASH LBRACKET(var_c). { var_s.length = 1; var_s.data = var_c; }
objectname(var_s) ::= BACKSLASH RBRACKET(var_c). { var_s.length = 1; var_s.data = var_c; }
objectname(var_s) ::= objectname jsonpath_char(var_ch). {
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

jsonpath_char(var_ch) ::= jsonpath_sym(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
jsonpath_char(var_ch) ::= hexdig(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
jsonpath_char(var_ch) ::= LU(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
jsonpath_char(var_ch) ::= BACKSLASH QUOTATION(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
jsonpath_char(var_ch) ::= BACKSLASH BACKSLASH(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
jsonpath_char(var_ch) ::= BACKSLASH SLASH(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
jsonpath_char(var_ch) ::= BACKSLASH BACKSPACE(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
jsonpath_char(var_ch) ::= BACKSLASH FORMFEED(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
jsonpath_char(var_ch) ::= BACKSLASH CLF(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
jsonpath_char(var_ch) ::= BACKSLASH CR(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
jsonpath_char(var_ch) ::= BACKSLASH HTAB(var_cc). { var_ch.data = var_cc; var_ch.length = 1; }
jsonpath_char(var_ch) ::= BACKSLASH LU hexdig(h1) hexdig(h2) hexdig(h3) hexdig(h4). { var_ch = convertUtf16ToString(h1, h2, *h3, *h4); }