/*
 * Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022 Parkhomenko Stanislav
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
#include "string.h"
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
%token_class control NULL CONTROL BACKSPACE FORMFEED.

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

%fallback HSP SP.
%fallback HHTAB HTAB.

%wildcard ANY.

main ::= ONE http_message.
main ::= TWO jsonn.
main ::= THREE mmainn.

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
method ::= token(var_s). { executeHttpMethodCallback(ps->request, &var_s); }

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
        executeHttpUriCallback(ps->request, &var_s);
    }
}
request_target ::= absolute_path(var_s) QUESTION query. {
    if (LE_OK != decodeValue( &(var_s) , FALSE)) {
        markAsParseFailed(ps);
    } else {
        executeHttpUriCallback(ps->request, &var_s);
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
        executeGetParameterCallback(ps->request, &var_k, &e);
    }
}
key_val ::= key(var_k) EQUALS val(var_l). {
    if ((LE_OK != decodeValue( &(var_k) , TRUE)) ||
            (LE_OK != decodeValue( &(var_l) , TRUE))) {
        markAsParseFailed(ps);
    } else {
        executeGetParameterCallback(ps->request, &var_k, &var_l);
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
    executeHttpVersionCallback(ps->request, &var_s);
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
    executeHeaderCallback(ps->request, &var_k, &e);
}
header_field ::= token(var_k) COLON field_value(var_v). {
    trim(&var_v);
    executeHeaderCallback(ps->request, &var_k, &var_v);
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

jsonn ::= json NULL.

json ::= json_ows start_root value end_root json_ows. { }

start_root ::= . {  }

end_root ::= . {  }

%type string {string}
%type json_chars {string}
%type json_char {string}
%type json_key {string}
%type array {string}
%type object {string}

%type number {string}

value ::= object(o).  { /* o */ }
value ::= array(a). { /* a */ }

value ::= number(s1). { /* s1 */ }
value ::= string(s1). { /* s1 */ }
value ::= true. { }
value ::= false. { }
value ::= null. { }

object(o) ::= l_crl_brckt(o_start) json_ows r_crl_brckt(o_end). { /* o, o_start, o_end */ }
object(o) ::= l_crl_brckt(o_start) json_ows object_content json_ows r_crl_brckt(o_end). { /* o, o_start, o_end */ }

json_key(s1) ::= string(s1). { /* s1 */ }

object_content ::= json_key(s1) json_ows COLON json_ows value. { /* s1 */ }
object_content ::= object_content json_ows COMMA json_ows json_key(s1) json_ows COLON json_ows value. { /* s1 */ }

array(a) ::= l_sqr_brckt(a_start) json_ows r_sqr_brckt(a_end). { /* a, a_start, a_end */ }
array(a) ::= l_sqr_brckt(a_start) json_ows array_content json_ows r_sqr_brckt(a_end). { /* a, a_start, a_end */ }

array_content ::= value array_inc.
array_content ::= array_content json_ows COMMA json_ows value array_inc.

array_inc ::= . { }

%type mantissa {string}
%type exponent {string}

number(n) ::= MINUS(minus) mantissa(mantissa). { /* n, minus, mantissa */ }
number(n) ::= mantissa(mantissa). { /* n, mantissa */ }
number(n) ::= MINUS(minus) mantissa(mantissa) exponent(exponent). { /* n, minus, mantissa, exponent */ }
number(n) ::= mantissa(mantissa) exponent(exponent). { /* n, mantissa, exponent */ }

mantissa(m) ::= ZERO(z). { /* m, z */ }
mantissa(m) ::= ZERO(z) DOT digits(d). { /* m, z, d */ }
mantissa(m) ::= digit_without_zero(d). { /* m, d */ }
mantissa(m) ::= digit_without_zero(d) DOT digits(ds). { /* m, d, ds */ }
mantissa(m) ::= digit_without_zero(d) digits(ds). { /* m, d, ds */ }
mantissa(m) ::= digit_without_zero(d) digits(ds1) DOT digits(ds2). { /* m, d, ds1, ds2 */ }

exponent(exp) ::= exp(e) sign(s) digits(d). { /* exp, e, s, d */ }

