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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../3rdParty/unity/git/src/unity.h"
#include "../src/lemonHttp/parser.h"
#include "../src/lemonHttp/string.h"
#include "../src/net/socket.h"

#include "fakeDescriptor.h"

#define TESTNAME normalizePath

void setUp(void) {

}

void tearDown(void) {

}

static void test1(void) {
    httpRequest request;
    string* out;
    
    const char* rawRequest = "GET /a/b/././c HTTP/1.1\r\n\r\n";

    TEST_ASSERT_EQUAL(OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/a/b/c", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);
}

static void test2(void) {
    httpRequest request;
    string* out;
    
    const char* rawRequest = "GET /a/bxxxxx/../c HTTP/1.1\r\n\r\n";

    TEST_ASSERT_EQUAL(OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/a/c", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);
}

static void test3(void) {
    httpRequest request;
    string* out;
    
    const char* rawRequest = "GET /a/b/../../c HTTP/1.1\r\n\r\n";

    TEST_ASSERT_EQUAL(OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/c", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);
}

static void test4(void) {
    httpRequest request;
    string* out;
    
    const char* rawRequest = "GET /a/b/../../../c HTTP/1.1\r\n\r\n";

    TEST_ASSERT_EQUAL(OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/c", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);
}

static void test5(void) {
    httpRequest request;
    string* out;
    
    const char* rawRequest = "GET /a/b/../../.././.././././.././../../c HTTP/1.1\r\n\r\n";

    TEST_ASSERT_EQUAL(OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/c", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);
}

static void test6(void) {
    httpRequest request;
    string* out;
    
    const char* rawRequest = "GET /a/b/c/d/e/f/g/h/i/../../.././.././././.././../../j HTTP/1.1\r\n\r\n";

    TEST_ASSERT_EQUAL(OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/a/b/j", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);
}

static void test7(void) {
    httpRequest request;
    string* out;
    
    const char* rawRequest = "GET /a/b/../c/././d HTTP/1.1\r\n\r\n";

    TEST_ASSERT_EQUAL(OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/a/c/d", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);
}

static void test8(void) {
    httpRequest request;
    string* out;
    
    const char* rawRequest = "GET /A/b/c/%7bfoo%7d HTTP/1.1\r\n\r\n";

    TEST_ASSERT_EQUAL(OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/a/b/c/{foo}", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);
}

static void test9(void) {
    httpRequest request;
    string* out;
    
    const char* rawRequest = "GET /host/%7Euser/x/y/z HTTP/1.1\r\n\r\n";

    TEST_ASSERT_EQUAL(OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/host/~user/x/y/z", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);
}

static void test10(void) {
    httpRequest request;
    string* out;
    
    const char* rawRequest = "GET /host/%7euser/x/y/z HTTP/1.1\r\n\r\n";

    TEST_ASSERT_EQUAL(OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/host/~user/x/y/z", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);
}


int main() {
    UnityBegin(__FILE__);
    RUN_TEST(test1);
    RUN_TEST(test2);
    RUN_TEST(test3);
    RUN_TEST(test4);
    RUN_TEST(test5);
    RUN_TEST(test6);
    RUN_TEST(test7);
    RUN_TEST(test8);
    RUN_TEST(test9);
    RUN_TEST(test10);
    return (UnityEnd());
}
