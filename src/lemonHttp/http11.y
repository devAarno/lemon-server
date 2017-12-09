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

%include {
#include <assert.h>
#include "parser.h"
#include "http_request.h"
#include "http_request_int.h"
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

%token_class digit ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE.
%token_class hex ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|A|B|C|D|E|F.
%token_class hexl ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|AL|BL|CL|DL|EL|FL.
%token_class letter A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z.
%token_class letterl AL|BL|CL|DL|EL|FL|GL|HL|IL|JL|KL|LL|ML|NL|OL|PL|QL|RL|SL|TL|UL|VL|WL|XL|YL|ZL.
%token_class alpha A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|AL|BL|CL|DL|EL|FL|GL|HL|IL|JL|KL|LL|ML|NL|OL|PL|QL|RL|SL|TL|UL|VL|WL|XL|YL|ZL.

/* tchar = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." /
    "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA */
%token_class tchar EXCLAMATION|OCTOTHORPE|DOLLAR|PERCENT|AMPERSAND|APOSTROPHE|ASTERISK|PLUS|MINUS|DOT|CARET|UNDERSCORE|BACKQUOTE|VBAR|TILDE|ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|AL|BL|CL|DL|EL|FL|GL|HL|IL|JL|KL|LL|ML|NL|OL|PL|QL|RL|SL|TL|UL|VL|WL|XL|YL|ZL.

/* unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~" */
%token_class unreserved A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|AL|BL|CL|DL|EL|FL|GL|HL|IL|JL|KL|LL|ML|NL|OL|PL|QL|RL|SL|TL|UL|VL|WL|XL|YL|ZL|ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|MINUS|DOT|UNDERSCORE|TILDE.

/* sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "=" */        
%token_class subdelims EXCLAMATION|DOLLAR|AMPERSAND|APOSTROPHE|LPARENTHESIS|RPARENTHESIS|ASTERISK|PLUS|COMMA|SEMICOLON|EQ.

/* sub-delims-kv = "!" / "$" / "'" / "(" / ")" / "*" / "+" / "," / ";" */        
%token_class subdelims_kv EXCLAMATION|DOLLAR|APOSTROPHE|LPARENTHESIS|RPARENTHESIS|ASTERISK|PLUS|COMMA|SEMICOLON.

%token_class hexdig ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|A|B|C|D|E|F|AL|BL|CL|DL|EL|FL.

/* !"#$%&'()*+,-./01234567890:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~ */
%token_class vchar EXCLAMATION|QUOTATION|OCTOTHORPE|DOLLAR|PERCENT|AMPERSANDERSAND|APOSTROPHE|LPARENTHESIS|RPARENTHESIS|ASTERISK|PLUS|COMMA|MINUS|DOT|SLASH|ZERO|ONE|TWO|THREE|FOUR|FIVE|SIX|SEVEN|EIGHT|NINE|COLON|SEMICOLON|LESSTHAN|EQUALS|GREATERTHAN|AT|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|LBRACKET|BACKSLASH|RBRACKET|CARET|UNDERSCORE|BACKQUOTE|AL|BL|CL|DL|EL|FL|GL|HL|IL|JL|KL|LL|ML|NL|OL|PL|QL|RL|SL|TL|UL|VL|WL|XL|YL|ZL|LBRACE|VBAR|RBRACE|TILDE.

main ::= http_request.

/* HTTP-message   = start-line
                    *( header-field CRLF )
                    CRLF
                    [ message-body ] 
 * (Here message body ignored as a parsing entity.)
 */

/* start-line     = request-line / status-line 
 * (Here request only.)
 */
http_request ::= request_line http_headers crlf. {markAsParsed(ps); puts("DONE");}


/* request-line   = method SP request-target SP HTTP-version CRLF */
request_line ::= method SP request_target SP http_version crlf.

/* method = token  */
method ::= token.

/* token = 1*tchar */
token ::= tchar.
token ::= token tchar.


/*method ::= STRING(X). { 
    puts("Method:");
    puts(X);
  appendElementOfHttpRequest(ps->request, X, ps->length, METHOD);
}*/

/* request-target = origin-form
                    / absolute-form
                    / authority-form
                    / asterisk-form 
 * (Here only origin-form.)
 */
/* origin-form    = absolute-path [ "?" query ] */

/* pct-encoded    = "%" HEXDIG HEXDIG */
/* sub-delims     */
request_target ::= absolute_path.
request_target ::= absolute_path QST query.

/* absolute-path  = 1*( "/" segment )*/
absolute_path ::= SLASH segment.
absolute_path ::= absolute_path SLASH segment.

/* segment        = *pchar */
segment ::= .
segment ::= segment pchar.

/* pchar          = unreserved / pct-encoded / sub-delims / ":" / "@" */
pchar ::= unreserved.
pchar ::= pctencoded.
pchar ::= subdelims.
pchar ::= CLN.
pchar ::= ATSIGN.

/* pct-encoded    = "%" HEXDIG HEXDIG */
pctencoded ::= PERCENT hexdig hexdig.
        
/*absolute_path ::= STRING(X). {
    puts("Uri:");
    puts(X);
  appendElementOfHttpRequest(ps->request, X, ps->length, URI);
}*/
        
/*absolute_path ::= path.
path ::= SLASH.
path ::= SLASH ident.
path ::= path filler SLASH ident.
path ::= path filler DOT DOT SLASH.*/

/*absolute_path ::= path.

path ::= SLASH.
path(P) ::= path filler STRING(X) SLASH. {puts("JJJJJJJ"); puts(P); P, X }
path ::= path filler DOT DOT SLASH.

filler ::= .
filler ::= filler SLASH.
filler ::= filler DOT SLASH.*/

/* query          = *( pchar / "/" / "?" ) 
 * (In genral case, but we go deeper. Let's exclude '=' and '&'
 * from subdelims and get a key-value pairs.)
 */
query ::= .
query ::= key_val.
query ::= query AMPERSAND key_val.
key_val ::= key EQ.
key_val ::= key EQ val.

key ::= pchar_kv.
val ::= pchar_kv.

pchar_kv ::= unreserved.
pchar_kv ::= pctencoded.
pchar_kv ::= subdelims_kv.
pchar_kv ::= CLN.
pchar_kv ::= ATSIGN.

/*key ::= STRING(K). {
  appendElementOfHttpRequest(ps->request, K, ps->length, GET_QUERY_ELEMENT);
}
val ::= STRING(V). {
  appendElementOfHttpRequest(ps->request, V, ps->length, VALUE);
}*/

/* HTTP-version  = HTTP-name "/" DIGIT "." DIGIT */
/* HTTP-name     = %x48.54.54.50 ; "HTTP", case-sensitive */

http_version ::= H T T P SLASH digit DOT digit.

/*http_version ::= STRING(X). {
    puts("HTTP VER:");
    puts(X);
  appendElementOfHttpRequest(ps->request, X, ps->length, HTTP_VERSION);
}*/

/* HTTP-message   = start-line
                    *( header-field CRLF )
                    ... */
http_headers ::= .
http_headers ::= http_headers header_field crlf.

/* header-field   = field-name ":" OWS field-value OWS */
/* header_field ::= field_name CLN ows field_value ows. */
header_field ::= field_name CLN ows field_value.

/* field-name     = token */
field_name ::= token.

/* field-value    = *( field-content / obs-fold ) */
field_value ::= .
field_value ::= field_value field_content.
/*field_value ::= field_value obs_fold.*/

/* field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ] */
field_content ::= field_vchar.
field_content ::= field_vchar SP ows field_vchar.
field_content ::= field_vchar HTAB ows field_vchar.

/* obs-fold       = CRLF 1*( SP / HTAB ) */
/*obs_fold ::= crlf SP ows.
obs_fold ::= crlf HTAB ows.*/

/* field-vchar    = VCHAR / obs-text */
field_vchar ::= vchar.
field_vchar ::= OBSTEXT.

/*http_header ::= header_name CLN field_value ows. {
  puts("ENABLE ALL");
  (ps->stopSymbols)[':'] = TOK_CLN;
  (ps->stopSymbols)['&'] = TOK_AMP;
  (ps->stopSymbols)['='] = TOK_EQ;
  (ps->stopSymbols)['?'] = TOK_QST;
}

header_name ::= STRING(K). {
    printf("xxxOFFF %.*s!\n", (int)ps->length, K);
  appendElementOfHttpRequest(ps->request, K, ps->length, HEADER);
  (ps->stopSymbols)[':'] = (ps->stopSymbols)['&'] = (ps->stopSymbols)['='] = (ps->stopSymbols)['?'] = 0;
}

field_value ::= .
field_value ::= field_value ows STRING(V). {
  printf("OFFF %.*s!\n", (int)ps->length, V);
  appendElementOfHttpRequest(ps->request, V, ps->length, VALUE);
}*/

ows ::= .
ows ::= ows SP.
ows ::= ows HTAB.

crlf ::= CR LF.