exp ::= LE.
exp ::= E.

%type sign {string}

sign(s) ::= . { /* s */ }
sign(s) ::= PLUS(p). { /* s, p */ }
sign(s) ::= MINUS(m). { /* s, m */ }

%type digits {string}

digits(ds) ::= digit(d). { /* ds, d */ }
digits(ds) ::= digits digit. { /* ds */ }

string(s) ::= QUOTATION QUOTATION. { /* s */ }
string(s) ::= QUOTATION json_chars(cs) QUOTATION. { /* s, cs */ }

json_chars(cs) ::= json_char(c). { /* cs, c */ }
json_chars(cs) ::= json_chars json_char(c). { /* cs, c */ }

json_char(c) ::= json_sym(s). { /* c, s */ }
json_char(c) ::= BACKSLASH QUOTATION(s). { /* c, s */ }
json_char(c) ::= BACKSLASH BACKSLASH(s). { /* c, s */ }
json_char(c) ::= BACKSLASH SLASH(s). { /* c, s */ }
json_char(c) ::= BACKSLASH BACKSPACE(s). { /* c, s */ }
json_char(c) ::= BACKSLASH FORMFEED(s). { /* c, s */ }
json_char(c) ::= BACKSLASH CLF(s). { /* c, s */ }
json_char(c) ::= BACKSLASH CR(s). { /* c, s */ }
json_char(c) ::= BACKSLASH HTAB(s). { /* c, s */ }
json_char(c) ::= BACKSLASH LU hexdig(h1) hexdig(h2) hexdig(h3) hexdig(h4). { /* c, h1, h2, h3, h4 */ }
json_char(c) ::= LT(s). { /* c, s */ }
json_char(c) ::= LR(s). { /* c, s */ }
json_char(c) ::= LU(s). { /* c, s */ }
json_char(c) ::= LL(s). { /* c, s */ }
json_char(c) ::= LS(s). { /* c, s */ }
json_char(c) ::= LN(s). { /* c, s */ }
json_char(c) ::= hexdig(s). { /* c, s */ }
json_char(c) ::= SP(s). { /* c, s */ }
json_char(c) ::= HTAB(s). { /* c, s */ }
json_char(c) ::= COLON(s). { /* c, s */ }
json_char(c) ::= COMMA(s). { /* c, s */ }
json_char(c) ::= PLUS(s). { /* c, s */ }
json_char(c) ::= MINUS(s). { /* c, s */ }
json_char(c) ::= DOT(s). { /* c, s */ }
json_char(c) ::= control(s). { /* c, s */ }
json_char(c) ::= OBSTEXT(s). { /* c, s */ }


true ::= LT LR LU LE.
false ::= LF LA LL LS LE.
null ::= LN LU LL LL.

l_crl_brckt ::= LBRACE(c). { /* c */ }

r_crl_brckt ::= RBRACE(c). { /* c */ }

l_sqr_brckt ::= LBRACKET(c). { /* c */ }

r_sqr_brckt ::= RBRACKET(c). { /* c */ }

json_ows ::= .
json_ows ::= json_ows SP.
json_ows ::= json_ows HTAB.
json_ows ::= json_ows CLF.
json_ows ::= json_ows CR.


/* ---------------- JSONPATH ---------------- */
mmainn ::= mmain NULL.

mmain ::= jsonpath. { /* DONE1 */ }
mmain ::= jsonpath DOT. { /* DONE2 */ }
mmain ::= jsonpath DOT DOT. { /* DONE3 */}

jsonpath ::= DOLLAR. {}

