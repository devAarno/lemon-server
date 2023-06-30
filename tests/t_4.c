/*
 * Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022, 2023 Parkhomenko Stanislav
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
#include "../src/lemonHttp/lemonError.h"

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

typedef struct {
    string expectedValue;
    char callCounter;
} calledCallback;

void setUp(void) {
}

void tearDown(void) {
}

static lemonError fakeExecute(const string *value, calledCallback *data) {
    printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    TEST_ASSERT_EQUAL(data->expectedValue.length, value->length);
    TEST_ASSERT_EQUAL_STRING_LEN(data->expectedValue.data, value->data, value->length);
    ++(data->callCounter);
    return LE_OK;
}

static void test_correctValues1(void) {
    httpRequest request;
    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;
    calledCallback helloCallback1;
    calledCallback value1Callback1;
    calledCallback value2Callback1;
    calledCallback testCallback1;
    calledCallback yesCallback1;
    calledCallback clientCallback1;
    calledCallback serverCallback1;
    calledCallback thisCallback1;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/1 1.jpg";
    uriCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    helloCallback1.callCounter = 0;
    helloCallback1.expectedValue.data = "world";
    helloCallback1.expectedValue.length = 5;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "hello", fakeExecute, &helloCallback1));

    value1Callback1.callCounter = 0;
    value1Callback1.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "value1", fakeExecute, &value1Callback1));

    value2Callback1.callCounter = 0;
    value2Callback1.expectedValue.data = "    ";
    value2Callback1.expectedValue.length = 4;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "value2", fakeExecute, &value2Callback1));

    testCallback1.callCounter = 0;
    testCallback1.expectedValue.data = "te st";
    testCallback1.expectedValue.length = 5;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "test", fakeExecute, &testCallback1));

    yesCallback1.callCounter = 0;
    yesCallback1.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "yes", fakeExecute, &yesCallback1));

    clientCallback1.callCounter = 0;
    clientCallback1.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "Client", fakeExecute, &clientCallback1));

    serverCallback1.callCounter = 0;
    serverCallback1.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "Server", fakeExecute, &serverCallback1));

    thisCallback1.callCounter = 0;
    thisCallback1.expectedValue.data = "is+a value";
    thisCallback1.expectedValue.length = 10;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "this", fakeExecute, &thisCallback1));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
    TEST_ASSERT_EQUAL(1, helloCallback1.callCounter);

    TEST_ASSERT_EQUAL(1, value1Callback1.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, value1Callback1.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(value1Callback1.expectedValue)));

    TEST_ASSERT_EQUAL(1, value2Callback1.callCounter);

    TEST_ASSERT_EQUAL(1, testCallback1.callCounter);

    TEST_ASSERT_EQUAL(1, yesCallback1.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, yesCallback1.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(yesCallback1.expectedValue)));

    TEST_ASSERT_EQUAL(1, clientCallback1.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, clientCallback1.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(clientCallback1.expectedValue)));

    TEST_ASSERT_EQUAL(1, serverCallback1.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, serverCallback1.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(serverCallback1.expectedValue)));

    TEST_ASSERT_EQUAL(1, thisCallback1.callCounter);
}

static void test_correctValues2(void) {
    httpRequest request;
    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;
    calledCallback aCallback1;
    calledCallback sixtyCallback1;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    strncpy(request.privateBuffer, rawRequest2, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/f";
    uriCallback.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    aCallback1.callCounter = 0;
    aCallback1.expectedValue.data = "b";
    aCallback1.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "a", fakeExecute, &aCallback1));

    sixtyCallback1.callCounter = 0;
    sixtyCallback1.expectedValue.data = "%63";
    sixtyCallback1.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "%60", fakeExecute, &sixtyCallback1));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
    TEST_ASSERT_EQUAL(1, aCallback1.callCounter);
    TEST_ASSERT_EQUAL(1, sixtyCallback1.callCounter);
}

static void test_incorrectValues1(void) {
    httpRequest request;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    strncpy(request.privateBuffer, rawRequest3a, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(LE_INCORRECT_SYNTAX, parseHTTP(&request));
}

static void test_incorrectValues2(void) {
    httpRequest request;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    strncpy(request.privateBuffer, rawRequest3b, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(LE_INCORRECT_SYNTAX, parseHTTP(&request));
}

static void test_incorrectValues3(void) {
    httpRequest request;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    strncpy(request.privateBuffer, rawRequest3c, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(LE_INCORRECT_SYNTAX, parseHTTP(&request));
}

static void test_incorrectValues4(void) {
    httpRequest request;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    strncpy(request.privateBuffer, rawRequest3d, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(LE_INCORRECT_SYNTAX, parseHTTP(&request));
}

static void test_incorrectValues5(void) {
    httpRequest request;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    strncpy(request.privateBuffer, rawRequest3e, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(LE_INCORRECT_SYNTAX, parseHTTP(&request));
}

static void test_incorrectValues6(void) {
    httpRequest request;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    strncpy(request.privateBuffer, rawRequest3f, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(LE_INCORRECT_SYNTAX, parseHTTP(&request));
}

static void test_correctValues3(void) {
    httpRequest request;
    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;
    calledCallback aCallback1;
    calledCallback sixtyCallback1;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    strncpy(request.privateBuffer, rawRequest3g, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/f";
    uriCallback.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    aCallback1.callCounter = 0;
    aCallback1.expectedValue.data = "b";
    aCallback1.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "a", fakeExecute, &aCallback1));

    sixtyCallback1.callCounter = 0;
    sixtyCallback1.expectedValue.data = "%6";
    sixtyCallback1.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "%6", fakeExecute, &sixtyCallback1));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
    TEST_ASSERT_EQUAL(1, aCallback1.callCounter);
    TEST_ASSERT_EQUAL(1, sixtyCallback1.callCounter);
}

static void test_correctValues4(void) {
    httpRequest request;
    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;
    calledCallback aCallback1;
    calledCallback sixtyCallback1;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    strncpy(request.privateBuffer, rawRequest3h, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/f";
    uriCallback.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    aCallback1.callCounter = 0;
    aCallback1.expectedValue.data = "b";
    aCallback1.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "a", fakeExecute, &aCallback1));

    sixtyCallback1.callCounter = 0;
    sixtyCallback1.expectedValue.data = "%6z";
    sixtyCallback1.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "%6z", fakeExecute, &sixtyCallback1));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
    TEST_ASSERT_EQUAL(1, aCallback1.callCounter);
    TEST_ASSERT_EQUAL(1, sixtyCallback1.callCounter);
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
