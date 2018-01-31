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
#include "parser.h"
#include "httpRequest.h"
#include "httpRequestInternal.h"
#include "string.h"
}

%name ParseHTTP11
%token_type {char*}
%token_prefix TOK_
%extra_argument {parserState *ps}
%syntax_error  {if (FALSE == isParsed(ps) ) { puts("Systax error"); puts("---"); markAsSyntaxIncorrect(ps);} }
%parse_failure { markAsParseFailed(ps); }

/* https://habrahabr.ru/post/232385/ 
 * https://www.mnot.net/blog/2014/06/07/rfc2616_is_dead
 */

/* Declare unusable token */
%token_class control CONTROL.

/* !"#$%&'()*+,-./01234567890:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~ */
%token_class vchar EXCLAMATION|QUOTATION|OCTOTHORPE|DOLLAR|PERCENT|AMPERSAND|APOSTROPHE|LPARENTHESIS|RPARENTHESIS|ASTERISK|PLUS|COMMA|MINUS|DOT|SLASH|ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|COLON|SEMICOLON|LESSTHAN|EQUALS|GREATERTHAN|QUESTION|AT|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|LBRACKET|BACKSLASH|RBRACKET|CARET|UNDERSCORE|BACKQUOTE|LA|LB|LC|LD|LE|LF|LG|LH|LI|LJ|LK|LL|LM|LN|LO|LP|LQ|LR|LS|LT|LU|LV|LW|LX|LY|LZ|LBRACE|VBAR|RBRACE|TILDE.

%token_class digit ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE.

/* tchar = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." /
    "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA */
%token_class tchar EXCLAMATION|OCTOTHORPE|DOLLAR|PERCENT|AMPERSAND|APOSTROPHE|ASTERISK|PLUS|MINUS|DOT|CARET|UNDERSCORE|BACKQUOTE|VBAR|TILDE|ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|LA|LB|LC|LD|LE|LF|LG|LH|LI|LJ|LK|LL|LM|LN|LO|LP|LQ|LR|LS|LT|LU|LV|LW|LX|LY|LZ.

/* unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~" */
%token_class unreserved A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|LA|LB|LC|LD|LE|LF|LG|LH|LI|LJ|LK|LL|LM|LN|LO|LP|LQ|LR|LS|LT|LU|LV|LW|LX|LY|LZ|ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|MINUS|DOT|UNDERSCORE|TILDE.

/* unreserved_dot_ex = ALPHA / DIGIT / "-" / "_" / "~" */
%token_class unreserved_dot_ex A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|LA|LB|LC|LD|LE|LF|LG|LH|LI|LJ|LK|LL|LM|LN|LO|LP|LQ|LR|LS|LT|LU|LV|LW|LX|LY|LZ|ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|MINUS|UNDERSCORE|TILDE.

/* sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "=" */        
%token_class subdelims EXCLAMATION|DOLLAR|AMPERSAND|APOSTROPHE|LPARENTHESIS|RPARENTHESIS|ASTERISK|PLUS|COMMA|SEMICOLON|EQUALS.

/* sub-delims-kv = "!" / "$" / "'" / "(" / ")" / "*" / "+" / "," / ";" */        
%token_class subdelims_kv EXCLAMATION|DOLLAR|APOSTROPHE|LPARENTHESIS|RPARENTHESIS|ASTERISK|PLUS|COMMA|SEMICOLON.

%token_class hexdig ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|A|B|C|D|E|F|LA|LB|LC|LD|LE|LF.

%fallback HSP SP.

%fallback HHTAB HTAB.

%wildcard ANY.

/* HTTP-message   = start-line
                    *( header-field CRLF )
                    CRLF
                    [ message-body ] 
 * (Here message body ignored as a parsing entity.)
 */

/* start-line     = request-line / status-line 
 * (Here request only.)
 */
http_message ::= request_line http_headers finalcrlf ANY.


/* request-line   = method SP request-target SP HTTP-version CRLF */
request_line ::= method SP request_target SP http_version crlf.

/* method = token  */
%type token {string}
method ::= token(var_s). {
    if (NULL == appendElementOfHttpRequest(ps->request, &var_s, METHOD)) {
        markAsParseFailed(ps);
    }
}

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
        if (NULL == appendElementOfHttpRequest(ps->request, &var_s, URI)) {
            markAsParseFailed(ps);
        }
    }
}
request_target ::= absolute_path(var_s) QUESTION query. {
    if (LE_OK != decodeValue( &(var_s) , FALSE)) {
        markAsParseFailed(ps);
    } else {
        if (NULL == appendElementOfHttpRequest(ps->request, &var_s, URI)) {
            markAsParseFailed(ps);
        }
    }
}