%type dotobjectname {string}
%type objectname {string}
%type jsonpath_char {string}
%type arrayindex {string}
jsonpath ::= jsonpath DOT dotobjectname(var_s). { /* var_s */ }
jsonpath ::= jsonpath DOT ASTERISK. {}
jsonpath ::= jsonpath LBRACKET APOSTROPHE objectname(var_s) APOSTROPHE RBRACKET. {/* var_s */}
jsonpath ::= jsonpath LBRACKET arrayindex(var_s) RBRACKET. {/* var_s */}
jsonpath ::= jsonpath LBRACKET ASTERISK RBRACKET. {}
jsonpath ::= jsonpath DOT DOT dotobjectname(var_s). {/* var_s */}
jsonpath ::= jsonpath DOT DOT ASTERISK. {}
jsonpath ::= jsonpath DOT DOT LBRACKET APOSTROPHE objectname(var_s) APOSTROPHE RBRACKET. {/* var_s */}
jsonpath ::= jsonpath DOT DOT LBRACKET arrayindex(var_s) RBRACKET. {/* var_s */}
jsonpath ::= jsonpath DOT DOT LBRACKET ASTERISK RBRACKET. {}

dotobjectname(var_s) ::= jsonpath_char(var_ch). { /* var_s, var_ch */ }
dotobjectname(var_s) ::= BACKSLASH ASTERISK(var_c). { /* var_s, var_c */ }
dotobjectname(var_s) ::= dotobjectname jsonpath_char(var_ch). { /* var_s, var_ch */ }
dotobjectname(var_s) ::= dotobjectname BACKSLASH ASTERISK(var_c). { /* var_s, var_c */ }

/*dotobjectname(var_s) ::= dotobjectname jsonpath_char(var_ch). { (var_s.data)[var_s.length] = *var_ch; ++(var_s.length); }
dotobjectname(var_s) ::= dotobjectname BACKSLASH ASTERISK(var_c). { (var_s.data)[var_s.length] = *var_c; ++(var_s.length); }*/

objectname(var_s) ::= jsonpath_char(var_ch). { /* var_s, var_ch */ }
objectname(var_s) ::= BACKSLASH APOSTROPHE(var_c). { /* var_s, var_c */ }
objectname(var_s) ::= BACKSLASH LBRACKET(var_c). { /* var_s, var_c */ }
objectname(var_s) ::= BACKSLASH RBRACKET(var_c). { /* var_s, var_c */ }
objectname(var_s) ::= objectname jsonpath_char(var_ch). { /* var_s, var_ch */ }
objectname(var_s) ::= objectname BACKSLASH APOSTROPHE(var_c). { /* var_s, var_c */ }
objectname(var_s) ::= objectname BACKSLASH LBRACKET(var_c). { /* var_s, var_c */ }
objectname(var_s) ::= objectname BACKSLASH RBRACKET(var_c). { /* var_s, var_c */ }

arrayindex(var_s) ::= digit(var_ch). { /* var_s, var_ch */ }
arrayindex(var_s) ::= arrayindex digit(var_ch). { /* var_s, var_ch */ }

jsonpath_char(var_ch) ::= jsonpath_sym(var_cc). { /* var_ch, var_cc */ }
jsonpath_char(var_ch) ::= hexdig(var_cc). { /* var_ch, var_cc */ }
jsonpath_char(var_ch) ::= LU(var_cc). { /* var_ch, var_cc */ }
jsonpath_char(var_ch) ::= BACKSLASH QUOTATION(var_cc). { /* var_ch, var_cc */ }
jsonpath_char(var_ch) ::= BACKSLASH BACKSLASH(var_cc). { /* var_ch, var_cc */ }
jsonpath_char(var_ch) ::= BACKSLASH SLASH(var_cc). { /* var_ch, var_cc */ }
jsonpath_char(var_ch) ::= BACKSLASH BACKSPACE(var_cc). { /* var_ch, var_cc */ }
jsonpath_char(var_ch) ::= BACKSLASH FORMFEED(var_cc). { /* var_ch, var_cc */ }
jsonpath_char(var_ch) ::= BACKSLASH CLF(var_cc). { /* var_ch, var_cc */ }
jsonpath_char(var_ch) ::= BACKSLASH CR(var_cc). { /* var_ch, var_cc */ }
jsonpath_char(var_ch) ::= BACKSLASH HTAB(var_cc). { /* var_ch, var_cc */ }
jsonpath_char(var_ch) ::= BACKSLASH LU hexdig(h1) hexdig(h2) hexdig(h3) hexdig(h4). { /* var_ch, h1 ,h2, h3, h4 */ }