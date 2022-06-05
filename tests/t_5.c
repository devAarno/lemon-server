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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../3rdParty/unity/git/src/unity.h"
#include "../src/lemonHttp/parser.h"
#include "../src/lemonHttp/string.h"
#include "../src/lemonHttp/lemonError.h"

#include "fakeDescriptor.h"

#define TESTNAME normalizePath

typedef struct {
    string expectedValue;
    char callCounter;
} calledCallback;

void setUp(void) {

}

void tearDown(void) {

}

static const lemonError fakeExecute(const string *value, calledCallback *data) {
    printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    TEST_ASSERT_EQUAL(data->expectedValue.length, value->length);
    TEST_ASSERT_EQUAL_STRING_LEN(data->expectedValue.data, value->data, value->length);
    ++(data->callCounter);
    return LE_OK;
}

static void test1(void) {
    httpRequest request;

    const char* rawRequest = "GET /a/b/././c HTTP/1.1\r\n\r\n";

    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/a/b/c";
    uriCallback.expectedValue.length = 6;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
}

static void test2(void) {
    httpRequest request;

    const char* rawRequest = "GET /a/bxxxxx/../c HTTP/1.1\r\n\r\n";

    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/a/c";
    uriCallback.expectedValue.length = 4;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
}

static void test3(void) {
    httpRequest request;

    const char* rawRequest = "GET /a/b/../../c HTTP/1.1\r\n\r\n";

    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/c";
    uriCallback.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
}

static void test4(void) {
    httpRequest request;

    const char* rawRequest = "GET /a/b/../../../c HTTP/1.1\r\n\r\n";

    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/c";
    uriCallback.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
}

static void test5(void) {
    httpRequest request;

    const char* rawRequest = "GET /a/b/../../.././.././././.././../../c HTTP/1.1\r\n\r\n";

    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/c";
    uriCallback.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
}

static void test6(void) {
    httpRequest request;

    const char* rawRequest = "GET /a/b/c/d/e/f/g/h/i/../../.././.././././.././../../j HTTP/1.1\r\n\r\n";

    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/a/b/j";
    uriCallback.expectedValue.length = 6;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
}

static void test7(void) {
    httpRequest request;

    const char* rawRequest = "GET /a/b/../c/././d HTTP/1.1\r\n\r\n";

    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/a/c/d";
    uriCallback.expectedValue.length = 6;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
}

static void test8(void) {
    httpRequest request;

    const char* rawRequest = "GET /A/b/c/%7bfoo%7d HTTP/1.1\r\n\r\n";

    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/A/b/c/{foo}";
    uriCallback.expectedValue.length = 12;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
}

static void test9(void) {
    httpRequest request;

    const char* rawRequest = "GET /host/%7Euser/x/y/z HTTP/1.1\r\n\r\n";

    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/host/~user/x/y/z";
    uriCallback.expectedValue.length = 17;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
}

static void test10(void) {
    httpRequest request;

    const char* rawRequest = "GET /host/%7euser/x/y/z HTTP/1.1\r\n\r\n";

    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/host/~user/x/y/z";
    uriCallback.expectedValue.length = 17;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
}

static void test11(void) {
    httpRequest request;

    const char* rawRequest = "GET /..//..///a//./././///../../b/c/.%64%65%66/test/./././///././//me/////./.././../.%7a%7A.///./. HTTP/1.1\r\n\r\n";

    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/b/c/.def/.zz."; /* Really? Not `/b/c/.def/.zz./`? */
    uriCallback.expectedValue.length = 14;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
}

static void test12(void) {
    httpRequest request;

    const char* rawRequest = "GET /..//..///a//./././///../../b/c/.%64%65%66/test/./././///././//me/////./.././../.%7a%7A././../.././.qq HTTP/1.1\r\n\r\n";

    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/b/c/.qq";
    uriCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
}

static void test13(void) {
    httpRequest request;

    const char* rawRequest = "POST /test/me/./again/.and./fake1/fake2/../fake3/../../again/index.html?i=100&j=-7&temp=Kelvin HTTP/1.1\r\nHost:          localhost\r\ncomplex: test1;test2;test3    groovy  mmm   \r\nHozt:z=z&z?z:z127.0.0.1\r\n\r\n";

    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "POST";
    methodCallback.expectedValue.length = 4;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/test/me/again/.and./again/index.html";
    uriCallback.expectedValue.length = 37;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
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
    RUN_TEST(test11);
    RUN_TEST(test12);
    RUN_TEST(test13);
    return (UnityEnd());
}