/* absolute-path  = 1*( "/" segment )*/
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
pchar(var_p) ::= unreserved_dot_ex(var_c). { *var_c = tolower(*var_c); var_p.data = var_c; var_p.length = 1; }
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
        requestElement *parent, *child;
        if ((NULL == (parent = appendElementOfHttpRequest(ps->request, &var_k, GET_QUERY_ELEMENT))) ||
                (NULL == (child = getEmptyValueElement(ps->request)))) {
            markAsParseFailed(ps);
        } else {
            if (LE_OK != linkRequestElement(parent, child)) {
                markAsParseFailed(ps);
            }
        }
    }
}
key_val ::= key(var_k) EQUALS val(var_l). {
    if ((LE_OK != decodeValue( &(var_k) , TRUE)) ||
            (LE_OK != decodeValue( &(var_l) , TRUE))) {
        markAsParseFailed(ps);
    } else {
        requestElement *parent, *child;
        if ((NULL == (parent = appendElementOfHttpRequest(ps->request, &var_k, GET_QUERY_ELEMENT))) ||
                (NULL == (child = appendElementOfHttpRequest(ps->request, &var_l, VALUE)))) {
            markAsParseFailed(ps);
        } else {
            if (LE_OK != linkRequestElement(parent, child)) {
                markAsParseFailed(ps);
            }
        }
    }
}

%type pchar_kv {string}
key(var_k) ::= pchar_kv(var_c). { var_k.length = var_c.length; var_k.data = var_c.data; }
key(var_k) ::= key pchar_kv(var_c). { var_k.length += var_c.length; }


val(var_l) ::= pchar_kv(var_c). { var_l.length = var_c.length; var_l.data = var_c.data; }
val(var_l) ::= val(var_l) pchar_kv(var_c). { var_l.length += var_c.length; }

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
    if (NULL == appendElementOfHttpRequest(ps->request, &var_s, HTTP_VERSION)) {
        markAsParseFailed(ps);
    }
}

/* HTTP-message   = start-line
                    *( header-field CRLF )
                    ... */
http_headers ::= .
http_headers ::= http_headers header_field crlf.

/* header-field   = field-name ":" OWS field-value OWS */
/* field-name     = token */
/* field-value    = *( field-content / obs-fold ) */
/* There is no support of obs-fold, because of parsing conflicts. So reduce 
 * some rules. */
%type field_content {string}
header_field ::= token(var_k) COLON ows. {
    requestElement *parent, *child;
    if ((NULL == (parent = appendElementOfHttpRequest(ps->request, &var_k, HEADER))) ||
                (NULL == (child = getEmptyValueElement(ps->request)))) {
        markAsParseFailed(ps);
    } else {
        if (LE_OK != linkRequestElement(parent, child)) {
            markAsParseFailed(ps);
        }
    }
}
header_field ::= token(var_k) COLON ows field_content(var_v) ows. {
    if (LE_OK != trim(&var_v)) {
        markAsParseFailed(ps);
    } else {
        requestElement *parent, *child;
        if ((NULL == (parent = appendElementOfHttpRequest(ps->request, &var_k, HEADER))) ||
                (NULL == (child = appendElementOfHttpRequest(ps->request, &var_v, VALUE)))) {
            markAsParseFailed(ps);
        } else {
            if (LE_OK != linkRequestElement(parent, child)) {
                markAsParseFailed(ps);
            }
        }
    }
}

/* field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ] */
field_content(var_v) ::= field_vchar(var_c). { var_v.length = 1; var_v.data = var_c; }
field_content(var_v) ::= field_content field_vchar. { ++(var_v.length); }

/* field-vchar    = VCHAR / obs-text */
field_vchar ::= vchar.
field_vchar ::= HSP.
field_vchar ::= HHTAB.
field_vchar ::= OBSTEXT.

crlf ::= CR CLF.
finalcrlf ::= CR CLF. {markAsParsed(ps); puts("DONE");}

ows ::= .
ows ::= ows SP.
ows ::= ows HTAB.