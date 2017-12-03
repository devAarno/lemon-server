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

%left EQ.
%left AMP.
%left QST CLN.
%left SP HTAB.
%left CR LF.

main ::= http_request.

http_request ::= request_line http_headers crlf. {markAsParsed(ps); puts("DONE");}

request_line ::= method SP request_target SP http_version crlf.
method ::= STRING(X). { 
    puts("Method:");
    puts(X);
  appendElementOfHttpRequest(ps->request, X, ps->length, METHOD);
}

request_target ::= origin_form.
request_target ::= absolute_form.

origin_form ::= absolute_path.
origin_form ::= absolute_path QST query.

absolute_path ::= STRING(X). {
    puts("Uri:");
    puts(X);
  appendElementOfHttpRequest(ps->request, X, ps->length, URI);
}
        
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

query ::= .
query ::= key_val.
query ::= query AMP key_val.
key_val ::= key EQ.
key_val ::= key EQ val.
key ::= STRING(K). {
  appendElementOfHttpRequest(ps->request, K, ps->length, GET_QUERY_ELEMENT);
}
val ::= STRING(V). {
  appendElementOfHttpRequest(ps->request, V, ps->length, VALUE);
}

absolute_form ::= absolute_uri. {puts("NOT SUPPORTED");}
absolute_form ::= absolute_uri QST query. {puts("NOT SUPPORTED");}

absolute_uri ::= STRING(X) CLN STRING(Y). {/* X Y */}

http_version ::= STRING(X). {
    puts("HTTP VER:");
    puts(X);
  appendElementOfHttpRequest(ps->request, X, ps->length, HTTP_VERSION);
}

http_headers ::= .

http_headers ::= http_headers http_header crlf.

http_header ::= header_name CLN field_value ows. {
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
}

ows ::= .
ows ::= ows SP.
ows ::= ows HTAB.

crlf ::= CR LF.
