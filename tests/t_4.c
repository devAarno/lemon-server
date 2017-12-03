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

#include <string.h>

#include "../3rdParty/unity/git/src/unity.h"
#include "../src/lemonHttp/parser.h"
#include "../src/lemonHttp/string.h"

#include "fakeDescriptor.h"

#define TESTNAME DecodeValues

static const char* rawRequest1 = "GET /1%201.%6apg?heLLo=world&vAlue1=&value2=++%20+&TesT=te+st&Yes= HTTP/1.1\r\nClient:           \r\nServer:\r\nThis:   is+a value  \r\n\r\n";
static const char* rawRequest2 = "GET /%66?%61=%62 HTTP/1.1\r\n%60: %63  \r\n\r\n";
static const char* rawRequest3a = "GET /%6 HTTP/1.1\r\n\r\n";
static const char* rawRequest3b = "GET /%66?%6=%62 HTTP/1.1\r\n\r\n";
static const char* rawRequest3c = "GET /%66?%61=%6 HTTP/1.1\r\n\r\n";
static const char* rawRequest3d = "GET /%6z HTTP/1.1\r\n\r\n";
static const char* rawRequest3e = "GET /%66?%6z=%62 HTTP/1.1\r\n\r\n";
static const char* rawRequest3f = "GET /%66?%61=%6z HTTP/1.1\r\n\r\n";
static const char* rawRequest3g = "GET /%66?%61=%62 HTTP/1.1\r\n%6: %6  \r\n\r\n";
static const char* rawRequest3h = "GET /%66?%61=%62 HTTP/1.1\r\n%6z: %6z  \r\n\r\n";

void setUp(void) {
}

void tearDown(void) {
}

static void test_correctValues1(void) {
    httpRequest request;
    string* out;

    initHttpRequest(&request, FAKE_DESCRIPTOR);

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/1 1.jpg", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);



    out = (string *) getQueryParameterOfHttpRequest(&request, "hello");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("world", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "value1");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "value2");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("    ", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "test");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("te st", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "yes");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);


    out = (string *) getHeaderOfHttpRequest(&request, "Client");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "Server");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "This");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("is+a value", out->data, out->length);
}

static void test_correctValues2(void) {
    httpRequest request;
    string* out;

    initHttpRequest(&request, FAKE_DESCRIPTOR);

    strncpy(request.privateBuffer, rawRequest2, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/f", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);



    out = (string *) getQueryParameterOfHttpRequest(&request, "a");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("b", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "%60");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("%63", out->data, out->length);
}

static void test_incorrectValues1(void) {
    httpRequest request;

    initHttpRequest(&request, FAKE_DESCRIPTOR);

    strncpy(request.privateBuffer, rawRequest3a, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(PARSING_IS_FAILED, parse(&request));
}

static void test_incorrectValues2(void) {
    httpRequest request;

    initHttpRequest(&request, FAKE_DESCRIPTOR);

    strncpy(request.privateBuffer, rawRequest3b, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(PARSING_IS_FAILED, parse(&request));
}

static void test_incorrectValues3(void) {
    httpRequest request;

    initHttpRequest(&request, FAKE_DESCRIPTOR);

    strncpy(request.privateBuffer, rawRequest3c, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(PARSING_IS_FAILED, parse(&request));
}

static void test_incorrectValues4(void) {
    httpRequest request;

    initHttpRequest(&request, FAKE_DESCRIPTOR);

    strncpy(request.privateBuffer, rawRequest3d, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(PARSING_IS_FAILED, parse(&request));
}

static void test_incorrectValues5(void) {
    httpRequest request;

    initHttpRequest(&request, FAKE_DESCRIPTOR);

    strncpy(request.privateBuffer, rawRequest3e, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(PARSING_IS_FAILED, parse(&request));
}

static void test_incorrectValues6(void) {
    httpRequest request;

    initHttpRequest(&request, FAKE_DESCRIPTOR);

    strncpy(request.privateBuffer, rawRequest3f, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(PARSING_IS_FAILED, parse(&request));
}

static void test_correctValues3(void) {
    httpRequest request;
    string* out;

    initHttpRequest(&request, FAKE_DESCRIPTOR);

    strncpy(request.privateBuffer, rawRequest3g, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/f", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);



    out = (string *) getQueryParameterOfHttpRequest(&request, "a");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("b", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "%6");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("%6", out->data, out->length);
}

static void test_correctValues4(void) {
    httpRequest request;
    string* out;

    initHttpRequest(&request, FAKE_DESCRIPTOR);

    strncpy(request.privateBuffer, rawRequest3h, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/f", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);



    out = (string *) getQueryParameterOfHttpRequest(&request, "a");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("b", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "%6z");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("%6z", out->data, out->length);
}

int main() {
    UnityBegin(__FILE__);
    RUN_TEST(test_correctValues1);
    RUN_TEST(test_correctValues2);
    RUN_TEST(test_incorrectValues1);
    RUN_TEST(test_incorrectValues2);
    RUN_TEST(test_incorrectValues3);
    RUN_TEST(test_incorrectValues4);
    RUN_TEST(test_incorrectValues5);
    RUN_TEST(test_incorrectValues6);
    RUN_TEST(test_correctValues3);
    RUN_TEST(test_correctValues4);
    return (UnityEnd());
}
